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

private:
    std::vector<Priorities>     _selectedPriorities;

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

#endif // CHARACTERDATA_H
