#include "skillsortfilterproxymodel.h"

#include <iostream>

#include "skilltreemodel.h"
#include "rules/rules.h"

//---------------------------------------------------------------------------------
SkillSortFilterProxyModel::SkillSortFilterProxyModel(QObject* p_parent)
    : QSortFilterProxyModel(p_parent)
    , _showEmptyCategories(true)
    , _showSkillGroups(true)
    , _showNormalSkills(true)
    , _showUserSkills(false)
    , _filterMask(0)
{

}

//---------------------------------------------------------------------------------
SkillSortFilterProxyModel::~SkillSortFilterProxyModel()
{

}

//---------------------------------------------------------------------------------
void
SkillSortFilterProxyModel::update()
{
    emit dataChanged(index(0,0), index(rowCount() - 1, 0));
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
    SkillDefinition* leftItem = static_cast<SkillDefinition*>(p_left.internalPointer());
    SkillDefinition* rightItem = static_cast<SkillDefinition*>(p_right.internalPointer());
    QString leftData = leftItem->id;
    QString rightData = rightItem->id;

    // Check for skill group item, it must always be last
    if (p_left.isValid())
    {
        SkillDefinition* item = (SkillDefinition*)p_left.internalPointer();
        if (item->type == NUM_SKILL_TYPES)
        {
            return true;
        }
    }
    if (p_right.isValid())
    {
        SkillDefinition* item = (SkillDefinition*)p_right.internalPointer();
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
    SkillDefinition* currentItem = static_cast<SkillDefinition*>(currentIndex.internalPointer());

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
            SkillDefinition* item = currentItem->children[i];
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
SkillSortFilterProxyModel::filterAcceptsItem(SkillDefinition* p_item) const
{
    bool accept = true;

    // Categories are accepted by default
    if (p_item->isCategory)
    {
        return true;
    }

    // User defined skills may or may not be shown
    if (p_item->isUserDefined && !_showUserSkills)
    {
        return false;
    }

    // Groups may or may not be shown
    if (p_item->isGroup && !_showSkillGroups)
    {
        return false;
    }

    // Non-skill-groups may or may not be shown
    if (!p_item->isGroup &&
        (p_item->parent == NULL || (!p_item->parent->isCategory && !p_item->isGroup)) &&
        !_showNormalSkills)
    {
        return false;
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

        // If this item is part of an accepted group, add it automatically
        // Even if it would not normally be accepted
        if (p_item->parent && !p_item->parent->isCategory &&
            p_item->parent->isGroup)
        {
            for (int i = 0; i < numFilters; ++i)
            {
                if (p_item->parent->id.contains(_filterIDContains[i]))
                {
                    accept = true;
                    break;
                }
            }
        }
    }

    // Accept items whose ID is equal to one in our list
    if (accept &&
        !p_item->isCategory &&
        _filterMask & SKILL_FILTERMASK_ID_EQUALS)
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

        // If this item is part of an accepted group, add it automatically
        // Even if it would not normally be accepted
        if (p_item->parent && !p_item->parent->isCategory &&
            p_item->parent->isGroup)
        {
            for (int i = 0; i < numFilters; ++i)
            {
                if (p_item->parent->id == _filterIDEquals[i])
                {
                    accept = true;
                    break;
                }
            }
        }
    }

    // Accept items that fit one of the filter types
    if (accept &&
        !p_item->isCategory &&
        _filterMask & SKILL_FILTERMASK_TYPE)
    {
        accept = false;
        int numFilters = _filterTypes.size();
        if (!SKILL_RULES->getDefinition(p_item->id).isGroup)
        {
            for (int i = 0; i < numFilters; ++i)
            {
                if (p_item->type == _filterTypes[i])
                {
                    accept = true;
                    break;
                }
            }

            // If this item is part of an accepted group, accept it automatically
            // Even if it would not normally be accepted
            if (p_item->parent && !p_item->parent->isCategory &&
                p_item->parent->isGroup)
            {
                for (int i = 0; i < numFilters; ++i)
                {
                    if (p_item->parent->type == _filterTypes[i])
                    {
                        accept = true;
                        break;
                    }
                }
            }
        }
        else
        {
            // A group is only of valid type if all its group skills are of a valid type
            unsigned int valid = 0;
            for (unsigned int i = 0; i < p_item->children.size(); ++i)
            {
                for (int j = 0; j < numFilters; ++j)
                {
                    if (p_item->children[i]->type == _filterTypes[j])
                    {
                        valid++;
                        break;
                    }
                }
            }
            if (valid == p_item->children.size())
            {
                accept = true;
            }
        }
    }

    return accept;
}

