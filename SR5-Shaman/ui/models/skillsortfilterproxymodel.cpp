#include "skillsortfilterproxymodel.h"

#include "skilltreemodel.h"
#include "rules/skillrules.h"

//---------------------------------------------------------------------------------
SkillSortFilterProxyModel::SkillSortFilterProxyModel(QObject* p_parent)
    : QSortFilterProxyModel(p_parent)
    , _showEmptyCategories(true)
    , _filterMask(0)
{

}

//---------------------------------------------------------------------------------
SkillSortFilterProxyModel::~SkillSortFilterProxyModel()
{

}

//---------------------------------------------------------------------------------
int
SkillSortFilterProxyModel::columnCount(const QModelIndex& p_parent ) const
{
   return sourceModel()->columnCount(p_parent);
}

//---------------------------------------------------------------------------------
void
SkillSortFilterProxyModel::applyFilter()
{
    // Okay... what happened to good old reset()?
    beginResetModel();
    endResetModel();
}

//---------------------------------------------------------------------------------
bool
SkillSortFilterProxyModel::lessThan(const QModelIndex& p_left, const QModelIndex& p_right) const
{
    // Sort by alphabet
    QString leftData = sourceModel()->data(p_left).toString();
    QString rightData = sourceModel()->data(p_right).toString();

    // Check for skill group item, it must always be last
    if (p_left.isValid())
    {
        SkillModelItem* item = (SkillModelItem*)p_left.internalPointer();
        if (item->type == NUM_SKILL_TYPES)
        {
            return true;
        }
    }
    if (p_right.isValid())
    {
        SkillModelItem* item = (SkillModelItem*)p_right.internalPointer();
        if (item->type == NUM_SKILL_TYPES)
        {
            return false;
        }
    }
    return leftData > rightData;
}

//---------------------------------------------------------------------------------
bool
SkillSortFilterProxyModel::filterAcceptsRow(int p_row, const QModelIndex& p_parent) const
{
    QModelIndex currentIndex = sourceModel()->index(p_row, 0, p_parent);
    SkillModelItem* currentItem = static_cast<SkillModelItem*>(currentIndex.internalPointer());

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
            SkillModelItem* item = currentItem->children[i];
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
SkillSortFilterProxyModel::filterAcceptsItem(SkillModelItem* p_item) const
{
    bool accept = true;

    // Categories are accepted by default
    if (p_item->isCategory)
    {
        return true;
    }

    // Accept items whose ID contains a substring
    if (!p_item->isCategory &&
        _filterMask & SKILL_FILTERMASK_ID_CONTAINS)
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

    // Accept items that fit one of the filter types
    if (!p_item->isCategory &&
        _filterMask & SKILL_FILTERMASK_TYPE)
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

