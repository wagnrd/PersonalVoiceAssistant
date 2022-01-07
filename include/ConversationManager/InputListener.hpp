//
// Created by Denis Wagner on 5/7/19.
//

#ifndef PERSONAL_ASSISTANT_INPUTLISTENER_HPP
#define PERSONAL_ASSISTANT_INPUTLISTENER_HPP

#include <QtCore/QThread>
#include <QtCore/QTimer>

class SkillManager;

class SpeechManager;

class InputListener : public QThread
{
    SpeechManager* speechManager;
    SkillManager * skillManager;

    std::string& prefix;

    std::string noSkillReply       = Config::getNoSkillReply();
    bool        conversationActive = false;
    std::string currentUtterance;
    QTimer      lastInputTimer;
    int         tries              = 0;

Q_OBJECT

private slots:
    void lastInputTimeout();

signals:
    void startTimer();
    void stopTimer();

public:
    InputListener( SpeechManager* speechManager, SkillManager* skillManager, std::string& prefix );
    void setConversationActivity( bool active );

private:
    virtual void run() override;
    void cropUtterance( std::string& utterance );
    bool containsPrefix( const std::string& utterance ) const;
};

#endif //PERSONAL_ASSISTANT_INPUTLISTENER_HPP
