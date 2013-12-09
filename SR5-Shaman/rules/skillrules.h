#ifndef SKILLRULES_H
#define SKILLRULES_H

#include <QString>
#include <QMap>
#include <vector>
#include <QObject>

// Skill categories
enum SkillType
{
    SKILL_TYPE_INVALID = -1,
    SKILL_TYPE_COMBAT,
    SKILL_TYPE_PHYSICAL,
    SKILL_TYPE_SOCIAL,
    SKILL_TYPE_MAGIC,
    SKILL_TYPE_RESONANCE,
    SKILL_TYPE_TECHNICAL,
    SKILL_TYPE_VEHICLE,
    SKILL_TYPE_KNOWLEDGE,
    NUM_SKILL_TYPES
};

// Knowledge skill types
enum KnowledgeType
{
    KNOWLEDGE_TYPE_INVALID = -1,
    KNOWLEDGE_TYPE_ACADEMIC,
    KNOWLEDGE_TYPE_INTEREST,
    KNOWLEDGE_TYPE_PROFESSIONAL,
    KNOWLEDGE_TYPE_STREET,
    NUM_KNOWLEDGE_TYPES
};

/**
 * @brief The definition of one skill or skill group.
 */
struct SkillDefinition
{

public:
    /**
     * @brief Constructor.
     */
    SkillDefinition()
        : isGroup(false), isLanguage(false), isUserDefined(false), requiresCustom(false)
        , knowledgeType(KNOWLEDGE_TYPE_INVALID)
        , custom("")
        , attribute("")
    {}

    bool                            isGroup;
    bool                            isLanguage;
    bool                            isUserDefined;
    bool                            requiresCustom;
    SkillType                       type;
    KnowledgeType                   knowledgeType;
    QString                         custom;     // This can be used for further definition, like the name defined by the user
    QString                         attribute;
    QString                         group;
    QMap<QString, QString>          translations;
    QMap<QString, SkillDefinition*> groupSkills;
};

/**
 * @brief This class holds all information regarding magic specific rules.
 *          For example, this class holds all magic type definitions.
 * @author  TheSHEEEP
 */
class SkillRules
{
public:
    /**
     * @brief Constructor.
     */
    SkillRules();

    /**
     * @brief Destructor.
     */
    ~SkillRules();

    /**
     * @brief Initializes the skill rules by reading the passed JSON file.
     * @param p_jsonFile    The JSON file to parse.
     */
    void initialize(const QString& p_jsonFile);

    /**
     * @brief Returns the localized name of the passed type.
     */
    QString getTypeString(SkillType p_type) const;

    /**
     * @brief Returns the map of metatype definitions.
     */
    const QMap<QString, SkillDefinition*>& getAllDefinitions() const;

    /**
     * @brief Returns the definition of the metatype with the passed ID.
     * @note    Does NOT check if the id exists. Use getAllDefinitions() for that.
     */
    const SkillDefinition& getDefinition(const QString& p_uniqueId) const;

    /**
     * @brief Returns the definitions in vector that fit the passed configuration.
     * @param p_type        The skill type to look for.
     * @param p_onlyGroups  If this is true, only skill groups will be returned.
     *                      If this is false, only skills that are not a group will be returned.
     */
    std::vector<std::pair<QString, SkillDefinition*> > getDefinitionsByType(SkillType p_type, bool p_onlyGroups) const;

private:
    QMap<QString, SkillDefinition*>  _definitions;
};

//---------------------------------------------------------------------------------
inline
const QMap<QString, SkillDefinition*>&
SkillRules::getAllDefinitions() const
{
    return _definitions;
}

//---------------------------------------------------------------------------------
inline
const SkillDefinition&
SkillRules::getDefinition(const QString& p_uniqueId) const
{
    return *(_definitions[p_uniqueId]);
}

#endif // SKILLRULES_H
