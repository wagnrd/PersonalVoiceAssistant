//
// Created by Denis Wagner on 5/7/19.
//

#include <include/SpeechManager/SpeechManager.hpp>
#include <include/SkillManager/SkillManager.hpp>
#include "include/ConversationManager/InputListener.hpp"

InputListener::InputListener( SpeechManager* speechManager, SkillManager* skillManager, std::string& prefix )
        : speechManager( speechManager ), skillManager( skillManager ), prefix( prefix )
{
    // lower the timer precision to reduce cpu usage in main thread
    lastInputTimer.setTimerType( Qt::TimerType::CoarseTimer );
    lastInputTimer.setInterval( 4000 );
    lastInputTimer.setSingleShot( true );
    connect( &lastInputTimer, SIGNAL( timeout() ), this, SLOT( lastInputTimeout() ) );
    connect( this, SIGNAL( startTimer() ), &lastInputTimer, SLOT( start() ) );
    connect( this, SIGNAL( stopTimer() ), &lastInputTimer, SLOT( stop() ) );

    // start thread
    start();
}

void InputListener::run()
{
    while ( !isInterruptionRequested() )
    {
        std::string newInferrence = speechManager->reseiveInputText();

        if ( !newInferrence.empty() )
            currentUtterance += newInferrence;
        else
            continue;

        std::cout << "Utterance: " << currentUtterance << std::endl;

        if ( !conversationActive )
        {
            if ( containsPrefix( currentUtterance ) )
            {
                conversationActive = true;
                cropUtterance( currentUtterance );
            }
            else
            {
                currentUtterance.clear();

                continue;
            }
        }

        // will just be called if conversationActive == true
        if ( !currentUtterance.empty() )
        {
            bool matchingSkillFound = skillManager->submitUtterance( currentUtterance );

            if ( matchingSkillFound )
            {
                currentUtterance.clear();

                if ( lastInputTimer.isActive() )
                        emit stopTimer();
            }
            else
            {
                // starts/restarts the timer to wait for further inputs until it deletes currentUtterance
                emit startTimer();
            }
        }
    }
}

void InputListener::cropUtterance( std::string& utterance )
{
    // get the position behind the prefix, so after the cropping only the plain utterance without prefix is left
    auto cropPos = utterance.find( prefix ) + prefix.size() + 1;
    cropPos = cropPos < 0 ? 0 : cropPos;
    utterance.erase( 0, cropPos );
}

bool InputListener::containsPrefix( const std::string& utterance ) const
{
    return utterance.find( prefix ) != std::string::npos;
}

void InputListener::lastInputTimeout()
{
    currentUtterance.clear();

    constexpr int triesUntilSkillDeactivation = 1;

    if ( ++tries > triesUntilSkillDeactivation )
    {
        skillManager->emergencySkillDeactivation();
        tries = 0;
    }

    speechManager->submitOutputText( noSkillReply );
}

void InputListener::setConversationActivity( bool active )
{
    conversationActive = active;
    currentUtterance.clear();
}

