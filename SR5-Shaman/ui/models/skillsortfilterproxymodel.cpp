#include "skillsortfilterproxymodel.h"

#include "skilltreemodel.h"
#include "rules/skillrules.h"

//---------------------------------------------------------------------------------
SkillSortFilterProxyModel::SkillSortFilterProxyModel(QObject* p_parent)
    :QSortFilterProxyModel(p_parent)
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
SkillSortFilterProxyModel::filterAcceptsRow(int p_sourceRow, const QModelIndex& p_sourceParent) const
{
    return true;
}

