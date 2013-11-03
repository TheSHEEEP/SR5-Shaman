#ifndef CHARACTERDATA_H
#define CHARACTERDATA_H

#include "rules/rules.h"
#include <vector>
#include <QString>

// Helpful defines
#define CHARACTER CharacterData::getSingletonPtr()

/**
 * @brief This singleton class holds all player choices of the character creation.
 *          Does NOT hold any information about rules.
 */
class CharacterData
{
private:
    static CharacterData* _instance;

    /**
     * @brief Constructor.
     */
    CharacterData();

public:
    /**
     * @brief Returns a pointer to the CharacterData.
     */
    static CharacterData* getSingletonPtr()
    {
        if (_instance == 0)
        {
            _instance = new CharacterData();
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
     *          If the index is already taken by another priority, two things can happen:
     *              a) If the passed priority was previously at another index, the two will
     *                 switch indices.
     *              b) If the passed priority is new, the current priority at the passed index
     *                 will simply be overwritten.
     * @param p_priorityIndex   The index of the priority (0-4 is A-E)
     * @param p_prio            The priority type to set.
     */
    void setPriority(int p_priorityIndex, Priority p_prio);

    /**
     * @brief Returns the priority index of the passed priority. Or -1 if the priority does not yet have an index.
     */
    int getPriorityIndex(Priority p_prio) const;

    /**
     * @brief Will unset the priority of the chosen type, setting it to invalid.
     *          i.e.    If you pass PRIORITY_METATYPE and it is currently at priority D,
     *                  priority D will be set to invalid.
     */
    void unsetPriority(Priority p_prio);

private:
    std::vector<Priority>     _selectedPriorities;

    QString     _metatypeID;    // Corresponds to the unique ID of one metatype
};


//---------------------------------------------------------------------------------
inline
void
CharacterData::setMetatypeID(const QString& p_uniqueID)
{
    _metatypeID = p_uniqueID;
}

//---------------------------------------------------------------------------------
inline
const QString&
CharacterData::getMetatypeID() const
{
    return _metatypeID;
}

//---------------------------------------------------------------------------------
inline
void
CharacterData::unsetPriority(Priority p_prio)
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
CharacterData::getPriorityIndex(Priority p_prio) const
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

#endif // CHARACTERDATA_H
