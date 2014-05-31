#ifndef RESOURCESORTFILTERPROXYMODEL_H
#define RESOURCESORTFILTERPROXYMODEL_H

#include <QSortFilterProxyModel>
#include <QStringList>

#include "rules/resourcerules.h"

#define RESOURCE_FILTERMASK_ID_CONTAINS    1  // Use the filterIDContains (getFilterIDContains) for filtering
#define RESOURCE_FILTERMASK_ID_EQUALS      2  // Use the filterIDEquals (getFilterIDEquals) for filtering
#define RESOURCE_FILTERMASK_TYPE           4  // Use the filterTypes (getFilterTypes) for filtering

class ResourceDefinition;

class ResourceSortFilterProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT

public:
    /**
     * @brief Constructor.
     */
    ResourceSortFilterProxyModel(QObject* p_parent = 0);

    /**
     * @brief Destructor.
     */
    virtual ~ResourceSortFilterProxyModel();

    /**
     * @brief Emits the data changed signal to update any view.
     */
    void update();

    /**
     * @brief Has to be overridden in order not to screw up the display, it seems.
     *          See here: http://schneide.wordpress.com/2010/04/26/wrestling-with-qts-model-view-api-filtering-in-treemodels/
     */
    int columnCount(const QModelIndex& p_parent ) const;

    /**
     * @brief If the filter will show empty categories.
     */
    void setShowEmptyCategories(bool p_show);

    /**
     * @brief If this is true, user defined resources will be shown.
     */
    void setShowUserResources(bool p_userResources);

    /**
     * @brief Sets the filter mask to use.
     *          Use defines like RESOURCE_FILTERMASK_ID_CONTAINS here to set how the filtering behaves.
     */
    void setFilterMask(int p_mask);

    /**
     * @brief This filter will only show items whose ID contains an entry in the passed QStringList.
     * @note    The ID does not have to be equal with an entry, but only contain an entry as a substring.
     */
    void setFilterIDContains(const QStringList& p_filterIDs);

    /**
     * @brief Returns the contains filter for IDs.
     */
    QStringList& getFilterIDContains();

    /**
     * @brief This filter will only show items whose ID is equal to an entry in the passed QStringList.
     */
    void setFilterIDEquals(const QStringList& p_filterIDs);

    /**
     * @brief Returns the equals filter for IDs.
     */
    QStringList& getFilterIDEquals();

    /**
     * @brief This filter will only show items whose type is one of those in the passed vector.
     */
    void setFilterTypes(const std::vector<ResourceType>& p_filterTypes);

    /**
     * @brief Returns the filter for types.
     */
    std::vector<ResourceType>& getFilterTypes();

    /**
     * @brief Applies the filtering, updating all views using this.
     */
    void applyFilter();

protected:
    /**
     * @brief Will filter the rows depending on the conditions set in this instance.
     * @param p_sourceRow       The source row to check.
     * @param p_sourceParent    The source parent to check.
     * @return True if the row is accepted.
     */
    virtual bool filterAcceptsRow(int p_row, const QModelIndex& p_parent) const;

    /**
     * @brief Will return true if the left model index is less than the right one.
     *          This implementation uses alphabetic order.
     */
    virtual bool lessThan(const QModelIndex& p_left, const QModelIndex& p_right) const;

private:
    bool        _showEmptyCategories;
    bool        _showUserResources;
    int         _filterMask;

    QStringList                 _filterIDContains;
    QStringList                 _filterIDEquals;
    std::vector<ResourceType>   _filterTypes;

    /**
     * @brief Checks the item according to the filters conditions.
     */
    bool filterAcceptsItem(ResourceDefinition* p_item) const;
};

//---------------------------------------------------------------------------------
inline
void
ResourceSortFilterProxyModel::setShowEmptyCategories(bool p_show)
{
    _showEmptyCategories = p_show;
}

//---------------------------------------------------------------------------------
inline
void
ResourceSortFilterProxyModel::setShowUserResources(bool p_userResources)
{
    _showUserResources = p_userResources;
}

//---------------------------------------------------------------------------------
inline
void
ResourceSortFilterProxyModel::setFilterMask(int p_mask)
{
    _filterMask = p_mask;
}

//---------------------------------------------------------------------------------
inline
void
ResourceSortFilterProxyModel::setFilterIDContains(const QStringList& p_filterIDs)
{
    _filterIDContains = p_filterIDs;
}

//---------------------------------------------------------------------------------
inline
QStringList&
ResourceSortFilterProxyModel::getFilterIDContains()
{
    return _filterIDContains;
}

//---------------------------------------------------------------------------------
inline
void
ResourceSortFilterProxyModel::setFilterIDEquals(const QStringList& p_filterIDs)
{
    _filterIDEquals = p_filterIDs;
}

//---------------------------------------------------------------------------------
inline
QStringList&
ResourceSortFilterProxyModel::getFilterIDEquals()
{
    return _filterIDEquals;
}

//---------------------------------------------------------------------------------
inline
void
ResourceSortFilterProxyModel::setFilterTypes(const std::vector<ResourceType>& p_filterTypes)
{
    _filterTypes = p_filterTypes;
}

//---------------------------------------------------------------------------------
inline
std::vector<ResourceType>&
ResourceSortFilterProxyModel::getFilterTypes()
{
    return _filterTypes;
}

#endif // RESOURCESORTFILTERPROXYMODEL_H
