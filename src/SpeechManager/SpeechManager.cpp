//
// Created by Denis Wagner on 4/11/19.
//

#include "include/SpeechManager/SpeechManager.hpp"

bool SpeechManager::submitOutputText( const std::string& text )
{
    constexpr size_t outputTextBufferLimit = 5;

    if ( outputTextBuffer.size() >= outputTextBufferLimit )
        return false;

    outputTextBuffer.push_back( text );

    return true;
}

/**
 * Receives the next unprocessed speechToText entry from the Queue.
 * Blocks if there is no string inside the Queue (may use a separate thread for this function)
 *
 * @return A string containing the next unprocessed text, that has been transformed from speech to text.
 */
std::string SpeechManager::reseiveInputText()
{
    std::string inputText = inputTextBuffer.front();
    inputTextBuffer.pop_front();

    return std::move( inputText );
}

void SpeechManager::setSpeaking( bool isSpeaking )
{
    speaking = isSpeaking;
}

bool SpeechManager::isSpeaking()
{
    return speaking;
}

