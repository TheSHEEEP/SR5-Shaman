#ifndef RULES_H
#define RULES_H

/**
 * This header must be included for any knowledge about the SR rules.
 **/

#include "metatyperules.h"
#include "attributerules.h"
#include "magicrules.h"
#include "skillrules.h"
#include "qualityrules.h"
#include "resourcerules.h"

// Helpful defines
#define METATYPE_RULES Rules::getSingletonPtr()->getMetatypeRules()
#define ATTRIBUTE_RULES Rules::getSingletonPtr()->getAttributeRules()
#define MAGIC_RULES Rules::getSingletonPtr()->getMagicRules()
#define QUALITY_RULES Rules::getSingletonPtr()->getQualityRules()
#define SKILL_RULES Rules::getSingletonPtr()->getSkillRules()
#define RESOURCE_RULES Rules::getSingletonPtr()->getResourceRules()

/**
 * @brief The priorities of the character generation.
 */
enum Priority
{
    PRIORITY_INVALID = -1,
    PRIORITY_METATYPE,
    PRIORITY_ATTRIBUTES,
    PRIORITY_MAGIC,
    PRIORITY_SKILLS,
    PRIORITY_RESOURCES,
    PRIORITY_NUM_PRIORITIES
};

/**
 * @brief This singleton class holds all the rules for the SR5 Shaman.
 * @note    Remember to initialize it on startup, to read all the JSON files that define the rules.
 */
class Rules
{
private:
    static Rules* _instance;

    /**
     * @brief Constructor.
     */
    Rules();

    /**
     * @brief Destructor.
     */
    ~Rules();

public:
    /**
     * @brief Returns a pointer to the CharacterData.
     */
    static Rules* getSingletonPtr()
    {
        if (_instance == 0)
        {
            _instance = new Rules();
        }
        return _instance;
    }

    /**
     * @brief Initializes all rules, reading and applying the JSON files within the ruleset folder.
     */
    void initialize();

    /**
     * @brief Returns a pointer to the metatype rules.
     */
    MetatypeRules* getMetatypeRules() const;

    /**
     * @brief Returns a pointer to the attribute rules.
     */
    AttributeRules* getAttributeRules() const;

    /**
     * @brief Returns a pointer to the magic rules.
     */
    MagicRules* getMagicRules() const;

    /**
     * @brief Returns a pointer to the skill rules.
     */
    SkillRules* getSkillRules() const;

    /**
     * @brief Returns a pointer to the quality rules.
     */
    QualityRules* getQualityRules() const;

    /**
     * @brief Returns a pointer to the resource rules.
     */
    ResourceRules* getResourceRules() const;

private:
    bool    _initialized;

    MetatypeRules*      _metatypeRules;
    AttributeRules*     _attributeRules;
    MagicRules*         _magicRules;
    SkillRules*         _skillRules;
    QualityRules*       _qualityRules;
    ResourceRules*      _resourceRules;
};

//---------------------------------------------------------------------------------
inline
MetatypeRules*
Rules::getMetatypeRules() const
{
    return _metatypeRules;
}

//---------------------------------------------------------------------------------
inline
AttributeRules*
Rules::getAttributeRules() const
{
    return _attributeRules;
}

//---------------------------------------------------------------------------------
inline
MagicRules*
Rules::getMagicRules() const
{
    return _magicRules;
}

//---------------------------------------------------------------------------------
inline
SkillRules*
Rules::getSkillRules() const
{
    return _skillRules;
}

//---------------------------------------------------------------------------------
inline
QualityRules*
Rules::getQualityRules() const
{
    return _qualityRules;
}

//---------------------------------------------------------------------------------
inline
ResourceRules*
Rules::getResourceRules() const
{
    return _resourceRules;
}

#endif // RULES_H
