//
// Created by Denis Wagner on 4/11/19.
//

#ifndef PERSONAL_ASSISTANT_CONVERSATIONMANAGER_HPP
#define PERSONAL_ASSISTANT_CONVERSATIONMANAGER_HPP

#include <include/SpeechManager/SpeechManager.hpp>
#include <include/SkillManager/SkillManager.hpp>
#include <include/Utils/Config.hpp>
#include "InputListener.hpp"

class ConversationManager
{
    std::string prefix = Config::getPrefix();

    SpeechManager speechManager;
    SkillManager  skillManager{ this };
    InputListener inputListener{ &speechManager, &skillManager, prefix };

public:
    void sendResponse( const std::string& response );
    void activateConversation( bool activate = true );
};

#endif //PERSONAL_ASSISTANT_CONVERSATIONMANAGER_HPP
