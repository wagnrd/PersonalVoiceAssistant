//
// Created by Denis Wagner on 5/8/19.
//

#ifndef PERSONAL_ASSISTANT_SKILL_HPP
#define PERSONAL_ASSISTANT_SKILL_HPP

#include <map>
#include <string>
#include <QObject>
#include "SkillAPI.hpp"

struct Skill
{
    /**
     * This is your API for communication with the rest of the program except from the UI.
     */
    SkillAPI* skillManager;

    /**
     * This function will be called AFTER skillManager has been set.
     * So you may want use this function for initializing the skill instead of the constructor
     * to avoid segmentation faults.
     */
    virtual void construct() = 0;

    /**
     * This function will be called every time a new utterance comes in and no skill is currently active.
     * So you can update the supported utterances and their intents according to the current state of the
     * skill if needed. The utterances can contain "entities" which are marked as "@some_entity" which will
     * act like wildcards for an arbitrary string. The intent acts like an identifier to know which type of
     * utterance this is. The intent should differ for every set of different entities used in an utterance.
     *
     * Example:
     *      1. Utterance: "How it the weather in @city?" - Intent: weather_city_intent
     *      2. Utterance: "Whats the weather in @city?" - Intent: weather_city_intent
     *      3. Utterance: "How is the weather @day? - Intent : weather_city_day_intent    // this one uses a different
     *                                                                                        set of entities
     *
     * The entities don't have to be in the same order for the same intent if there are more than one in an utterance.
     * When you receive them in the "process" function the map will contain the intent-names and their corresponding
     * value. So you don't have to mind the ordering.
     *
     * This is basically some sort of application for the position of the next active skill from which the
     * SkillManager will choose.
     *
     * @return The current map of supported utterances (key) and intents (value).
     */
    virtual const std::map<std::string, std::string>& getSupportedUtterances() = 0;

    /**
     * This function will be called if this skill is the currently active one and a new utterance comes in
     * for this particular skill.
     *
     * The values for "intent" and "entities" has been extracted out of the utterance in conjunction with
     * the map returned from "getSupportedUtterances()".
     *
     * @param utterance Contains the new utterance which belongs to this skill
     * @param intent    The intent of the the utterance
     * @param entities  A map of entities and their corresponding values found in the utterance
     */
    virtual void process( const std::string& utterance, const std::string& intent,
                          const std::map<std::string, std::string>& entities ) = 0;

    /**
     * Convenience function for skillManager->sendResponse(response, this, ...)
     *
     * @param response  The response to be spoken
     * @param active    Whether the skill should be deactivated or not after the response has been sent
     */
    void sendResponse( const std::string& response, bool active = true )
    {
        skillManager->sendResponse( response, this, active );
    }

    virtual ~Skill() = default;
};

#define Skill_ID "skill/0.2"

Q_DECLARE_INTERFACE( Skill, Skill_ID )

#endif //PERSONAL_ASSISTANT_SKILL_HPP
