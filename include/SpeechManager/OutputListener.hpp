//
// Created by Denis Wagner on 4/22/19.
//

#ifndef PERSONAL_ASSISTANT_OUTPUTLISTENER_HPP
#define PERSONAL_ASSISTANT_OUTPUTLISTENER_HPP

#include <QtCore/QThread>
#include <include/Utils/BlockingQueue.hpp>
#include <include/Utils/Config.hpp>

class SpeechManager;

class OutputListener : public QThread
{
    SpeechManager* speechManager;

    Utils::BlockingQueue<std::string>& outputTextBuffer;
    std::string mbrolaVoice = Config::getMbrolaVoice();

public:
    OutputListener( SpeechManager* speechManager, Utils::BlockingQueue<std::string>& outputTextBuffer );

private:
    virtual void run() override;
    void say( const std::string& text ) const;
};

#endif //PERSONAL_ASSISTANT_OUTPUTLISTENER_HPP
