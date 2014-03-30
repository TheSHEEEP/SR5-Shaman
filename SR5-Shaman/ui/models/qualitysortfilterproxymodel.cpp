#include "qualitysortfilterproxymodel.h"

//---------------------------------------------------------------------------------
QualitySortFilterProxyModel::QualitySortFilterProxyModel(QObject* p_parent)
    : QSortFilterProxyModel(p_parent)
    , _showEmptyCategories(true)
    , _filterMask(0)
{

}

//---------------------------------------------------------------------------------
QualitySortFilterProxyModel::~QualitySortFilterProxyModel()
{

}

//---------------------------------------------------------------------------------
int
QualitySortFilterProxyModel::columnCount(const QModelIndex& p_parent ) const
{
   return sourceModel()->columnCount(p_parent);
}

//---------------------------------------------------------------------------------
void
QualitySortFilterProxyModel::applyFilter()
{
    // Okay... what happened to good old reset()?
    beginResetModel();
    endResetModel();
}

//---------------------------------------------------------------------------------
bool
QualitySortFilterProxyModel::lessThan(const QModelIndex& p_left, const QModelIndex& p_right) const
{
    // Sort by alphabet
    QString leftData = sourceModel()->data(p_left).toString();
    QString rightData = sourceModel()->data(p_right).toString();

    return leftData > rightData;
}

//---------------------------------------------------------------------------------
bool
QualitySortFilterProxyModel::filterAcceptsRow(int p_row, const QModelIndex& p_parent) const
{
    QModelIndex currentIndex = sourceModel()->index(p_row, 0, p_parent);
    QualityDefinition* currentItem = static_cast<QualityDefinition*>(currentIndex.internalPointer());

    // Check the item
    bool accept = filterAcceptsItem(currentItem);

    // We may hide a category if it is empty
    if (accept && !_showEmptyCategories &&
            (currentItem->isCategory ||
             (currentItem->abilityType == MAGICABILITYTYPE_SPELL && currentItem->spell->isSpellCategory)) )
    {
        int numChildren = currentItem->children.size();
        accept = false;

        // If this is the spell category, only show it if any of it's children have any valid children themselves
        // TODO: Is there a quicker way to do this?
        if (currentItem->id == "CATEGORY_SPELLS")
        {
            for (int i = 0; i < numChildren; ++i)
            {
                MagicAbilityDefinition* item = currentItem->children[i];
                int numInnerChildren = item->children.size();
                for (int j = 0; j < numInnerChildren; ++j)
                {
                    if (filterAcceptsItem(item->children[j]))
                    {
                        accept = true;
                        break;
                    }
                }
                if (accept) break;
            }
        }
        else
        {
            // Iterate over the children to find out if any will be accepted
            // And if so, accept the category
            for (int i = 0; i < numChildren; ++i)
            {
                MagicAbilityDefinition* item = currentItem->children[i];
                if (filterAcceptsItem(item))
                {
                    accept = true;
                    break;
                }
            }
        }
    }

    // Return result
    return accept;
}

//---------------------------------------------------------------------------------
bool
QualitySortFilterProxyModel::filterAcceptsItem(QualityDefinition* p_item) const
{
    bool accept = true;

    // Categories are accepted by default
    if (p_item->isCategory ||
        (p_item->abilityType == MAGICABILITYTYPE_SPELL && p_item->spell->isSpellCategory))
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
            if (p_item->abilityType == _filterTypes[i])
            {
                accept = true;
                break;
            }
        }
    }

    return accept;
}
