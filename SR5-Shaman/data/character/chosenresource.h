#ifndef CHOSENRESOURCE_H
#define CHOSENRESOURCE_H

#include <QString>
#include <vector>

/**
 * @brief This is a single resource selected by the user.
 *          Properties are implemented as getters/setters as they might require additional
 *          functionality later on.
 */
class ChosenResource
{
public:
    /**
     * @brief Constructor.
     * @param p_originalID  The ID of the original resource item in the rules.
     */
    ChosenResource(const QString& p_originalID);

    /**
     * @brief Destructor.
     */
    ~ChosenResource();

    /**
     * @brief Returns the original ID of the item in the rules.
     */
    const QString& getOriginalID() const;

    /**
     * @brief Sets the custom name of the chosen resource.
     */
    void setCustomName(const QString& p_name);

    /**
     * @brief Returns the custom name of the chosen resource.
     */
    const QString& getCustomName() const;

    /**
     * @brief Adds the passed resource as a mount.
     */
    void addMount(ChosenResource* p_mount);

    /**
     * @brief Removes the passed mount.
     * @note    Does NOT delete it.
     * @param p_mount   The mount to look for.
     */
    void removeMount(ChosenResource* p_mount);

    /**
     * @brief Returns a reference to all mounts this chosen resource has.
     */
    std::vector<ChosenResource*>& getMounts();

    /**
     * @brief Set if this resource is wireless or not.
     */
    void setIsWireless(bool p_wireless);

    /**
     * @brief Returns true if this chosen resource is wireless.
     */
    bool getIsWireless() const;

    /**
     * @brief Sets the amount of this chosen resource.
     */
    void setAmount(int p_amount);

    /**
     * @brief Returns the amount of this resource.
     */
    int getAmount() const;

    /**
     * @brief Sets the rating of this resource.
     */
    void setRating(int p_rating);

    /**
     * @brief Returns the rating of this resource. Returns 0 for unrated resources.
     */
    int getRating() const;

private:
    QString                         _originalID;
    QString                         _customName;
    std::vector<ChosenResource*>    _mounts;
    bool                            _wireless;
    int                             _amount;
    signed char                     _rating;
};

//---------------------------------------------------------------------------------
inline
const QString&
ChosenResource::getOriginalID() const
{
    return _originalID;
}

//---------------------------------------------------------------------------------
void
ChosenResource::setCustomName(const QString& p_name)
{
    _customName = p_name;
}

//---------------------------------------------------------------------------------
inline
const QString&
ChosenResource::getCustomName() const
{
    return _customName;
}

//---------------------------------------------------------------------------------
inline
std::vector<ChosenResource*>&
ChosenResource::getMounts()
{
    return _mounts;
}

//---------------------------------------------------------------------------------
inline
void
ChosenResource::setIsWireless(bool p_wireless)
{
    _wireless = p_wireless;
}

//---------------------------------------------------------------------------------
inline
bool
ChosenResource::getIsWireless() const
{
    return _wireless;
}

//---------------------------------------------------------------------------------
inline
void
ChosenResource::setAmount(int p_amount)
{
    _amount = p_amount;
}

//---------------------------------------------------------------------------------
inline
int
ChosenResource::getAmount() const
{
    return _amount;
}

//---------------------------------------------------------------------------------
inline
void
ChosenResource::setRating(int p_rating)
{
    _rating = p_rating;
}

//---------------------------------------------------------------------------------
inline
int
ChosenResource::getRating() const
{
    return _rating;
}

#endif // CHOSENRESOURCE_H
