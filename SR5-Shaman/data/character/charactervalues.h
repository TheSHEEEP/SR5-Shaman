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
     * @param p_withAugmentations   If this is true, the augmentations will be considered.
     * @param p_withOther           If this is true, other attribute sources will also be considered
     *                              (like Adept Improved Attribute).
     */
    int getAttribute(const QString& p_attribute, bool p_withAugmentations = true, bool p_withOther = true) const;

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
     */
    int getPhysicalLimit() const;
    int getMentalLimit() const;
    int getSocialLimit() const;

    //*********************************************************************
    // SPELLS
    //*********************************************************************
    /**
     * @param p_id  The unique ID of the adept power to look for.
     * @return The level of the passed power, or 0 if the character does not have it.
     */
    int getAdeptPowerLevel(const QString& p_id) const;

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
