//
// Created by Denis Wagner on 5/19/19.
//

#ifndef PERSONAL_ASSISTANT_SKILLAPI_HPP
#define PERSONAL_ASSISTANT_SKILLAPI_HPP

#include <string>

class Skill;

class SkillAPI
{
public:
    virtual ~SkillAPI() = default;
    virtual void sendResponse( const std::string& response, Skill* skill, bool activate = true ) = 0;
    virtual void deactivateSkill( Skill* skill ) = 0;
};

#endif //PERSONAL_ASSISTANT_SKILLAPI_HPP
