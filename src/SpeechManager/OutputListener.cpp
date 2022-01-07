//
// Created by Denis Wagner on 4/22/19.
//

#include "include/SpeechManager/OutputListener.hpp"
#include <include/SpeechManager/SpeechManager.hpp>
#include <QDebug>
#include <speak_lib.h>

OutputListener::OutputListener( SpeechManager* speechManager, Utils::BlockingQueue<std::string>& outputTextBuffer )
        : speechManager( speechManager ), outputTextBuffer( outputTextBuffer )
{
    espeak_Initialize( AUDIO_OUTPUT_PLAYBACK, 200, nullptr, 0 );
    espeak_SetVoiceByName( mbrolaVoice.c_str() );

    // start thread
    start();
}

void OutputListener::run()
{
    while ( !isInterruptionRequested() )
    {
        say( outputTextBuffer.front() );
        outputTextBuffer.pop_front();
    }
}

void OutputListener::say( const std::string& text ) const
{
    speechManager->setSpeaking( true );

    espeak_Synth( text.c_str(), text.length() + 1, 0, POS_CHARACTER, 0, espeakCHARS_AUTO | espeakENDPAUSE, nullptr,
                  nullptr );
    espeak_Synchronize();

    speechManager->setSpeaking( false );
}

