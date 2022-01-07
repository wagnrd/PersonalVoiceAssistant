//
// Created by Denis Wagner on 4/11/19.
//

#ifndef PERSONAL_ASSISTANT_SPEECHMANAGER_HPP
#define PERSONAL_ASSISTANT_SPEECHMANAGER_HPP

#include "SpeechListener.hpp"
#include "OutputListener.hpp"
#include "SpeechModelManager.hpp"
#include "SpeechListenerConsoleMock.hpp"

class QIODevice;

class SpeechManager
{
    Utils::BlockingQueue<std::string> inputTextBuffer;
    Utils::BlockingQueue<std::string> outputTextBuffer;

    SpeechListener speechListener{ this, inputTextBuffer };
    //SpeechListenerConsoleMock speechListener{ this, inputTextBuffer };
    OutputListener outputListener{ this, outputTextBuffer };

    bool speaking = false;

    friend SpeechListener;
    friend SpeechListenerConsoleMock;
    friend OutputListener;

public:
    bool submitOutputText( const std::string& text );
    std::string reseiveInputText();

private:
    void setSpeaking( bool isSpeaking );
    bool isSpeaking();
};

#endif //PERSONAL_ASSISTANT_SPEECHMANAGER_HPP
