#ifndef MAGICRULES_H
#define MAGICRULES_H

#include <QString>
#include <QMap>
#include <vector>


/**
 * @brief The definition of one magic type priority.
 */
struct MagicTypePriorityDefinition
{
    MagicTypePriorityDefinition()
        : startingMagic(-1)
        , freeSpells(-1)
        , freePowerPoints(-1)
    {
        freeSkills.first = -1;
        freeSkills.second = -1;
        freeSkillGroup.first = -1;
        freeSkillGroup.second = -1;
    }

    int                 startingMagic;
    int                 freeSpells;
    int                 freePowerPoints;
    std::pair<int, int> freeSkills;
    std::pair<int, int> freeSkillGroup;
};

/**
 * @brief The definition of one magic type.
 */
struct MagicTypeDefinition
{
    QMap<QString, QString>                  translations;
    std::vector<QString>                    types;
    QMap<int, MagicTypePriorityDefinition*>  priorities;
};

/**
 * @brief The definition of one magic type.
 */
struct SpellDefinition
{
    QMap<QString, QString>                  translations;
    std::vector<QString>                    types;
};

/**
 * @brief This class holds all information regarding magic specific rules.
 *          For example, this class holds all magic type definitions.
 * @author  TheSHEEEP
 */
class MagicRules
{
public:
    /**
     * @brief Constructor.
     */
    MagicRules();

    /**
     * @brief Destructor.
     */
    ~MagicRules();

    /**
     * @brief Initializes the magic rules by reading the passed JSON file.
     * @param p_jsonFile    The JSON file to parse.
     */
    void initialize(const QString& p_jsonFile);

    /**
     * @brief Returns the map of metatype definitions.
     */
    const QMap<QString, MagicTypeDefinition*>& getAllDefinitions() const;

    /**
     * @brief Returns the definition of the metatype with the passed ID.
     * @note    Does NOT check if the id exists. Use getAllDefinitions() for that.
     */
    const MagicTypeDefinition& getDefinition(const QString& p_uniqueId) const;

private:
    QMap<QString, MagicTypeDefinition*>  _typeDefinitions;
    QMap<QString, SpellDefinition*>  _spellDefinitions;
};

//---------------------------------------------------------------------------------
inline
const QMap<QString, MagicTypeDefinition*>&
MagicRules::getAllDefinitions() const
{
    return _typeDefinitions;
}

//---------------------------------------------------------------------------------
inline
const MagicTypeDefinition&
MagicRules::getDefinition(const QString& p_uniqueId) const
{
    return *(_typeDefinitions[p_uniqueId]);
}


#endif // MAGICRULES_H
