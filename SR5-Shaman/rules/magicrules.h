#ifndef MAGICRULES_H
#define MAGICRULES_H

#include <QString>
#include <QStringList>
#include <QMap>
#include <vector>
#include <QObject>

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

// Spell enums
enum SpellType
{
    SPELLTYPE_INVALID = -1,
    SPELLTYPE_PHYSICAL,
    SPELLTYPE_MAGICAL,
    NUM_SPELLTYPES
};
enum SpellRange
{
    SPELLRANGE_INVALID = -1,
    SPELLRANGE_TOUCH,
    SPELLRANGE_LOS,
    SPELLRANGE_LOS_AREA,
    NUM_SPELLRANGES
};
enum SpellDamage
{
    SPELLDAMAGE_INVALID = -1,
    SPELLDAMAGE_PHYSICAL,
    SPELLDAMAGE_MENTAL,
    NUM_SPELLDAMAGES
};
enum SpellDuration
{
    SPELLDURATION_INVALID = -1,
    SPELLDURATION_INSTANT,
    SPELLDURATION_SUSTAINED,
    SPELLDURATION_PERMANENT,
    NUM_SPELLDURATIONS
};

/**
 * @brief The definition of one spell category.
 */
struct SpellCategoryDefinition
{
    QMap<QString, QString>              translations;
};

/**
 * @brief The definition of one spell.
 */
struct SpellDefinition
{
    SpellDefinition()
        : type(SPELLTYPE_INVALID)
        , range(SPELLRANGE_INVALID)
        , damageType(SPELLDAMAGE_INVALID)
        , duration(SPELLDURATION_INVALID)
    {}

    QMap<QString, QString>              translations;
    QString                             category;
    SpellType                           type;
    SpellRange                          range;
    SpellDamage                         damageType;
    SpellDuration                       duration;
    QString                             drain;
    QStringList                         descriptors;
    bool                                requiresCustom;
    QString                             customString;
    bool                                essenceEffect;
};

// Adept Power enums
enum CostType
{
    COSTTYPE_INVALID = -1,
    COSTTYPE_PER_LEVEL,
    COSTTYPE_NORMAL,
    COSTTYPE_ARRAY,
    NUM_COSTTYPES
};
enum ActivationType
{
    ACTIVATIONTYPE_INVALID = -1,
    ACTIVATIONTYPE_INTERRUPT,
    ACTIVATIONTYPE_FREE,
    ACTIVATIONTYPE_SIMPLE,
    NUM_ACTIVATIONTYPES
};

/**
 * @brief The definition of one adept power.
 */
struct AdeptPowerDefinition
{
    AdeptPowerDefinition()
        : costType(COSTTYPE_INVALID)
    {}

    QMap<QString, QString>              translations;
    CostType                            costType;
    std::vector<float>                  costArray;
    QString                             activationType;
    bool                                requiresCustom;
    QString                             customString;
};


// Complex Form enums
enum TargetType
{
    TARGETTYPE_INVALID = -1,
    TARGETTYPE_PERSONA,
    TARGETTYPE_DEVICE,
    TARGETTYPE_FILE,
    TARGETTYPE_SPRITE,
    NUM_TARGETTYPES
};

/**
 * @brief The definition of one complex form.
 */
struct ComplexFormDefinition
{
    ComplexFormDefinition()
        : targetType(TARGETTYPE_INVALID)
        , duration(SPELLDURATION_INVALID)
    {}

    QMap<QString, QString>              translations;
    TargetType                          targetType;
    SpellDuration                       duration;
    QString                             fadingValue;
    bool                                requiresCustom;
    QString                             customString;
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
     * @brief Returns the map of magic type definitions.
     */
    const QMap<QString, MagicTypeDefinition*>& getAllMagicTypeDefinitions() const;

    /**
     * @brief Returns the map of spell category definitions.
     */
    const QMap<QString, SpellCategoryDefinition*>& getAllSpellCategoryDefinitions() const;

    /**
     * @brief Returns the map of spell definitions.
     */
    const QMap<QString, SpellDefinition*>& getAllSpellDefinitions() const;

    /**
     * @brief Returns a map of spell definitions that are in the passed category.
     * @param p_uniqueID    The unique ID of the category.
     */
    QMap<QString, SpellDefinition*> getAllSpellDefinitionsByCategory(const QString& p_uniqueID) const;

    /**
     * @brief Returns the map of adept power definitions.
     */
    const QMap<QString, AdeptPowerDefinition*>& getAllAdeptPowerDefinitions() const;

    /**
     * @brief Returns the map of complex forms definitions.
     */
    const QMap<QString, ComplexFormDefinition*>& getAllComplexFormDefinitions() const;

    /**
     * @brief Returns the definition of the magic type with the passed ID.
     * @note    Does NOT check if the id exists. Use getAllDefinitions() for that.
     */
    const MagicTypeDefinition& getMagicTypeDefinition(const QString& p_uniqueId) const;

    /**
     * @brief Returns the definition of the spell with the passed ID.
     * @note    Does NOT check if the id exists. Use getAllDefinitions() for that.
     */
    const SpellDefinition& getSpellDefinition(const QString& p_uniqueID) const;

    /**
     * @brief Returns the definition of the adept power with the passed ID.
     * @note    Does NOT check if the id exists. Use getAllDefinitions() for that.
     */
    const AdeptPowerDefinition& getAdeptPowerDefinition(const QString& p_uniqueID) const;

    /**
     * @brief Returns the definition of the complex form with the passed ID.
     * @note    Does NOT check if the id exists. Use getAllDefinitions() for that.
     */
    const ComplexFormDefinition& getComplexFormDefinition(const QString& p_uniqueID) const;

    /**
     * @brief Convenience function that returns the translation for the current locale.
     * @param p_uniqueID    The unique ID of the category.
     * @return
     */
    QString getSpellCategoryTranslation(const QString& p_uniqueID) const;

private:
    QMap<QString, MagicTypeDefinition*>     _typeDefinitions;
    QMap<QString, SpellCategoryDefinition*> _spellCategoryDefinitions;
    QMap<QString, SpellDefinition*>         _spellDefinitions;
    QMap<QString, AdeptPowerDefinition*>    _adeptPowerDefinitions;
    QMap<QString, ComplexFormDefinition*>   _complexFormDefinitions;
};

//---------------------------------------------------------------------------------
inline
const QMap<QString, MagicTypeDefinition*>&
MagicRules::getAllMagicTypeDefinitions() const
{
    return _typeDefinitions;
}

//---------------------------------------------------------------------------------
inline
const QMap<QString, SpellCategoryDefinition*>&
MagicRules::getAllSpellCategoryDefinitions() const
{
    return _spellCategoryDefinitions;
}

//---------------------------------------------------------------------------------
inline
const QMap<QString, SpellDefinition*>&
MagicRules::getAllSpellDefinitions() const
{
    return _spellDefinitions;
}

//---------------------------------------------------------------------------------
inline
const QMap<QString, AdeptPowerDefinition*>&
MagicRules::getAllAdeptPowerDefinitions() const
{
    return _adeptPowerDefinitions;
}

//---------------------------------------------------------------------------------
inline
const QMap<QString, ComplexFormDefinition*>&
MagicRules::getAllComplexFormDefinitions() const
{
    return _complexFormDefinitions;
}

//---------------------------------------------------------------------------------
inline
const MagicTypeDefinition&
MagicRules::getMagicTypeDefinition(const QString& p_uniqueId) const
{
    return *(_typeDefinitions[p_uniqueId]);
}

//---------------------------------------------------------------------------------
inline
const SpellDefinition&
MagicRules::getSpellDefinition(const QString& p_uniqueId) const
{
    return *(_spellDefinitions[p_uniqueId]);
}

//---------------------------------------------------------------------------------
inline
const AdeptPowerDefinition&
MagicRules::getAdeptPowerDefinition(const QString& p_uniqueId) const
{
    return *(_adeptPowerDefinitions[p_uniqueId]);
}

//---------------------------------------------------------------------------------
inline
const ComplexFormDefinition&
MagicRules::getComplexFormDefinition(const QString& p_uniqueId) const
{
    return *(_complexFormDefinitions[p_uniqueId]);
}


#endif // MAGICRULES_H
