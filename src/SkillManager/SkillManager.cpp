//
// Created by Denis Wagner on 5/8/19.
//

#include <QDebug>
#include <QtCore/QDir>
#include <QtCore/QPluginLoader>
#include <QtCore/QCoreApplication>
#include <regex>

#include <include/SkillManager/Skill.hpp>
#include <include/SkillManager/Skills/Echo/Echo.hpp>
#include <include/SkillManager/Skills/EchoEn/EchoEn.hpp>
#include <include/SkillManager/Skills/Remember/Remember.hpp>
#include <include/SkillManager/Skills/RememberEn/RememberEn.hpp>
#include <include/ConversationManager/ConversationManager.hpp>
#include "include/SkillManager/SkillManager.hpp"

SkillManager::SkillManager( ConversationManager* conversationManager )
        : conversationManager( conversationManager )
{
    //loadSkills();

    // load embedded skills
    //skills.push_back( new Echo( this ) );
    //skills.push_back( new Remember( this ) );
    skills.push_back( new EchoEn( this ) );
    skills.push_back( new RememberEn( this ) );
}

SkillManager::~SkillManager()
{
    for ( auto skill : skills )
        delete skill;
}

void SkillManager::loadSkills()
{
    // most of the following source code has been taken from the official Qt tutorial for QPluginLoader
    QDir skillDir( qApp->applicationDirPath() );

    // assumes that the binary runs in some kind of "bin" folder and "skills" is on the same level as "bin"
    skillDir.cdUp();
    skillDir.cd( "skills" );

    for ( const auto& fileName : skillDir.entryList( QDir::Files ) )
    {
        QPluginLoader pluginLoader( skillDir.absoluteFilePath( fileName ) );
        QObject* pluginObj = pluginLoader.instance();

        if ( pluginObj )
        {
            auto skill = qobject_cast<Skill*>( pluginObj );

            if ( skill )
            {
                skill->skillManager = this;
                skill->construct();
                skills.push_back( skill );

                continue;
            }
        }

        qDebug() << "Skill" << fileName << "couldn't be loaded!";
    }
}

bool SkillManager::submitUtterance( const std::string& utterance )
{
    constexpr double matchRatingThreshold = 0;

    if ( !activeSkills.empty() )
    {
        auto[skillMatchRating, intent, entities] = getSkillMatchRating( activeSkills.top(), utterance );

        if ( skillMatchRating >= matchRatingThreshold )
        {
            activeSkills.top()->process( utterance, intent, entities );

            return true;
        }
    }
    else
    {
        double                             highestSkillMatchRating = 0;
        std::map<std::string, std::string> highestMatchingEntities;
        std::string                        highestMatchingIntent;
        Skill* highestMatchingSkill = nullptr;

        for ( auto skill : skills )
        {
            auto[skillMatchRating, intent, entities] = getSkillMatchRating( skill, utterance );

            if ( skillMatchRating >= matchRatingThreshold && skillMatchRating > highestSkillMatchRating )
            {
                highestSkillMatchRating = skillMatchRating;
                highestMatchingSkill    = skill;
                highestMatchingEntities = std::move( entities );
                highestMatchingIntent   = std::move( intent );
            }
        }

        if ( highestMatchingSkill )
        {
            activeSkills.push( highestMatchingSkill );
            highestMatchingSkill->process( utterance, highestMatchingIntent, highestMatchingEntities );

            return true;
        }
    }

    // if no active skill or no matching skills exist
    return false;
}

void SkillManager::sendResponse( const std::string& response, Skill* skill, bool activate )
{
    conversationManager->sendResponse( response );

    if ( !activate )
        deactivateSkill( skill );
}

void SkillManager::deactivateSkill( Skill* skill )
{
    activeSkills.erase( skill );

    if ( activeSkills.empty() )
        conversationManager->activateConversation( false );
}

void SkillManager::emergencySkillDeactivation()
{
    // TODO: maybe signal the skill that it has been deactivated
    if ( activeSkills.empty() )
    {
        conversationManager->activateConversation( false );
    }
    else
        deactivateSkill( activeSkills.top() );
}

/**
 * Compares two characters while ignoring their case.
 *
 * @param c1 First character
 * @param c2 second character
 * @return the boolean result if the two characters are equal (ignoring the case)
 */
bool caseInsCharCompare( char c1, char c2 )
{
    return tolower( c1 ) == tolower( c2 );
}

/**
 * Compares two strings while ignoring their case.
 *
 * @param s1 first string
 * @param s2 second string
 * @return the boolean result if the two strings are equal (ignoring the case)
 */
bool caseInsCompare( const std::string& s1, const std::string& s2 )
{
    return ( ( s1.size() == s2.size() ) && std::equal( s1.begin(), s1.end(), s2.begin(), s2.end(),
                                                       caseInsCharCompare ) );
}

std::tuple<double, std::string, std::map<std::string, std::string>>
SkillManager::getSkillMatchRating( Skill* skill, const std::string& utterance )
{
    std::regex           findWords( "\\S+" );
    std::regex           findEntities( "@\\w+" );
    std::sregex_iterator wordIteratorUtt( utterance.begin(), utterance.end(), findWords );
    std::sregex_iterator endIterator;

    double                             highestMatchRating = -1;
    std::string                        highestMatchintent;
    std::map<std::string, std::string> highestMatchEntities;

    auto& supportedUtterances = skill->getSupportedUtterances();

    for ( auto& supportedUtterance : supportedUtterances )
    {
        std::cout << "Utterance: #" << utterance << "# - supported utterance: #" << supportedUtterance.first << "#"
                  << std::endl;

        std::sregex_iterator wordIteratorSup( supportedUtterance.first.begin(), supportedUtterance.first.end(),
                                              findWords );

        auto   itUtt           = wordIteratorUtt;
        auto   itSup           = wordIteratorSup;
        auto   lastItSup       = itSup;
        int    entitiesMatched = 0;
        int    wordCount       = 0;
        double score           = 0;

        std::map<std::string, std::string> entities;

        while ( true )
        {
            bool match       = false;
            bool entityMatch = false;
            int  skipped     = 0;

            while ( itSup != endIterator )
            {
                if ( std::regex_match( itSup->str(), findEntities ) )
                {
                    entityMatch = true;
                    ++entitiesMatched;
                    lastItSup = itSup;

                    // adding entityMatch to entity map
                    if ( entities.find( itSup->str() ) == entities.end() )
                        entities[itSup->str()] = itUtt->str(); // first insert
                    else
                        entities[itSup->str()] += " " + itUtt->str();

                    if ( entitiesMatched == 1 )
                        score += 0.75;
                    else
                        score += 1 / static_cast<double>(entitiesMatched);

                    break;
                }
                else if ( caseInsCompare( itSup->str(), itUtt->str() ) )
                {
                    match     = true;
                    score += 1 - skipped;
                    lastItSup = ++itSup;

                    break;
                }
                else
                {
                    ++skipped;
                    ++itSup;
                }
            }

            if ( !match && skipped > 0 )
            {
                score -= 1;
                itSup = lastItSup;
            }
            else if ( entityMatch )
            {
                auto lookAheadItSup = itSup;
                auto lookAheadItUtt = itUtt;
                ++lookAheadItSup;
                ++lookAheadItUtt;

                if ( lookAheadItSup->str() == lookAheadItUtt->str() ||
                     std::regex_match( lookAheadItSup->str(), findEntities ) )
                {
                    lastItSup       = ++itSup;
                    entitiesMatched = 0;
                }
            }

            ++itUtt;
            ++wordCount;

            if ( itUtt == endIterator )
                break;
        }

        while ( itSup != endIterator )
        {
            score -= 1;
            ++itSup;
        }

        double matchRating = score / static_cast<double>(wordCount);

        std::cout << "Match rate: " << matchRating << " (" << score << "/" << wordCount << ")" <<
                  std::endl;

        if ( matchRating > highestMatchRating )
        {
            highestMatchRating   = matchRating;
            highestMatchintent   = supportedUtterance.second; // cannot move, since the map is a reference
            highestMatchEntities = std::move( entities );
        }
    }

    return { highestMatchRating, highestMatchintent, highestMatchEntities };
}
