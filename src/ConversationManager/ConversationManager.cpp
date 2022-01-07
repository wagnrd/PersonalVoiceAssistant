//
// Created by Denis Wagner on 4/11/19.
//

#include "include/ConversationManager/ConversationManager.hpp"

void ConversationManager::sendResponse( const std::string& response )
{
    speechManager.submitOutputText( response );
}

void ConversationManager::activateConversation( bool activate )
{
    inputListener.setConversationActivity( activate );
}

