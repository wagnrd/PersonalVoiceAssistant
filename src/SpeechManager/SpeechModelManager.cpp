//
// Created by Denis Wagner on 4/25/19.
//

#include "include/SpeechManager/SpeechModelManager.hpp"
#include <deepspeech/native_client/deepspeech.h>

SpeechModelManager::SpeechModelManager( Utils::BlockingQueue<std::string>& inferredTextBuffer )
        : inferredTextBuffer( inferredTextBuffer )
{
    DS_CreateModel( modelPath, mfccFeatures, contextWindowSize, alphabetPath, beamWidth, &model );

    // uses real words -> unlikely to understand words it doesn't know
    DS_EnableDecoderWithLM( model, alphabetPath, lmBinPath, triePath, lmAlpha, lmBeta );

    // start thread
    start();
}

void SpeechModelManager::run()
{
    while ( !isInterruptionRequested() )
    {
        auto& speechData = speechDataBuffer.front();
        speechDataBuffer.pop_front();

        char* inferredText = DS_SpeechToText( model, &speechData[0], speechData.size(), SAMPLE_RATE );
        inferredTextBuffer.push_back( inferredText );

        // result of DS_SpeechToText has to be manually freed
        delete[] inferredText;
    }
}

void SpeechModelManager::feedSpeechData( std::vector<short> speechData )
{
    speechDataBuffer.push_back( std::move( speechData ) );
}

