#include "magictreemodel.h"

#include "rules/rules.h"
#include "data/appstatus.h"

//---------------------------------------------------------------------------------
MagicTreeModel::MagicTreeModel()
{

}

//---------------------------------------------------------------------------------
MagicTreeModel::~MagicTreeModel()
{

}

//---------------------------------------------------------------------------------
void
MagicTreeModel::initialize()
{
    _rootItem = MAGIC_RULES->getModelRootItem();
}

//---------------------------------------------------------------------------------
QVariant
MagicTreeModel::data(const QModelIndex& p_index, int p_role) const
{
    // Validity checks
    if (!p_index.isValid())
        return QVariant();

    if (p_role != Qt::DisplayRole)
        return QVariant();

    MagicAbilityDefinition* item = static_cast<MagicAbilityDefinition*>(p_index.internalPointer());

    QVariant result;
    result.setValue((void*)item);
    return result;
}

//---------------------------------------------------------------------------------
Qt::ItemFlags
MagicTreeModel::flags(const QModelIndex& p_index) const
{
    // Validity checks
    if (!p_index.isValid())
        return 0;

    return QAbstractItemModel::flags(p_index);
}

//---------------------------------------------------------------------------------
QVariant
MagicTreeModel::headerData(int p_section, Qt::Orientation p_orientation, int p_role) const
{
    if (p_role != Qt::DisplayRole)
        return QVariant();

    if (p_orientation == Qt::Horizontal)
        return QVariant("Name");

    return QVariant();
}

//---------------------------------------------------------------------------------
QModelIndex
MagicTreeModel::index(int p_row, int p_column, const QModelIndex& p_parent) const
{
    // Validity checks
    if (p_parent.isValid() && p_parent.column() != 0)
            return QModelIndex();

    // Get parent item
    MagicAbilityDefinition* parentItem;
    if (!p_parent.isValid())
        parentItem = _rootItem;
    else
        parentItem = static_cast<MagicAbilityDefinition*>(p_parent.internalPointer());

    // Get correct child item and create model index
    MagicAbilityDefinition* childItem = parentItem->children[p_row];
    if (childItem)
        return createIndex(p_row, 0, childItem);
    else
        return QModelIndex();
}

//---------------------------------------------------------------------------------
QModelIndex
MagicTreeModel::parent(const QModelIndex& p_index) const
{
    // Validity checks
    if (!p_index.isValid())
        return QModelIndex();

    // Get child and parent items
    MagicAbilityDefinition* childItem = static_cast<MagicAbilityDefinition*>(p_index.internalPointer());
    MagicAbilityDefinition* parentItem = childItem->parent;

    if (parentItem == _rootItem)
        return QModelIndex();

    // Create model index
    // Use continuity of vector data to get index
    return createIndex(parentItem->children.size(), 0, parentItem);
}

//---------------------------------------------------------------------------------
int
MagicTreeModel::rowCount(const QModelIndex& p_parent) const
{
    // Validity checks
    if (p_parent.column() > 0)
        return 0;

    // Get parent item
    MagicAbilityDefinition* parentItem;
    if (!p_parent.isValid())
        parentItem = _rootItem;
    else
        parentItem = static_cast<MagicAbilityDefinition*>(p_parent.internalPointer());

    // Return size of children
    return parentItem->children.size();
}

//---------------------------------------------------------------------------------
int
MagicTreeModel::columnCount(const QModelIndex& p_parent) const
{
    // TODO: This will probably be more as soon as we want to show more than just the name
    return 1;
//    if (p_parent.isValid())
//        return static_cast<SkillDefinition*>(p_parent.internalPointer())->columnCount();
//    else
//        return _rootItem->columnCount();
}
