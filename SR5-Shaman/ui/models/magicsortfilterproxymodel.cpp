#include "magicsortfilterproxymodel.h"

#include <iostream>

#include "rules/rules.h"

//---------------------------------------------------------------------------------
MagicSortFilterProxyModel::MagicSortFilterProxyModel(QObject* p_parent)
    : QSortFilterProxyModel(p_parent)
    , _showEmptyCategories(true)
    , _filterMask(0)
{

}

//---------------------------------------------------------------------------------
MagicSortFilterProxyModel::~MagicSortFilterProxyModel()
{

}

//---------------------------------------------------------------------------------
int
MagicSortFilterProxyModel::columnCount(const QModelIndex& p_parent ) const
{
   return sourceModel()->columnCount(p_parent);
}

//---------------------------------------------------------------------------------
void
MagicSortFilterProxyModel::applyFilter()
{
    // Okay... what happened to good old reset()?
    beginResetModel();
    endResetModel();
}

//---------------------------------------------------------------------------------
bool
MagicSortFilterProxyModel::lessThan(const QModelIndex& p_left, const QModelIndex& p_right) const
{
    // Sort by alphabet
    QString leftData = sourceModel()->data(p_left).toString();
    QString rightData = sourceModel()->data(p_right).toString();

    return leftData > rightData;
}

//---------------------------------------------------------------------------------
bool
MagicSortFilterProxyModel::filterAcceptsRow(int p_row, const QModelIndex& p_parent) const
{
    QModelIndex currentIndex = sourceModel()->index(p_row, 0, p_parent);
    MagicModelItem* currentItem = static_cast<MagicModelItem*>(currentIndex.internalPointer());

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
            MagicModelItem* item = currentItem->children[i];
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
MagicSortFilterProxyModel::filterAcceptsItem(MagicModelItem* p_item) const
{
    bool accept = true;

    // Categories are accepted by default
    if (p_item->isCategory)
    {
        return true;
    }

    // Accept items whose ID contains a substring
    if (!p_item->isCategory &&
        _filterMask & MAGIC_FILTERMASK_ID_CONTAINS)
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
        _filterMask & MAGIC_FILTERMASK_ID_EQUALS)
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
        _filterMask & MAGIC_FILTERMASK_TYPE)
    {
        accept = false;
        int numFilters = _filterTypes.size();
        for (int i = 0; i < numFilters; ++i)
        {
            if (p_item->itemType == _filterTypes[i])
            {
                accept = true;
                break;
            }
        }
    }

    return accept;
}
