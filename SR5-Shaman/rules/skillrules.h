#ifndef SKILLRULES_H
#define SKILLRULES_H

#include <QString>
#include <QMap>
#include <vector>

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
        : isGroup(false), isLanguage(false), isUserDefined(false)
        , knowledgeType(KNOWLEDGE_TYPE_INVALID)
        , custom("")
        , attribute("")
    {}

    bool                            isGroup;
    bool                            isLanguage;
    bool                            isUserDefined;
    KnowledgeType                   knowledgeType;
    QString                         custom;     // This can be used for further definition, like the name defined by the user
    QString                         attribute;
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
     * @brief Returns the map of metatype definitions.
     */
    const QMap<QString, SkillDefinition*>& getAllDefinitions() const;

    /**
     * @brief Returns the definition of the metatype with the passed ID.
     * @note    Does NOT check if the id exists. Use getAllDefinitions() for that.
     */
    const SkillDefinition& getDefinition(const QString& p_uniqueId) const;

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
