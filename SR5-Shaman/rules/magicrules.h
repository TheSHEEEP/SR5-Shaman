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
    {
        freeSkills.first = -1;
        freeSkills.second = -1;
        freeSkillGroup.first = -1;
        freeSkillGroup.second = -1;
    }

    int                 startingMagic;
    int                 freeSpells;
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
 * @brief The definition of one spell.
 */
struct SpellDefinition
{
    SpellDefinition()
        : isSpellCategory(false)
        , type(SPELLTYPE_INVALID)
        , range(SPELLRANGE_INVALID)
        , damageType(SPELLDAMAGE_INVALID)
        , duration(SPELLDURATION_INVALID)
    {}

    bool                                isSpellCategory;
    QString                             category;
    SpellType                           type;
    SpellRange                          range;
    SpellDamage                         damageType;
    SpellDuration                       duration;
    QString                             drain;
    QStringList                         descriptors;
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

    CostType                            costType;
    std::vector<float>                  costArray;
    ActivationType                      activationType;
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

    TargetType                          targetType;
    SpellDuration                       duration;
    QString                             fadingValue;
};

// Magic ability types
enum MagicAbilityType
{
    MAGICABILITYTYPE_INVALID = -1,
    MAGICABILITYTYPE_SPELL,
    MAGICABILITYTYPE_ADEPT_POWER,
    MAGICABILITYTYPE_COMPLEX_FORM,
    NUM_MAGICABILITYTYPES
};

class CustomChoice;
class Effect;

/**
 * @brief This is the definition of a single magical ability.
 *          It can be a spell, an adept power or a complex form, depending on its type.
 *          Also has some additional functionality to serve as a model item for Views.
 */
class MagicAbilityDefinition
{
public:
    /**
     * @brief Constructor.
     */
    MagicAbilityDefinition(MagicAbilityDefinition* p_parent = NULL);

    /**
     * @brief Copy constructor.
     */
    MagicAbilityDefinition(const MagicAbilityDefinition& p_other);

    /**
     * @brief Destructor.
     */
    ~MagicAbilityDefinition();

    /**
     * @brief Returns true if this item has a child with the passed value.
     * @param p_id  The ID to look for.
     */
    bool hasChild(const QString& p_id) const;

    /**
     * @brief Returns the child with the passed value. Or NULL, if no child was found.
     */
    MagicAbilityDefinition* getChild(const QString& p_id) const;

    MagicAbilityDefinition*                 parent;
    std::vector<MagicAbilityDefinition*>    children;

    QString                         id;
    bool                            isCategory;
    bool                            requiresCustom;
    QString                         customString;
    CustomChoice*                   customChoices;
    std::vector<Effect*>            effects;
    bool                            isUserDefined;
    QMap<QString, QString>          translations;

    MagicAbilityType                abilityType;
    SpellDefinition*                spell;
    AdeptPowerDefinition*           adeptPower;
    ComplexFormDefinition*          complexForm;
};

Q_DECLARE_METATYPE(MagicAbilityDefinition)

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
     * @return The root item of the model. Use this for display in trees, etc.
     */
    MagicAbilityDefinition* getModelRootItem();

    /**
     * @brief Returns the map of magic type definitions.
     */
    const QMap<QString, MagicTypeDefinition*>& getAllMagicTypeDefinitions() const;

    /**
     * @brief Returns the map of spell category definitions.
     */
    const QMap<QString, SpellDefinition*>& getAllSpellCategoryDefinitions() const;

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
     * @brief Returns the definition of the ability with the passed name.
     * @note    Does NOT check if the id exists. Use getAllDefinitions() for that.
     */
    const MagicAbilityDefinition& getDefinition(const QString& p_uniqueID) const;

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

    /**
     * @brief Will construct a new spell out of the passed spell and the customization.
     *          A new spell will only be constructed if this specialization does not exist already.
     * @param p_id          The ID of the original spell.
     * @param p_customValue The custom value.
     * @param p_translation The translation to show.
     * @return The ID of the new spell.
     */
    QString constructCustomizedSpell(const QString &p_id, const QString &p_customValue, const QString &p_translation);

    /**
     * @return The karma cost of one power point.
     */
    int getPowerPointPurchaseCost() const;

private:
    // Some redundant data here, but it makes access faster
    QMap<QString, MagicAbilityDefinition*>  _definitions;
    QMap<QString, MagicTypeDefinition*>     _typeDefinitions;
    QMap<QString, SpellDefinition*>         _spellCategoryDefinitions;
    QMap<QString, SpellDefinition*>         _spellDefinitions;
    QMap<QString, AdeptPowerDefinition*>    _adeptPowerDefinitions;
    QMap<QString, ComplexFormDefinition*>   _complexFormDefinitions;

    MagicAbilityDefinition* _rootItem; // The root item of the magic abilities, used to display in tree models
};

//---------------------------------------------------------------------------------
inline
MagicAbilityDefinition*
MagicRules::getModelRootItem()
{
    return _rootItem;
}

//---------------------------------------------------------------------------------
inline
const QMap<QString, MagicTypeDefinition*>&
MagicRules::getAllMagicTypeDefinitions() const
{
    return _typeDefinitions;
}

//---------------------------------------------------------------------------------
inline
const QMap<QString, SpellDefinition*>&
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
const MagicAbilityDefinition&
MagicRules::getDefinition(const QString& p_uniqueID) const
{
    return *(_definitions[p_uniqueID]);
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

//---------------------------------------------------------------------------------
inline
int
MagicRules::getPowerPointPurchaseCost() const
{
    // TODO: Make this configurable?
    return 2;
}
#endif // MAGICRULES_H
