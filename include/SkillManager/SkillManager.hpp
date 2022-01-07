//
// Created by Denis Wagner on 5/8/19.
//

#ifndef PERSONAL_ASSISTANT_SKILLMANAGER_HPP
#define PERSONAL_ASSISTANT_SKILLMANAGER_HPP

#include <include/Utils/ExclusiveStack.hpp>
#include "SkillAPI.hpp"

class Skill;

class ConversationManager;

class SkillManager : public SkillAPI
{
    ConversationManager* conversationManager;

    std::vector<Skill*>    skills;
    ExclusiveStack<Skill*> activeSkills;

public:
    explicit SkillManager( ConversationManager* conversationManager );
    virtual ~SkillManager() override;
    bool submitUtterance( const std::string& utterance );
    void emergencySkillDeactivation();
    virtual void sendResponse( const std::string& response, Skill* skill, bool activate = true ) override;
    virtual void deactivateSkill( Skill* skill ) override;

private:
    void loadSkills();
    /**
     * @param skill     The skill that should be rated
     * @param utterance The current utterance against which the skill should be tested
     * @return (1) rating between 0 and 1, (2) intent, (3) entities map
     */
    std::tuple<double, std::string, std::map<std::string, std::string>>
    getSkillMatchRating( Skill* skill, const std::string& utterance );
};

#endif //PERSONAL_ASSISTANT_SKILLMANAGER_HPP
