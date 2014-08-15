#ifndef RESOURCERULES_H
#define RESOURCERULES_H

#include <QString>
#include <QStringList>
#include <QMap>
#include <vector>
#include <QPair>
#include <QObject>

class ResourceEffect;
class QJsonObject;

// All those resource infos and definitions are just too many to spam into the header of the rules
#include "resourcerules.inl"

/**
 * @brief This class holds all information regarding resources.
 *          Meaning cyberware, bioware, weapons, vehicles, housing, etc.
 * @author  TheSHEEEP
 */
class ResourceRules
{
public:
    /**
     * @brief Constructor.
     */
    ResourceRules();

    /**
     * @brief Destructor.
     */
    ~ResourceRules();

    /**
     * @brief Initializes the resource rules by reading the passed JSON file.
     * @param p_jsonFile    The JSON file to parse.
     */
    void initialize(const QString& p_jsonFile);

    /**
     * @brief Returns the localized name of the passed type.
     */
    QString getTypeString(ResourceType p_type) const;

    /**
     * @return The root item of the model. Use this for display in trees, etc.
     */
    ResourceDefinition* getModelRootItem();

    /**
     * @brief Returns the number of nuyen available for a character with the passed resource priority.
     * @param p_prioIndex   The index of the priority.
     */
    int getNuyenForPriority(int p_prioIndex) const;

    /**
     * @brief Returns the map of resource definitions.
     */
    const QMap<QString, ResourceDefinition*>& getAllDefinitions() const;

    /**
     * @brief Returns the definition of the resource with the passed ID.
     * @note    Does NOT check if the id exists. Use getAllDefinitions() for that.
     */
    const ResourceDefinition& getDefinition(const QString& p_uniqueId) const;

    /**
     * @brief Returns the definitions of the resources that contain the passed part of the ID.
     *          Useful to get all user defined resources of a type.
     */
    std::vector<std::pair<QString, ResourceDefinition*> > getDefinitionsContaining(const QString& p_idPart) const;

    /**
     * @brief Returns the definitions in vector that fit the passed configuration.
     * @param p_type        The resource type to look for.
     */
    std::vector<std::pair<QString, ResourceDefinition*> > getDefinitionsByType(ResourceType p_type) const;

    /**
     * @brief Will completely remove the passed resource from the definitions.
     *          USE WITH CAUTION!
     * @param p_resource   The ID of the resource.
     */
    void removeResource(const QString& p_resource);

    /**
     * @brief Will construct a new resource out of the passed resource and the customization.
     *          A new resource will only be constructed if this specialization does not exist already.
     * @param p_id          The ID of the original resource.
     * @param p_customValue The custom value.
     * @return The ID of the new resource.
     */
    QString constructCustomizedResource(const QString &p_id, const QString &p_customValue);

private:
    QMap<QString, ResourceDefinition*>  _definitions;
    std::vector<int>                    _nuyenPerPrio;

    ResourceDefinition*    _rootItem;  // The root item of the skills, used to display in tree models

    /**
     * @brief Parses the JSON object to get the correct availability.
     * @param p_resourceDef     The resource definition to fill.
     * @param p_currentResource The JSON object that has the availability information.
     */
    void fillAvailability(ResourceDefinition* p_resourceDef, QJsonObject* p_currentResource);

    /**
     * @brief Parses the JSON object to get the correct cost.
     * @param p_resourceDef     The resource definition to fill.
     * @param p_currentResource The JSON object that has the cost information.
     */
    void fillCost(ResourceDefinition* p_resourceDef, QJsonObject* p_currentResource);

    /**
     * @brief Parses the JSON object to fill the weapon specific fields.
     * @param p_resourceDef     The weapon definition to fill.
     * @param p_currentResource The JSON object that has the information.
     */
    void fillWeaponDefinition(WeaponDefinition* p_weaponDef, QJsonObject* p_currentResource);
};


//---------------------------------------------------------------------------------
inline
ResourceDefinition*
ResourceRules::getModelRootItem()
{
    return _rootItem;
}

//---------------------------------------------------------------------------------
inline
const QMap<QString, ResourceDefinition*>&
ResourceRules::getAllDefinitions() const
{
    return _definitions;
}

//---------------------------------------------------------------------------------
inline
const ResourceDefinition&
ResourceRules::getDefinition(const QString& p_uniqueId) const
{
    return *(_definitions[p_uniqueId]);
}

#endif // RESOURCERULES_H
