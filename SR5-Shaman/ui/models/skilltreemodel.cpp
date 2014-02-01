#include "skilltreemodel.h"

#include "rules/rules.h"
#include "data/appstatus.h"

//---------------------------------------------------------------------------------
SkillTreeModel::SkillTreeModel()
{

}

//---------------------------------------------------------------------------------
SkillTreeModel::~SkillTreeModel()
{
    // Children will be deleted recursively
    delete _rootItem;
}

//---------------------------------------------------------------------------------
void
SkillTreeModel::initialize()
{
    _rootItem = SKILL_RULES->getModelRootItem();
}

//---------------------------------------------------------------------------------
QVariant
SkillTreeModel::data(const QModelIndex& p_index, int p_role) const
{
    // Validity checks
    if (!p_index.isValid())
        return QVariant();

    if (p_role != Qt::DisplayRole)
        return QVariant();

    SkillDefinition* item = static_cast<SkillDefinition*>(p_index.internalPointer());

    QVariant result;
    result.setValue((void*)item);
    return result;
}

//---------------------------------------------------------------------------------
Qt::ItemFlags
SkillTreeModel::flags(const QModelIndex& p_index) const
{
    // Validity checks
    if (!p_index.isValid())
        return 0;

    return QAbstractItemModel::flags(p_index);
}

//---------------------------------------------------------------------------------
QVariant
SkillTreeModel::headerData(int p_section, Qt::Orientation p_orientation, int p_role) const
{
    if (p_role != Qt::DisplayRole)
        return QVariant();

    if (p_orientation == Qt::Horizontal)
        return QVariant("Name");

    return QVariant();
}

//---------------------------------------------------------------------------------
QModelIndex
SkillTreeModel::index(int p_row, int p_column, const QModelIndex& p_parent) const
{
    // Validity checks
    if (p_parent.isValid() && p_parent.column() != 0)
            return QModelIndex();

    // Get parent item
    SkillDefinition* parentItem;
    if (!p_parent.isValid())
        parentItem = _rootItem;
    else
        parentItem = static_cast<SkillDefinition*>(p_parent.internalPointer());

    // Get correct child item and create model index
    SkillDefinition* childItem = parentItem->children[p_row];
    if (childItem)
        return createIndex(p_row, 0, childItem);
    else
        return QModelIndex();
}

//---------------------------------------------------------------------------------
QModelIndex
SkillTreeModel::parent(const QModelIndex& p_index) const
{
    // Validity checks
    if (!p_index.isValid())
        return QModelIndex();

    // Get child and parent items
    SkillDefinition* childItem = static_cast<SkillDefinition*>(p_index.internalPointer());
    SkillDefinition* parentItem = childItem->parent;

    if (parentItem == _rootItem)
        return QModelIndex();

    // Create model index
    // Use continuity of vector data to get index
    return createIndex(parentItem->children.size(), 0, parentItem);
}

//---------------------------------------------------------------------------------
int
SkillTreeModel::rowCount(const QModelIndex& p_parent) const
{
    // Validity checks
    if (p_parent.column() > 0)
        return 0;

    // Get parent item
    SkillDefinition* parentItem;
    if (!p_parent.isValid())
        parentItem = _rootItem;
    else
        parentItem = static_cast<SkillDefinition*>(p_parent.internalPointer());

    // Return size of children
    return parentItem->children.size();
}

//---------------------------------------------------------------------------------
int
SkillTreeModel::columnCount(const QModelIndex& p_parent) const
{
    // TODO: This will probably be more as soon as we want to show more than just the name
    return 1;
//    if (p_parent.isValid())
//        return static_cast<SkillDefinition*>(p_parent.internalPointer())->columnCount();
//    else
//        return _rootItem->columnCount();
}
