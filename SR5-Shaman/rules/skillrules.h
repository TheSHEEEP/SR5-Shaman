#ifndef SKILLRULES_H
#define SKILLRULES_H

#include <QString>
#include <QMap>
#include <vector>
#include <QPair>
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
    SKILL_TYPE_LANGUAGE,
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
    KNOWLEDGE_TYPE_LANGUAGE,
    NUM_KNOWLEDGE_TYPES
};

/**
 * @brief This is the definition of a single skill.
 *          Also has some additional functionality to serve as a model item for Views.
 */
class SkillDefinition
{
public:
    /**
     * @brief Constructor.
     */
    SkillDefinition(SkillDefinition* p_parent = NULL);

    /**
     * @brief Copy constructor.
     */
    SkillDefinition(const SkillDefinition& p_other);

    /**
     * @brief Destructor.
     */
    ~SkillDefinition();

    /**
     * @brief Returns true if this item has a child with the passed value.
     * @param p_id  The ID to look for.
     */
    bool hasChild(const QString& p_id) const;

    /**
     * @brief Returns the child with the passed value. Or NULL, if no child was found.
     */
    SkillDefinition* getChild(const QString& p_id) const;

    // Those two seem to be redundant, but are useful when this definition is used as
    // a model item inside a View
    SkillDefinition*                parent;
    std::vector<SkillDefinition*>   children;

    QString                         id;
    bool                            isCategory;
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

Q_DECLARE_METATYPE(SkillDefinition)
Q_DECLARE_METATYPE(SkillDefinition*)

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
     * @brief Returns true if a skill with that ID exists.
     * @param p_id  The ID to look for.
     */
    bool getIsValidSkill(const QString& p_id) const;

    /**
     * @brief Returns the localized name of the passed type.
     */
    QString getTypeString(SkillType p_type) const;

    /**
     * @return The root item of the model. Use this for display in trees, etc.
     */
    SkillDefinition* getModelRootItem();

    /**
     * @brief Returns the number of skill points for skills/skill groups.
     * @param p_prioIndex   The chosen priority to look at.
     * @param p_groupPoints If this is true, skill points for groups will be returned.
     */
    int getNumSkillPoints(int p_prioIndex, bool p_groupPoints) const;

    /**
     * @brief Returns the karma cost/gain to inrease/decrease the passed skill.
     * @param p_skill       The skill to look at.
     * @param p_oldValue    The old value.
     * @param p_newValue    The new value.
     */
    int calculateSkillIncreaseCost(const QString& p_skill, int p_oldValue, int p_newValue);

    /**
     * @brief Returns the maximum number of increases for the passed values.
     * @param p_skill           The skill to look at.
     * @param p_oldValue        The start value.
     * @param p_maxValue        The maximum value to increase to.
     * @param p_availableKarma  The available karma.
     */
    int calculateMaximumSkillIncrease(  const QString& p_skill, int p_oldValue,
                                        int p_maxValue, int p_availableKarma) const;

    /**
     * @brief Calculates and returns the number of knowledge/language points for the passed attribute values.
     * @param p_intuition   The intuition value.
     * @param p_logic       The logic value.
     */
    int calculateKnowledgePoints(int p_intuition, int p_logic) const;

    /**
     * @brief Returns the map of metatype definitions.
     */
    const QMap<QString, SkillDefinition*>& getAllDefinitions() const;

    /**
     * @brief Returns the definition of the skill with the passed ID.
     * @note    Does NOT check if the id exists. Use getAllDefinitions() for that.
     */
    const SkillDefinition& getDefinition(const QString& p_uniqueId) const;

    /**
     * @brief Returns the definitions of the skills that contain the passed part of the ID.
     *          Useful to get all user defined skills of a type (like EXOTIC_MELEE_WEAPON).
     */
    std::vector<std::pair<QString, SkillDefinition*> > getDefinitionsContaining(const QString& p_idPart,
                                                                                bool p_onlyGroups) const;

    /**
     * @brief Returns the definitions in vector that fit the passed configuration.
     * @param p_type        The skill type to look for.
     * @param p_onlyGroups  If this is true, only skill groups will be returned.
     *                      If this is false, only skills that are not a group will be returned.
     */
    std::vector<std::pair<QString, SkillDefinition*> > getDefinitionsByType(SkillType p_type,
                                                                            bool p_onlyGroups) const;

    /**
     * @brief Will construct a new skill out of the passed skill and the customization.
     *          A new skill will only be constructed if this specialization does not exist already.
     * @param p_id          The ID of the original skill.
     * @param p_customValue The custom value.
     * @return The ID of the new skill.
     */
    QString constructCustomizedSkill(const QString &p_id, const QString &p_customValue);

    /**
     * @brief Will construct a new knowledge or language skill.
     * @param p_customValue     The name of the the new skill.
     * @param p_isKnowledge     If this is true, this will be a knowledge skill, if is is false, a language.
     * @param p_knowledgeType   The knowledge type. Ignored if p_isKnowledge is false.
     * @return
     */
    QString constructKnowledgeSkill(const QString &p_customValue, bool p_isKnowledge, KnowledgeType p_knowledgeType);

private:
    QMap<QString, SkillDefinition*> _definitions;
    std::vector<QPair<int, int>>    _skillPoints;

    SkillDefinition*    _rootItem;  // The root item of the skills, used to display in tree models
};


//---------------------------------------------------------------------------------
inline
bool
SkillRules::getIsValidSkill(const QString& p_id) const
{
    return _definitions.contains(p_id);
}

//---------------------------------------------------------------------------------
inline
SkillDefinition*
SkillRules::getModelRootItem()
{
    return _rootItem;
}

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
