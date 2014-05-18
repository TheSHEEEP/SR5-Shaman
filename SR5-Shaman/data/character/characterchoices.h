#ifndef CHARACTERDATA_H
#define CHARACTERDATA_H

#include "rules/rules.h"
#include <vector>
#include <QString>
#include <QObject>

// Helpful defines
#define CHARACTER_CHOICES CharacterChoices::getSingletonPtr()

/**
 * @brief This struct holds all information about house rules that are applied to this character.
 *        Must be stored in the character as this is an information on character level.
 */
struct HouseRules
{
    bool qualityLimitSumCounts = false;
};

/**
 * @brief This singleton class holds all player choices of the character creation.
 *          Does NOT hold any information about rules.
 * @author  TheSHEEEP
 */
class CharacterChoices : public QObject
{
    Q_OBJECT

private:
    static CharacterChoices* _instance;

    /**
     * @brief Constructor.
     */
    CharacterChoices();

public:
    /**
     * @brief Returns a pointer to the CharacterChoices.
     */
    static CharacterChoices* getSingletonPtr()
    {
        if (_instance == 0)
        {
            _instance = new CharacterChoices();
        }
        return _instance;
    }

    /**
     * @brief Sets the character's metytype ID.
     */
    void setMetatypeID(const QString& p_uniqueID);

    /**
     * @brief Returns the currently selected metatype ID.
     */
    const QString& getMetatypeID() const;

    /**
     * @brief Sets the priority at the passed index.
     *          This may unset other priorities.
     * @param p_priorityIndex   The index of the priority (0-4 is A-E)
     * @param p_prio            The priority type to set.
     */
    void setPriority(int p_priorityIndex, Priority p_prio);

    /**
     * @brief Returns the priority index of the passed priority. Or -1 if the priority does not yet have an index.
     */
    int getPriorityIndex(Priority p_prio) const;

    /**
     * @brief Returns the priority at the passed index. return PRIORITY_INVALID if none is there.
     * @param p_index   The index to look at.
     */
    Priority getPriorityAtIndex(int p_index) const;

    /**
     * @brief Will unset the priority of the chosen type, setting it to invalid.
     *          i.e.    If you pass PRIORITY_METATYPE and it is currently at priority D,
     *                  priority D will be set to invalid.
     */
    void unsetPriority(Priority p_prio);

    /**
     * @brief Sets the name of the character.
     */
    void setName(const QString& p_name);

    /**
     * @brief Returns the name of the character.
     */
    const QString& getName() const;

    /**
     * @brief Sets the nick of the character.
     */
    void setNick(const QString& p_nick);

    /**
     * @brief Returns the nick of the character.
     */
    const QString& getNick() const;

    //*********************************************************************
    // MAGIC
    //*********************************************************************
    /**
     * @brief Returns true if this character is a mgic user.
     */
    bool getIsMagicUser() const;

    /**
     * @brief Sets if the character is a magic user or not.
     *          This is a convenience function that also sets priorities accordingly.
     */
    void setIsMagicUser(bool p_user);

    /**
     * @brief Sets the magic user type. Pass "" to unset.
     */
    void setMagicUserType(const QString& p_type);

    /**
     * @brief Returns the unique ID of the magic user type. Or "", if none.
     */
    const QString& getMagicUserTypeID() const;

    //*********************************************************************
    // KARMA
    //*********************************************************************
    /**
     * @brief Returns the total amount of karma spent.
     */
    int getSpentKarma() const;

    /**
     * @brief Returns the number of available karma points.
     */
    int getAvailableKarma() const;

    //*********************************************************************
    // ATTRIBUTES
    //*********************************************************************
    /**
     * @brief Increase the passed attribute by the passed amount (can also be used to decrease).
     * @param p_attribute   The name of the attribute to increase.
     * @param p_increase    The amount by which the attribute is to be increased (can be negative for decrease).
     *                      May be bigger than the allowed maximum. In that case, the attribute is increased
     *                      only to the maximum.
     * @param p_fromFreebie The amount of the increases/decreases that come from the free attribute points
     *                      during character creation.
     *                      This number can be bigger than the actual number of attribute points left.
     *                      In that case, if we are in guided creation, increasse only to the number of attribute
     *                      points available. The rest will be bought from karma, if possible.
     *                      If we are not in guided creation, this number should always be 0.
     */
    void increaseAttribute(const QString& p_attribute, int p_increase, int p_fromFreebies);

    /**
     * @brief Returns the number of attribute increases for the passed attribute.
     * @param p_attribute       The name of the attribute to check.
     * @param p_fromFreebies    If the increases from free attribute points should be counted.
     * @param p_fromKarma       If the increases from karma points should be counted.
     * @return  The number of attribute increases.
     */
    int getAttributeIncreases(const QString& p_attribute, bool p_fromFreebies = true, bool p_fromKarma = true) const;

    /**
     * @brief Returns the number of available free attribute points.
     */
    int getAvailableAttributePoints() const;

    /**
     * @brief Returns the number of available SPECIAL free attribute points (edge & magic/resonance).
     */
    int getAvailableSpecialAttributePoints() const;

    /**
     * @brief This will reset all spent attribute increases to 0, freeing all freebies and spent karma.
     * @param p_cleanUpSpecialPoints    If this is true, the special attribute points will be cleaned.
     *                                  If this is false, they will NOT be cleaned!
     */
    void cleanUpSpentAttributes(bool p_cleanUpSpecialPoints);

    /**
     * @brief Resets all attribute increases of the passed attribute.
     * @param p_attribute       The name of the attribute.
     * @param p_fromFreebies    If increases from Freebies should be reset.
     * @param p_fromKarma       If increases from Karma should be reset.
     */
    void resetAttributeIncreases(const QString p_attribute, bool p_fromFreebies = true, bool p_fromKarma = true);

    //*********************************************************************
    // SKILLS
    //*********************************************************************
    /**
     * @brief Will increase/decrease the skill. Using skill points first, then karma.
     * @param p_skill           The ID of the skill.
     * @param p_numIncreases    The number of increases (negative for decreases).
     */
    void increaseSkill(const QString& p_skill, int p_numIncreases);

    /**
     * @brief Returns the number of skill increases for the passed skill.
     * @param p_skill           The name of the skill to check.
     * @param p_fromFreebies    If the increases from free skills should be counted.
     * @param p_fromSkillPoints If the increases from skill points should be counted.
     * @param p_fromKarma       If the increases from karma points should be counted.
     * @return  The number of skill increases.
     */
    int getSkillIncreases(const QString& p_skill, bool p_fromFreebies = true,
                          bool p_fromSkillPoints = true,
                          bool p_fromKarma = true) const;

    /**
     * @brief Returns the number of remaining free skills.
     * @param p_skillGroups If this is true, the number of remaining free skill groups will be returned instead.
     */
    int getAvailableFreeSkills(bool p_skillGroups) const;

    /**
     * @brief Returns the number of available skill points.
     * @param p_groupPoints If this is true, the number of skill points for groups will be returned.
     */
    int getAvailableSkillPoints(bool p_groupPoints) const;

    /**
     * @brief Adds the passed free skill.
     * @param p_id      The ID of the skill.
     * @param p_source  A hint to tell the program where to look for the value of the free skill.
     */
    void addFreeSkill(const QString& p_id, Priority p_source);

    /**
     * @brief Removes the passed free skill.
     * @param p_id  The ID of the skill.
     */
    void removeFreeSkill(const QString& p_id);

    /**
     * @brief Returns a list with all skill specializations for the passed skill.
     * @param p_skill   The ID of the skill.
     */
    QStringList getSkillSpecializations(const QString& p_skill) const;

    /**
     * @brief Adds the passed skill specialization to the passed skill.
     * @param p_skill   The ID of the skill.
     * @param p_spec    The specialization.
     */
    void addSkillSpecialization(const QString& p_skill, const QString& p_spec);

    /**
     * @brief Removes the passed skill specialization from the passed skill.
     * @param p_skill   The ID of the skill.
     * @param p_spec    The specialization.
     */
    void removeSkillSpecialization(const QString& p_skill, const QString& p_spec);

    /**
     * @brief Sets the passed language skill as a native one.
     * @param p_skill   The ID of the language skill.
     */
    void setNativeLanguage(const QString& p_skill);

    /**
     * @brief Will return true if the passed skill was chosen as a native language.
     * @param p_skill   The ID of the language skill.
     */
    bool getIsNativeLanguage(const QString& p_skill) const;

    /**
     * @brief Returns the number of available knowledge/language points.
     */
    int getAvailableKnowledgePoints() const;

    /**
     * @brief Returns true if the passed skill group is intact.
     *          Intact = All skills have the same pure value.
     * @param p_group   The ID of the skill group.
     */
    bool getIsSkillGroupIntact(const QString& p_group) const;

    /**
     * @brief Will reset all increases (except from freebies), specializations, etc. of this skill.
     * @param p_skill           The ID of the skill to reset.
     * @param p_deleteFromRules If true, the skill will be deleted from the rules, if it is a defined one
     *                          and has no increases remaining.
     */
    void resetSkill(const QString& p_skill, bool p_deleteFromRules);

    /**
     * @brief Removes all free skills.
     */
    void resetFreeSkills();

    /**
     * @brief Removes all skill points spent on skills.
     */
    void resetSkillPoints();

    /**
     * @brief Removes all karma spent on skills.
     */
    void resetSkillPurchases();

    /**
     * @brief Removes all skill specializations.
     */
    void resetSkillSpecializations();

    /**
     * @brief Returns a list with the IDs of chosen free skills.
     */
    QStringList getChosenFreeSkills();

    //*********************************************************************
    // SPELLS
    //*********************************************************************
    /**
     * @brief Adds the passed free spell/complex form/adept power.
     * @param p_id      The ID of the spell.
     * @return True if the adding worked out.
     */
    bool addFreeSpell(const QString& p_id);

    /**
     * @brief Removes the passed free spell/complex form/adept power.
     * @param p_id  The ID of the spell.
     */
    void removeFreeSpell(const QString& p_id);

    /**
     * @brief Removes all free spells/complex forms/adept powers.
     */
    void resetFreeSpells();

    /**
     * @brief Returns the number of remaining free spells / complex forms.
     */
    float getAvailableFreeSpells() const;

    /**
     * @brief Returns the number of spell/power points spent on the spell with the passed ID.
     * @param p_id      The ID of the spell.
     */
    float getSpellFreebies(const QString& p_id);

    /**
     * @brief Returns a list with the IDs of chosen free spells.
     */
    QStringList getChosenFreeSpells();

    //*********************************************************************
    // POWER POINTS
    //*********************************************************************
    /**
     * @brief Returns the total number of power points for adept powers.
     */
    float getPowerPoints() const;

    /**
     * @brief Purchases power points to get to the target value.
     * @param p_targetValue     The target value of purchased power points.
     *                          For example, if 2 PP are already purchased and 4 is given to this function,
     *                              2 PP will be purchased.
     *                          You can set the purchased PP to a lower number, too, to regain karma.
     */
    void setPurchasePowerPoints(int p_targetValue);

    /**
     * @brief Resets all purchased adept powers.
     */
    void resetAdeptPowers();

    /**
     * @return The number of purchased power points (purchased from karma).
     */
    int getPurchasedPowerPoints() const;

    /**
     * @return The number of spent power points.
     */
    float getSpentPowerPoints() const;

    /**
     * @brief Returns the number of remaining power points for adept powers.
     */
    float getAvailablePowerPoints() const;

    //*********************************************************************
    // QUALITIES
    //*********************************************************************
    /**
     * @brief Returns all qualities the character has.
     */
    const QMap<QString, int>& getQualities() const;

    /**
     * @brief Tries to add the quality with the passed ID.
     * @param p_id  The ID of the quality to add.
     */
    bool addQuality(const QString& p_id);

    /**
     * @brief Removes the passed quality and disables the effects.
     * @param p_id  The ID of the quality to remove.
     */
    void removeQuality(const QString& p_id);

    /**
     * @brief Checks if all qualities are still valid and removed those that are not.
     * @return True if all qualities are still valid,
     *          false if at leats one was invalid and removed.
     */
    bool validateQualities();

    /**
     * @brief Removes all selected qualities and their effects.
     */
    void resetQualities();

    /**
     * @brief Returns the amount of karma gained from/spent on qualities.
     * @param p_positive    If true, the karma spent on positive qualities is returned.
     *                      If false, karma gained from negative qualities is returned.
     */
    int getQualitiesKarma(bool p_positive) const;

    //*********************************************************************
    // CREATION DATA
    //*********************************************************************
    HouseRules& getHouseRules();

private:
    std::vector<Priority>     _selectedPriorities;

    QString     _name;
    QString     _nick;
    QString     _metatypeID;        // Corresponds to the unique ID of one metatype
    QString     _magicUserType;     // Corresponds to the unique ID of one magic user type

    QMap<QString, int>          _attributeIncreasesFreebies;
    QMap<QString, int>          _attributeIncreasesKarma;
    QMap<QString, int>          _skillIncreasesFreebies;
    QMap<QString, int>          _skillIncreasesSkillPoints;
    QMap<QString, int>          _skillIncreasesKarma;
    QMap<QString, float>        _spellsFromFreebies;
    QMap<QString, float>        _spellsFromKarma;
    QMap<QString, int>          _qualities;
    int                         _purchasedPowerPoints;

    QMap<QString, std::vector<std::pair<QString, bool> > >  _skillSpecializations;

    HouseRules  _houseRules;
};


//---------------------------------------------------------------------------------
inline
void
CharacterChoices::setMetatypeID(const QString& p_uniqueID)
{
    _metatypeID = p_uniqueID;
}

//---------------------------------------------------------------------------------
inline
const QString&
CharacterChoices::getMetatypeID() const
{
    return _metatypeID;
}

//---------------------------------------------------------------------------------
inline
void
CharacterChoices::unsetPriority(Priority p_prio)
{
    for (int i = 0; i < 5; ++i)
    {
        if (_selectedPriorities[i] == p_prio)
        {
            _selectedPriorities[i] = PRIORITY_INVALID;
            break;
        }
    }
}

//---------------------------------------------------------------------------------
inline
int
CharacterChoices::getPriorityIndex(Priority p_prio) const
{
    for (int i = 0; i < 5; ++i)
    {
        if (_selectedPriorities[i] == p_prio)
        {
            return i;
            break;
        }
    }

    return -1;
}

//---------------------------------------------------------------------------------
inline
Priority
CharacterChoices::getPriorityAtIndex(int p_index) const
{
    if (p_index < 0 || p_index > 4)
    {
        return PRIORITY_INVALID;
    }

    return _selectedPriorities[p_index];
}

//---------------------------------------------------------------------------------
inline
void
CharacterChoices::setName(const QString& p_name)
{
    _name = p_name;
}

//---------------------------------------------------------------------------------
inline
const QString&
CharacterChoices::getName() const
{
    return _name;
}

//---------------------------------------------------------------------------------
inline
void
CharacterChoices::setNick(const QString& p_nick)
{
    _nick = p_nick;
}

//---------------------------------------------------------------------------------
inline
const QString&
CharacterChoices::getNick() const
{
    return _nick;
}


//---------------------------------------------------------------------------------
inline
bool
CharacterChoices::getIsMagicUser() const
{
    return (getPriorityIndex(PRIORITY_MAGIC) != 4 && getPriorityIndex(PRIORITY_MAGIC) != -1);
}

//---------------------------------------------------------------------------------
inline
void
CharacterChoices::setIsMagicUser(bool p_user)
{
    if (!p_user)
    {
        setPriority(4, PRIORITY_MAGIC);
    }
}

//---------------------------------------------------------------------------------
inline
void
CharacterChoices::setMagicUserType(const QString& p_type)
{
    _magicUserType = p_type;
}

//---------------------------------------------------------------------------------
inline
const QString&
CharacterChoices::getMagicUserTypeID() const
{
    return _magicUserType;
}

//---------------------------------------------------------------------------------
inline
void
CharacterChoices::resetFreeSkills()
{
    _skillIncreasesFreebies.clear();
}

//---------------------------------------------------------------------------------
inline
void
CharacterChoices::resetSkillPoints()
{
    _skillIncreasesSkillPoints.clear();
}

//---------------------------------------------------------------------------------
inline
void
CharacterChoices::resetSkillPurchases()
{
    _skillIncreasesKarma.clear();
}

//---------------------------------------------------------------------------------
inline
void
CharacterChoices::resetSkillSpecializations()
{
    _skillSpecializations.clear();
}

//---------------------------------------------------------------------------------
inline
int
CharacterChoices::getPurchasedPowerPoints() const
{
    return _purchasedPowerPoints;
}

//---------------------------------------------------------------------------------
inline
HouseRules&
CharacterChoices::getHouseRules()
{
    return _houseRules;
}

//---------------------------------------------------------------------------------
inline
const QMap<QString, int>&
CharacterChoices::getQualities() const
{
    return _qualities;
}

#endif // CHARACTERDATA_H
