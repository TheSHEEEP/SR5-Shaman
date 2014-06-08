#ifndef RESOURCERULES_H
#define RESOURCERULES_H

#include <QString>
#include <QMap>
#include <vector>
#include <QPair>
#include <QObject>

// Resource categories - THE ORDER IS OMPORTANT
enum ResourceType
{
    RESOURCE_TYPE_INVALID = -1,
    RESOURCE_TYPE_MELEE_WEAPON,
    RESOURCE_TYPE_PROJECTILE_WEAPON,
    RESOURCE_TYPE_FIREARM,
    RESOURCE_TYPE_AMMUNITION,
    RESOURCE_TYPE_EXPLOSIVES,
    RESOURCE_TYPE_CLOTHING_ARMOR,
    RESOURCE_TYPE_ELECTRONICS,
    RESOURCE_TYPE_INDUSTRIAL_CHEMICALS,
    RESOURCE_TYPE_SURVIVAL_GEAR,
    RESOURCE_TYPE_GRAPPLE_FLARE,
    RESOURCE_TYPE_BIOTECH,
    RESOURCE_TYPE_DOCWAGON,
    RESOURCE_TYPE_AUGMENTATION,
    RESOURCE_TYPE_MAGICAL_EQUIPMENT,
    RESOURCE_TYPE_VEHICLES_DRONES,
    NUM_RESOURCE_TYPES
};

/**
 * @brief This is the definition of a single skill.
 *          Also has some additional functionality to serve as a model item for Views.
 */
class ResourceDefinition
{
public:
    /**
     * @brief Constructor.
     */
    ResourceDefinition(ResourceDefinition* p_parent = NULL);

    /**
     * @brief Copy constructor.
     */
    ResourceDefinition(const ResourceDefinition& p_other);

    /**
     * @brief Destructor.
     */
    ~ResourceDefinition();

    /**
     * @brief Returns true if this item has a child with the passed value.
     * @param p_id  The ID to look for.
     */
    bool hasChild(const QString& p_id) const;

    /**
     * @brief Returns the child with the passed value. Or NULL, if no child was found.
     */
    ResourceDefinition* getChild(const QString& p_id) const;

    // Those two seem to be redundant, but are useful when this definition is used as
    // a model item inside a View
    ResourceDefinition*                parent;
    std::vector<ResourceDefinition*>   children;

    QString                         id;
    bool                            isCategory;
    bool                            isUserDefined;
    bool                            requiresCustom;
    QString                         custom;     // This can be used for further definition, like the name defined by the user
    ResourceType                    type;
    QMap<QString, QString>          translations;
};

Q_DECLARE_METATYPE(ResourceDefinition)
Q_DECLARE_METATYPE(ResourceDefinition*)

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
    std::vector<QPair<int, int>>        _skillPoints;

    ResourceDefinition*    _rootItem;  // The root item of the skills, used to display in tree models
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
