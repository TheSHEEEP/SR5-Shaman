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
    RESOURCE_TYPE_MOUNT,   // This type is meant for every kind of mount, no matter if for weapons, clothing, etc.
    NUM_RESOURCE_TYPES
};

// Resource sub-types, those are simple defines as they represent the index within
// a resource category
#define RESOURCE_SUBTYPE_INVALID                    -1
#define RESOURCE_SUBTYPE_MELEE_BLADES               0
#define RESOURCE_SUBTYPE_MELEE_CLUBS                1
#define RESOURCE_SUBTYPE_MELEE_OTHER                2
#define RESOURCE_SUBTYPE_PROJECTILE_BOWS            0
#define RESOURCE_SUBTYPE_PROJECTILE_CROSSBOWS       1
#define RESOURCE_SUBTYPE_PROJECTILE_THROWING        2
#define RESOURCE_SUBTYPE_FIREARM_TASERS             0
#define RESOURCE_SUBTYPE_FIREARM_HOLDOUTS           1
#define RESOURCE_SUBTYPE_FIREARM_LPISTOLS           2
#define RESOURCE_SUBTYPE_FIREARM_HPISTOLS           3
#define RESOURCE_SUBTYPE_FIREARM_MPISTOLS           4
#define RESOURCE_SUBTYPE_FIREARM_SMGS               5
#define RESOURCE_SUBTYPE_FIREARM_ASSAULTR           6
#define RESOURCE_SUBTYPE_FIREARM_SNIPERR            7
#define RESOURCE_SUBTYPE_FIREARM_SHOTGUNS           8
#define RESOURCE_SUBTYPE_FIREARM_SPECIALW           9
#define RESOURCE_SUBTYPE_FIREARM_MACHINEG           10
#define RESOURCE_SUBTYPE_FIREARM_CANNONS_LAUNCHERS  11
#define RESOURCE_SUBTYPE_AMMU_GRENADES              0
#define RESOURCE_SUBTYPE_AMMU_ROCKETS               1

// Weapon fire modi
enum WeaponFireModus
{
    WEAPON_FIRE_MODUS_INVALID = -1,
    WEAPON_FIRE_MODUS_SS,
    WEAPON_FIRE_MODUS_SA,
    WEAPON_FIRE_MODUS_BF,
    WEAPON_FIRE_MODUS_FA,
    NUM_WEAPON_FIRE_MODI
};
// Weapon clip types
enum WeaponClipType
{
    WEAPON_CLIP_TYPE_INVALID = -1,
    WEAPON_CLIP_TYPE_BREAK,
    WEAPON_CLIP_TYPE_CLIP,
    WEAPON_CLIP_TYPE_DRUM,
    WEAPON_CLIP_TYPE_MUZZLE,
    WEAPON_CLIP_TYPE_INT_MAG,
    WEAPON_CLIP_TYPE_CYLINDER,
    WEAPON_CLIP_TYPE_BELT,
    NUM_WEAPON_CLIP_TYPES
};
// Availability classes
enum AvailabilityClass
{
    AVAILABILITY_CLASS_INVALID = -1,
    AVAILABILITY_CLASS_NORMAL,
    AVAILABILITY_CLASS_RESTRICTED,
    AVAILABILITY_CLASS_FORBIDDEN,
    AVAILABILITY_CLASS_NONE,
    NUM_AVAILABILITY_CLASSES
};

// Information about mounts
struct MountInfo
{
    MountInfo()
        : id(""), location(""), removable(false), rating(0)
    { }
    MountInfo(const MountInfo& p_other)
    {
        id = p_other.id;
        location = p_other.location;
        removable = p_other.removable;
        rating = p_other.rating;
    }

    QString id;
    QString location;
    bool    removable;
    char    rating;
};

/**
 * @brief This is the base definition of a single resource.
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
    unsigned short                  subType;
    QMap<QString, QString>          translations;

    // Common resource data
    QString                     availabilityNum;
    signed char                 availabilityClass;
    bool                        availabilityIsPerRating;
    QString                     cost;
    bool                        costIsPerRating;
    signed char                 maxRating;
    bool                        stacks;
    QStringList                 mounts;
    std::vector<MountInfo>      attachedMounts;
    bool                        wireless;
};

Q_DECLARE_METATYPE(ResourceDefinition)
Q_DECLARE_METATYPE(ResourceDefinition*)

/**
 * @brief This is the definition of a single weapon resource. Also used for ammunition.
 */
class WeaponDefinition : public ResourceDefinition
{
public:
    /**
     * @brief Constructor.
     */
    WeaponDefinition(ResourceDefinition* p_parent = NULL);

    /**
     * @brief Copy constructor.
     */
    WeaponDefinition(const WeaponDefinition& p_other);

    /**
     * @brief Destructor.
     */
    ~WeaponDefinition();

    signed char                     accuracy;
    signed char                     reach;
    QString                         damageNum;
    QString                         damageType;
    signed char                     ap;
    bool                            apIsModding;
    std::vector<WeaponFireModus>    fireModi;
    signed char                     rc;
    bool                            rcIsModding;
    signed short                    clipSize;
    WeaponClipType                  clipType;
    QString                         blast;
};

Q_DECLARE_METATYPE(WeaponDefinition)
Q_DECLARE_METATYPE(WeaponDefinition*)

/**
 * @brief This is the definition of a single mount resource.
 */
class MountDefinition : public ResourceDefinition
{
public:
    /**
     * @brief Constructor.
     */
    MountDefinition(ResourceDefinition* p_parent = NULL);

    /**
     * @brief Copy constructor.
     */
    MountDefinition(const MountDefinition& p_other);

    /**
     * @brief Destructor.
     */
    ~MountDefinition();

    QStringList                     locations;
    QStringList                     validSubtypes;
    QStringList                     validItems;
    bool                            permanent;
    std::vector<ResourceEffect*>    effects;
    bool                            effectsArePerRating;
    bool                            requiresWirelessHost;
    bool                            requiresSmartlinkOnHost;
};

Q_DECLARE_METATYPE(MountDefinition)
Q_DECLARE_METATYPE(MountDefinition*)
