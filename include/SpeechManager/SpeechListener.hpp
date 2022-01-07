//
// Created by Denis Wagner on 4/20/19.
//

#ifndef PERSONAL_ASSISTANT_SPEECHLISTENER_HPP
#define PERSONAL_ASSISTANT_SPEECHLISTENER_HPP

#include "EvalAudioChunk.hpp"
#include "SpeechModelManager.hpp"
#include <include/Utils/Mean.hpp>
#include <include/Utils/BlockingQueue.hpp>

#include <QtCore/QThread>
#include <QtCore/QTimer>
#include <QtMultimedia/QAudioInput>
#include <QtMultimedia/QAudioOutput>
#include <include/Utils/Config.hpp>

class QIODevice;

class SpeechManager;

class SpeechListener : public QThread
{
    using clock = std::chrono::system_clock;
    using milliseconds = std::chrono::milliseconds;

    const size_t SAMPLE_RATE         = Config::getSampleRate();
    const size_t VOICE_LOW_PASS      = 8000; // upper frequency bound
    const size_t VOICE_HIGH_PASS     = 100;  // lower frequency bound

    SpeechManager* speechManager;
    QIODevice    * inputDevice;
    QIODevice    * outputDevice;
    QAudioInput  * audioInput;
    QAudioOutput * audioOutput;

    Utils::BlockingQueue<EvalAudioChunk> audioBuffer;
    Utils::BlockingQueue<std::string>& inputTextBuffer;

    Utils::Mean        meanRMS{ 0, true }; // starts with 0, QAudioInput tends to receive loud sounds in first chunk
    Utils::Mean        meanPass;
    bool               listening     = false;
    std::vector<short> speechBuffer;
    SpeechModelManager speechModelManager{ inputTextBuffer };
    clock::time_point  lastTimeStamp = clock::now();

Q_OBJECT

private slots:
    void readAudioInput();

public:
    SpeechListener( SpeechManager* speechManager, Utils::BlockingQueue<std::string>& inputTextBuffer );

private:
    virtual void run() override;
    void processAudioBuffer( size_t bufferSize );
    void pushToSpeechBuffer( size_t numberOfProcessedElements );
    void filterSpeech( short* audioData, size_t dataSize );
    void playAudio( const EvalAudioChunk& audioChunk );
    void playAudio( std::vector<short> audioBuffer );
};

#endif //PERSONAL_ASSISTANT_SPEECHLISTENER_HPP
