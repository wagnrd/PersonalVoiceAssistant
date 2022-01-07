//
// Created by Denis Wagner on 5/20/19.
//

#ifndef PERSONAL_ASSISTANT_SPEECHLISTENERCONSOLEMOCK_HPP
#define PERSONAL_ASSISTANT_SPEECHLISTENERCONSOLEMOCK_HPP

#include <include/Utils/BlockingQueue.hpp>
#include <iostream>
#include <QtCore/QThread>
#include <QtCore/QtCore>

class SpeechManager;

class SpeechListenerConsoleMock : public QThread
{
    SpeechManager* speechManager;

    Utils::BlockingQueue<std::string>& inputTextBuffer;

public:
    SpeechListenerConsoleMock( SpeechManager* speechManager, Utils::BlockingQueue<std::string>& inputTextBuffer );

private:
    virtual void run() override;
};

#endif //PERSONAL_ASSISTANT_SPEECHLISTENERCONSOLEMOCK_HPP
