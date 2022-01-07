//
// Created by Denis Wagner on 6/22/19.
//

#include "include/SpeechManager/SpeechListenerConsoleMock.hpp"
#include <include/SpeechManager/SpeechManager.hpp>
#include <iostream>
#include <QtCore/QThread>
#include <QtCore/QtCore>

SpeechListenerConsoleMock::SpeechListenerConsoleMock( SpeechManager* speechManager,
                                                      Utils::BlockingQueue<std::string>& inputTextBuffer )
        : speechManager( speechManager ), inputTextBuffer( inputTextBuffer )
{
    start();
}

void SpeechListenerConsoleMock::run()
{
    std::string utterance;

    do
    {
        std::getline( std::cin, utterance );

        if ( speechManager->isSpeaking() )
            continue;

        inputTextBuffer.push_back( utterance );

    } while ( utterance != "quit()" );

    std::cout << "Bye bye" << std::endl;
    qApp->exit( 0 );
}
