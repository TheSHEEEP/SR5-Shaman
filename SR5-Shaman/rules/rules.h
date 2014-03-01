#ifndef RULES_H
#define RULES_H

/**
 * This header must be included for any knowledge about the SR rules.
 **/

#include "metatyperules.h"
#include "attributerules.h"
#include "magicrules.h"
#include "skillrules.h"
#include "effects/effectregistry.h"

// Helpful defines
#define METATYPE_RULES Rules::getSingletonPtr()->getMetatypeRules()
#define ATTRIBUTE_RULES Rules::getSingletonPtr()->getAttributeRules()
#define MAGIC_RULES Rules::getSingletonPtr()->getMagicRules()
#define SKILL_RULES Rules::getSingletonPtr()->getSkillRules()
#define EFFECT_REGISTRY Rules::getSingletonPtr()->getEffectRegistry()

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
     * @brief Returns a pointer to the skill rules.
     */
    EffectRegistry* getEffectRegistry() const;

private:
    bool    _initialized;

    MetatypeRules*      _metatypeRules;
    AttributeRules*     _attributeRules;
    MagicRules*         _magicRules;
    SkillRules*         _skillRules;
    EffectRegistry*     _skillRules;
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

#endif // RULES_H
