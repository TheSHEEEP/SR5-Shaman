#ifndef CHARACTERVALUES_H
#define CHARACTERVALUES_H

#include <QSet>
#include <QString>

// Helpful defines
#define CHARACTER_VALUES CharacterValues::getSingletonPtr()

/**
 * @brief   This singleton class should be used to get all character values that have some
 *          kind of calculation attached (attributes, skills, etc.).
 *          It does not store any information, but instead uses the CharacterChoices and Rules classes
 *          to calculate values on demand.
 * @author  TheSHEEEP
 */
class CharacterValues
{
private:
    static CharacterValues* _instance;

    /**
     * @brief Constructor.
     */
    CharacterValues();

public:
    /**
     * @brief Returns a pointer to the CharacterChoices.
     */
    static CharacterValues* getSingletonPtr()
    {
        if (_instance == 0)
        {
            _instance = new CharacterValues();
        }
        return _instance;
    }

public:
    //*********************************************************************
    // KARMA
    //*********************************************************************
    /**
     * @brief sets the karma pool to the passed value.
     */
    void setKarmaPool(int p_value);

    /**
     * @brief Increases the karma pool by the passed value.
     */
    void increaseKarmaPool(int p_increase);

    /**
     * @brief returns the current karma pool.
     */
    int getKarmaPool() const;

    //*********************************************************************
    // ATTRIBUTES
    //*********************************************************************
    //-----------------------------------------------------------
    // DIRECT VALUES
    //-----------------------------------------------------------
    /**
     * @brief Returns a base attribute value.
     * @param p_attribute           The name of the attribute. Can be one of the following:
     *                              body, agility, reaction, strength, willpower, logic, intuition, charisma, edge
     *                              and magic (use magic if you need resonance).
     * @param p_withAugmentations   If this is true, the augmentations (cyberware only) will be considered.
     * @param p_withOther           If this is true, other attribute sources will also be considered
     *                              (adept powers, qualities, etc.).
     */
    int getAttribute(const QString& p_attribute, bool p_withAugmentations = true, bool p_withOther = true) const;

    /**
     * @brief Returns the maximum NATURAL value of the passed attribute.
     *          This function si required as qualities may modify the value set in the metatype definition.
     * @param p_attribute   The attribute to look for.
     * @param p_effects     If effects increasing the attribute max should also be taken into account.
     */
    int getAttributeMax(const QString& p_attribute, bool p_effects = true);

    /**
     * @brief Returns the characters current essence value.
     * @param p_withAugments   If this is true, the augmentations will be considered.
     */
    float getEssence(bool p_withAugments = true) const;

    //-----------------------------------------------------------
    // DERIVED VALUES
    //-----------------------------------------------------------
    /**
     * @brief Returns the physical limit of the character.
     * @param p_modified    If this is true, modifications like effects will also be added to the value.
     */
    int getPhysicalLimit(bool p_modified = true) const;

    /**
     * @brief Returns the physical limit of the character.
     * @param p_modified    If this is true, modifications like effects will also be added to the value.
     */
    int getMentalLimit(bool p_modified = true) const;

    /**
     * @brief Returns the physical limit of the character.
     * @param p_modified    If this is true, modifications like effects will also be added to the value.
     */
    int getSocialLimit(bool p_modified = true) const;

    /**
     * @brief Returns the memory check pool of the character.
     * @param p_modified    If this is true, modifications like effects will also be added to the value.
     */
    int getMemory(bool p_modified = true) const;

    //*********************************************************************
    // SPELLS
    //*********************************************************************
    /**
     * @param p_id  The unique ID of the adept power to look for.
     * @return The level of the passed power, or 0 if the character does not have it.
     */
    int getAdeptPowerLevel(const QString& p_id) const;

    //*********************************************************************
    // SKILLS
    //*********************************************************************
    /**
     * @brief Returns a skill value.
     * @param p_skill         The ID of the skill.
     * @param p_withEffects   If this is true, effects (from cyberware, qualities, etc.) will be minded.
     */
    int getSkill(const QString& p_skill, bool p_withEffects = true) const;

    /**
     * @brief Returns the maximum value a skill can have.
     *          The value differs for creating a character and updating a character.
     * @param p_skill   The ID of the skill.
     */
    int getSkillMax(const QString& p_skill) const;

    /**
     * @brief Returns a list with all skill specializations for the passed skill.
     * @param p_skill   The ID of the skill.
     */
    QStringList getSkillSpecializations(const QString& p_skill) const;

    /**
     * @brief Returns the number of maximum knowledge/language points for this character.
     */
    int getMaxKnowledgePoints() const;

    //*********************************************************************
    // INITIATIVE
    //*********************************************************************
    /**
     * @brief Returns the number of initiative dice this character has.
     * @param p_modified    If modifications like effects should be considered.
     */
    int getInitiativeDice(bool p_modified = true) const;


    //*********************************************************************
    // QUALITIES
    //*********************************************************************
    /**
     * @brief Returns the level the character has in the passed quality.
     * @param p_quality The ID of the quality to look for.
     * @return  0 if the character does not have the quality.
     *          Otherwise, the level of the quality (1 for qualities that do not have levels).
     */
    int getQualityLevel(const QString& p_quality) const;

private:
    int             _karmaPool;
};

//---------------------------------------------------------------------------------
inline
void
CharacterValues::setKarmaPool(int p_value)
{
    _karmaPool = p_value;
}

//---------------------------------------------------------------------------------
inline
void
CharacterValues::increaseKarmaPool(int p_increase)
{
    _karmaPool += p_increase;
}

//---------------------------------------------------------------------------------
inline
int
CharacterValues::getKarmaPool() const
{
    return _karmaPool;
}

#endif // CHARACTERVALUES_H
