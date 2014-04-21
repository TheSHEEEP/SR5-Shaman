#ifndef QUALITYRULES_H
#define QUALITYRULES_H

#include <QString>
#include <QMap>
#include <vector>
#include <QObject>

#include "commonrules.h"

class Effect;

/**
 * @brief This is the definition of a single quality.
 *          Also has some additional functionality to serve as a model item for Views.
 */
class QualityDefinition
{
public:
    /**
     * @brief Constructor.
     */
    QualityDefinition(QualityDefinition* p_parent = NULL);

    /**
     * @brief Copy constructor.
     */
    QualityDefinition(const QualityDefinition& p_other);

    /**
     * @brief Destructor.
     */
    ~QualityDefinition();

    /**
     * @brief Returns true if this item has a child with the passed value.
     * @param p_id  The ID to look for.
     */
    bool hasChild(const QString& p_id) const;

    /**
     * @brief Returns the child with the passed value. Or NULL, if no child was found.
     */
    QualityDefinition* getChild(const QString& p_id) const;

    // Those two seem to be redundant, but are useful when this definition is used as
    // a model item inside a View
    QualityDefinition*                parent;
    std::vector<QualityDefinition*>   children;

    QString                         id;
    bool                            isCategory;
    bool                            isPositive;
    CostType                        costType;
    std::vector<float>              costArray;
    bool                            isUserDefined;
    bool                            requiresCustom;
    QString                         custom;     // This can be used for further definition, like the name defined by the user
    QString                         base;
    std::vector<Effect*>            effects;
    QMap<QString, QString>          translations;
};

Q_DECLARE_METATYPE(QualityDefinition)

/**
 * @brief This class holds all information regarding quality specific rules.
 *          For example, this class holds all quality definitions.
 * @author  TheSHEEEP
 */
class QualityRules
{
public:
    /**
     * @brief Constructor.
     */
    QualityRules();

    /**
     * @brief Destructor.
     */
    ~QualityRules();

    /**
     * @brief Initializes the quality rules by reading the passed JSON file.
     * @param p_jsonFile    The JSON file to parse.
     */
    void initialize(const QString& p_jsonFile);

    /**
     * @return The root item of the model. Use this for display in trees, etc.
     */
    QualityDefinition* getModelRootItem();

    /**
     * @brief Returns the map of metatype definitions.
     */
    const QMap<QString, QualityDefinition*>& getAllDefinitions() const;

    /**
     * @brief Returns the definition of the quality with the passed ID.
     * @note    Does NOT check if the id exists. Use getAllDefinitions() for that.
     */
    const QualityDefinition& getDefinition(const QString& p_uniqueId) const;

    /**
     * @brief Returns the definitions of the qualities that contain the passed part of the ID.
     *          Useful to get all user defined qualities of a type.
     */
    std::vector<std::pair<QString, QualityDefinition*> > getDefinitionsContaining(const QString& p_idPart) const;

    /**
     * @brief Will construct a new quality out of the passed quality and the customization.
     *          A new quality will only be constructed if this specialization does not exist already.
     * @param p_id          The ID of the original quality.
     * @param p_customValue The custom value.
     * @return The ID of the new quality.
     */
    QString constructCustomizedQuality(const QString &p_id, const QString &p_customValue);

    /**
     * @brief Returns a list with all IDs of custom versions of the passed quality.
     * @param p_id  The ID to look for. Can be the base or a custom ability itself.
     */
    QStringList getCustomVersions(const QString& p_id) const;

private:
    QMap<QString, QualityDefinition*>  _definitions;

    QualityDefinition*    _rootItem;  // The root item of the skills, used to display in tree models
};

//---------------------------------------------------------------------------------
inline
QualityDefinition*
QualityRules::getModelRootItem()
{
    return _rootItem;
}

//---------------------------------------------------------------------------------
inline
const QMap<QString, QualityDefinition*>&
QualityRules::getAllDefinitions() const
{
    return _definitions;
}

//---------------------------------------------------------------------------------
inline
const QualityDefinition&
QualityRules::getDefinition(const QString& p_uniqueId) const
{
    return *(_definitions[p_uniqueId]);
}

#endif // QUALITYRULES_H
