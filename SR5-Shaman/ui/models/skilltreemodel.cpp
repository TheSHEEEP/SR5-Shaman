#include "skilltreemodel.h"

//---------------------------------------------------------------------------------
SkillTreeModel::SkillTreeModel()
{
    // Construct root item
    _rootItem = new SkillModelItem();
    _rootItem->id = "DEFINITION";
    _rootItem->parent = NULL;
}

//---------------------------------------------------------------------------------
SkillTreeModel::~SkillTreeModel()
{
    delete _rootItem;
}

//---------------------------------------------------------------------------------
void
SkillTreeModel::initialize()
{
    // Get the skills from the rules
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

    SkillModelItem* item = static_cast<SkillModelItem*>(p_index.internalPointer());

    return QVariant(item->id);
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
    if (p_orientation == Qt::Horizontal && p_section == Qt::DisplayRole)
        return QVariant("Wat");

    return QVariant();
}

//---------------------------------------------------------------------------------
QModelIndex
SkillTreeModel::index(int p_row, int p_column, const QModelIndex& p_parent) const
{
    // Validity checks
    if (!hasIndex(p_row, p_column, p_parent))
        return QModelIndex();

    // Get parent item
    SkillModelItem* parentItem;
    if (!p_parent.isValid())
        parentItem = _rootItem;
    else
        parentItem = static_cast<SkillModelItem*>(p_parent.internalPointer());

    // Get correct child item and create model index
    SkillModelItem* childItem = parentItem->children[p_row];
    if (childItem)
        return createIndex(p_row, p_column, childItem);
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
    SkillModelItem* childItem = static_cast<SkillModelItem*>(p_index.internalPointer());
    SkillModelItem* parentItem = childItem->parent;

    if (parentItem == _rootItem)
        return QModelIndex();

    // Create model index
    // Use continuity of vector data to get index
    int row = std::distance(parentItem->parent->children[0], parentItem);
    return createIndex(row, 0, parentItem);
}

//---------------------------------------------------------------------------------
int
SkillTreeModel::rowCount(const QModelIndex& p_parent) const
{
    // Validity checks
    if (p_parent.column() > 0)
        return 0;

    // Get parent item
    SkillModelItem* parentItem;
    if (!p_parent.isValid())
        parentItem = _rootItem;
    else
        parentItem = static_cast<SkillModelItem*>(p_parent.internalPointer());

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
//        return static_cast<SkillModelItem*>(p_parent.internalPointer())->columnCount();
//    else
//        return _rootItem->columnCount();
}
