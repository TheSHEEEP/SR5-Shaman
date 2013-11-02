#ifndef RULES_H
#define RULES_H

/**
 * This header must be included for any knowledge about the SR rules.
 **/

#include "metatyperules.h"


/**
 * @brief The priorities of the character generation.
 */
enum Priorities
{
    PRIORITY_INVALID = -1,
    PRIORITY_METATYPE,
    PRIORITY_ATTRIBUTES,
    PRIORITY_MAGIC,
    PRIORITY_SKILLS,
    PRIORITY_RESOURCES,
    PRIORITY_NUM_PRIORITIES
};

// Helpful defines
#define METATYPE_RULES_PTR Rules::getSingletonPtr()->getMetatypeRules()

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
    const MetatypeRules* getMetatypeRules() const;

private:
    bool    _initialized;

    MetatypeRules*  _metatypeRules;
};

//---------------------------------------------------------------------------------
inline
const MetatypeRules*
Rules::getMetatypeRules() const
{
    return _metatypeRules;
}

#endif // RULES_H
