#include "resourcesortfilterproxymodel.h"

#include <iostream>

#include "resourcetreemodel.h"
#include "rules/rules.h"

//---------------------------------------------------------------------------------
ResourceSortFilterProxyModel::ResourceSortFilterProxyModel(QObject* p_parent)
    : QSortFilterProxyModel(p_parent)
    , _showEmptyCategories(true)
    , _showUserResources(false)
    , _filterMask(0)
{

}

//---------------------------------------------------------------------------------
ResourceSortFilterProxyModel::~ResourceSortFilterProxyModel()
{

}

//---------------------------------------------------------------------------------
void
ResourceSortFilterProxyModel::update()
{
    emit dataChanged(index(0,0), index(rowCount() - 1, 0));
}

//---------------------------------------------------------------------------------
int
ResourceSortFilterProxyModel::columnCount(const QModelIndex& p_parent ) const
{
   return sourceModel()->columnCount(p_parent);
}

//---------------------------------------------------------------------------------
void
ResourceSortFilterProxyModel::applyFilter()
{
    // Okay... what happened to good old reset()?
    beginResetModel();
    endResetModel();
}

//---------------------------------------------------------------------------------
bool
ResourceSortFilterProxyModel::lessThan(const QModelIndex& p_left, const QModelIndex& p_right) const
{
    ResourceDefinition* leftItem = static_cast<ResourceDefinition*>(p_left.internalPointer());
    ResourceDefinition* rightItem = static_cast<ResourceDefinition*>(p_right.internalPointer());
    // Sorting depends on exact item type

    // Categories are sorted by type

    // Actual items are sorted alphabetically
    QString leftData = leftItem->id;
    QString rightData = rightItem->id;

    return leftData > rightData;
}

//---------------------------------------------------------------------------------
bool
ResourceSortFilterProxyModel::filterAcceptsRow(int p_row, const QModelIndex& p_parent) const
{
    QModelIndex currentIndex = sourceModel()->index(p_row, 0, p_parent);
    ResourceDefinition* currentItem = static_cast<ResourceDefinition*>(currentIndex.internalPointer());

    // Check the item
    bool accept = filterAcceptsItem(currentItem);

    // We may hide a category if it is empty
    if (accept && currentItem->isCategory && !_showEmptyCategories)
    {
        // Iterate over the children to find out if any will be accepted
        // And if so, accept the category
        int numChildren = currentItem->children.size();
        accept = false;
        for (int i = 0; i < numChildren; ++i)
        {
            ResourceDefinition* item = currentItem->children[i];
            if (filterAcceptsItem(item))
            {
                accept = true;
                break;
            }
        }
    }

    // Return result
    return accept;
}

//---------------------------------------------------------------------------------
bool
ResourceSortFilterProxyModel::filterAcceptsItem(ResourceDefinition* p_item) const
{
    bool accept = true;

    // Categories are accepted by default
    if (p_item->isCategory)
    {
        return true;
    }

    // User defined Resources may or may not be shown
    if (p_item->isUserDefined && !_showUserResources)
    {
        return false;
    }

    // Accept items whose ID contains a substring
    if (!p_item->isCategory &&
        _filterMask & RESOURCE_FILTERMASK_ID_CONTAINS)
    {
        accept = false;
        int numFilters = _filterIDContains.size();
        for (int i = 0; i < numFilters; ++i)
        {
            if (p_item->id.contains(_filterIDContains[i]))
            {
                accept = true;
                break;
            }
        }
    }

    // Accept items whose ID is equal to one in our list
    if (accept &&
        !p_item->isCategory &&
        _filterMask & RESOURCE_FILTERMASK_ID_EQUALS)
    {
        accept = false;
        int numFilters = _filterIDEquals.size();
        for (int i = 0; i < numFilters; ++i)
        {
            if (p_item->id == _filterIDEquals[i])
            {
                accept = true;
                break;
            }
        }
    }

    // Accept items that fit one of the filter types
    if (accept &&
        !p_item->isCategory &&
        _filterMask & RESOURCE_FILTERMASK_TYPE)
    {
        accept = false;
        int numFilters = _filterTypes.size();
        for (int i = 0; i < numFilters; ++i)
        {
            if (p_item->type == _filterTypes[i])
            {
                accept = true;
                break;
            }
        }
    }

    return accept;
}
