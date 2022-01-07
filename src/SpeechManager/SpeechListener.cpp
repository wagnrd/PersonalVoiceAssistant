//
// Created by Denis Wagner on 4/20/19.
//

#include "include/SpeechManager/SpeechListener.hpp"
#include <include/Utils/Utils.hpp>
#include <include/SpeechManager/SpeechManager.hpp>

#include <QtCore/QDebug>
#include <QtCore/QTimer>
#include <QtMultimedia/QAudioFormat>
#include <QtMultimedia/QAudioDeviceInfo>
#include <QtCore/QTime>

#include <aquila/aquila.h>

SpeechListener::SpeechListener( SpeechManager* speechManager, Utils::BlockingQueue<std::string>& inputTextBuffer )
        : speechManager( speechManager ), inputTextBuffer( inputTextBuffer )
{
    QAudioFormat audioFormat;
    audioFormat.setSampleRate( SAMPLE_RATE );
    audioFormat.setSampleSize( 16 );
    audioFormat.setChannelCount( 1 );
    audioFormat.setCodec( "audio/pcm" );
    audioFormat.setByteOrder( QAudioFormat::BigEndian );
    audioFormat.setSampleType( QAudioFormat::SignedInt );

    // check if audio format is compatible to the input device
    QAudioDeviceInfo audioDeviceInfo;

    if ( audioDeviceInfo.isFormatSupported( audioFormat ) )
    {
        qWarning() << "Audio format is not supported by the input device.";
        audioFormat = audioDeviceInfo.nearestFormat( audioFormat );
        qWarning() << "New format: " << audioFormat;
    }

    audioInput   = new QAudioInput( audioFormat, this );
    audioOutput  = new QAudioOutput( audioFormat, this );
    inputDevice  = audioInput->start();
    outputDevice = audioOutput->start();
    connect( inputDevice, SIGNAL( readyRead() ), this, SLOT( readAudioInput() ) );

    // start thread
    start();
}

void SpeechListener::run()
{
    while ( !isInterruptionRequested() )
    {
        constexpr long maxSleepTime = 500;
        long           deltaTimes   = 0;
        size_t         bufferSize   = 0;

        while ( deltaTimes < maxSleepTime )
        {
            // blocks until the needed index is available
            deltaTimes += audioBuffer[bufferSize].deltaTime;
            ++bufferSize;
        }

        processAudioBuffer( bufferSize );
    }
}


// ********************** //
// *** Slot functions *** //
// ********************** //

void SpeechListener::readAudioInput()
{
    // if eSpeak speaks, no audio data should be recorded
    if ( speechManager->isSpeaking() )
        return;

    QByteArray bytes            = inputDevice->readAll();
    auto       availableSamples = static_cast<size_t>( bytes.size() / 2);
    // for Aquila the array size has to be padded to the next power of 2
    size_t     paddedSize       = Aquila::nextPowerOf2( availableSamples );

    auto audioData = new short[paddedSize];
    Utils::initWithZeros( audioData, paddedSize );
    Utils::mergeBytes<2>( bytes, audioData );
    filterSpeech( audioData, paddedSize );

    double rms = Aquila::rms( Aquila::SignalSource( audioData, paddedSize, SAMPLE_RATE ) );

    // to prevent overfitting of mean, so that sudden rises or drops of overall noise still get noticed
    // and to prevent an overflow in the intern numberOfValues variable
    if ( meanRMS.getNumberOfValues() == 500 )
        meanRMS.reduceByFactor( 2 );

    clock::time_point currentTimeStamp = clock::now();
    milliseconds      elapsedTime      = std::chrono::duration_cast<milliseconds>( currentTimeStamp - lastTimeStamp );
    lastTimeStamp = currentTimeStamp;

    // passedFilter is per default set to false
    EvalAudioChunk evalAudioChunk( audioData, availableSamples, elapsedTime.count() );

    // to make the system more robust against fluctuations in the audio signal
    constexpr double peekTolerance = 30;    // most peaks are in range of [0:30]

    if ( rms > meanRMS( rms + peekTolerance ) )
    {
        evalAudioChunk.passedFilter = true;
    }

    audioBuffer.push_back( std::move( evalAudioChunk ) );
}


// ************************ //
// *** Helper functions *** //
// ************************ //

void SpeechListener::processAudioBuffer( size_t bufferSize )
{
    for ( int i = 0; i < bufferSize; ++i )
        meanPass( audioBuffer[i].passedFilter ? 1 : 0 );

    double meanPassThreshold;

    if ( listening )
        meanPassThreshold = 0.4;
    else
        meanPassThreshold = 0.5;

    if ( meanPass() >= meanPassThreshold )
    {
        qDebug() << "listening";
        listening = true;
        pushToSpeechBuffer( bufferSize );
    }
    else if ( listening )
    {
        qDebug() << "finished";
        listening = false;
        pushToSpeechBuffer( bufferSize );

        // up until now everything has been recorded and can be fed to DeepSpeech
        speechModelManager.feedSpeechData( std::move( speechBuffer ) );
    }
    else
    {
        qDebug() << "ignored";

        // delete every EvalAudioChunk object that didn't pass the filter
        do
        {
            audioBuffer.pop_front();

        } while ( !audioBuffer.front().passedFilter );
    }

    meanPass.reset();
    qDebug() << meanRMS();
}

void SpeechListener::filterSpeech( short* audioData, size_t dataSize )
{
    // The usage of Aquila has been taken from examples on their website

    // convert short array "audioData" to double array to perform fft
    double convertedAudioData[dataSize];
    Utils::convertToDoubleArray( audioData, dataSize, convertedAudioData );

    Aquila::SignalSource source( convertedAudioData, dataSize, SAMPLE_RATE );

    // calculate FFT
    auto                 fft      = Aquila::FftFactory::getFft( dataSize );
    Aquila::SpectrumType spectrum = fft->fft( source.toArray() );
    Aquila::SpectrumType filterSpectrum( dataSize );

    // create low-pass and high-pass filter
    for ( int i = 0; i < dataSize; ++i )
    {
        if ( i < ( dataSize * VOICE_LOW_PASS / SAMPLE_RATE ) && i > ( dataSize * VOICE_HIGH_PASS / SAMPLE_RATE ) )
            // pass band
            filterSpectrum[i] = 1;
        else
            // stop band
            filterSpectrum[i] = 0;
    }

    // filter spectrum
    std::transform( std::begin( spectrum ), std::end( spectrum ), std::begin( filterSpectrum ), std::begin( spectrum ),
                    []( Aquila::ComplexType x, Aquila::ComplexType y ) { return x * y; } );

    // convert filtered spectrum back to time domain
    double filteredAudioData[dataSize];
    fft->ifft( spectrum, filteredAudioData );

    // convert back to short array
    Utils::convertFromDoubleArray( filteredAudioData, dataSize, audioData );
}

void SpeechListener::pushToSpeechBuffer( size_t numberOfProcessedElements )
{
    auto speechBufferBackIt = std::back_inserter( speechBuffer );

    for ( int audioBufferIndex = 0; audioBufferIndex < numberOfProcessedElements; ++audioBufferIndex )
    {
        EvalAudioChunk& audioChunk = audioBuffer.front();

        std::copy( audioChunk.audioData, audioChunk.audioData + audioChunk.dataSize, speechBufferBackIt );

        // remove element
        audioBuffer.pop_front();
    }
}

void SpeechListener::playAudio( const EvalAudioChunk& audioChunk )
{
    QByteArray bytes;
    Utils::splitToBytes<2>( audioChunk.audioData, audioChunk.dataSize, bytes );
    outputDevice->write( bytes );
}

void SpeechListener::playAudio( std::vector<short> audioBuffer )
{
    QByteArray bytes;
    Utils::splitToBytes<2>( &audioBuffer[0], audioBuffer.size(), bytes );
    std::cout << audioBuffer.size() << " " << bytes.size() << std::endl;
    outputDevice->write( bytes );
}
