#include "qualitytreemodel.h"

#include "rules/rules.h"

//---------------------------------------------------------------------------------
QualityTreeModel::QualityTreeModel()
{
}

//---------------------------------------------------------------------------------
QualityTreeModel::~QualityTreeModel()
{
}

//---------------------------------------------------------------------------------
void
QualityTreeModel::initialize()
{
    _rootItem = QUALITY_RULES->getModelRootItem();
}

//---------------------------------------------------------------------------------
QVariant
QualityTreeModel::data(const QModelIndex& p_index, int p_role) const
{
    // Validity checks
    if (!p_index.isValid())
        return QVariant();

    if (p_role != Qt::DisplayRole)
        return QVariant();

    QualityDefinition* item = static_cast<QualityDefinition*>(p_index.internalPointer());

    QVariant result;
    result.setValue((void*)item);
    return result;
}

//---------------------------------------------------------------------------------
Qt::ItemFlags
QualityTreeModel::flags(const QModelIndex& p_index) const
{
    // Validity checks
    if (!p_index.isValid())
        return 0;

    return QAbstractItemModel::flags(p_index);
}

//---------------------------------------------------------------------------------
QVariant
QualityTreeModel::headerData(int p_section, Qt::Orientation p_orientation, int p_role) const
{
    if (p_role != Qt::DisplayRole)
        return QVariant();

    if (p_orientation == Qt::Horizontal)
        return QVariant("Name");

    return QVariant();
}

//---------------------------------------------------------------------------------
QModelIndex
QualityTreeModel::index(int p_row, int p_column, const QModelIndex& p_parent) const
{
    // Validity checks
    if (p_parent.isValid() && p_parent.column() != 0)
            return QModelIndex();

    // Get parent item
    QualityDefinition* parentItem;
    if (!p_parent.isValid())
        parentItem = _rootItem;
    else
        parentItem = static_cast<QualityDefinition*>(p_parent.internalPointer());

    // Get correct child item and create model index
    QualityDefinition* childItem = parentItem->children[p_row];
    if (childItem)
        return createIndex(p_row, 0, childItem);
    else
        return QModelIndex();
}

//---------------------------------------------------------------------------------
QModelIndex
QualityTreeModel::parent(const QModelIndex& p_index) const
{
    // Validity checks
    if (!p_index.isValid())
        return QModelIndex();

    // Get child and parent items
    QualityDefinition* childItem = static_cast<QualityDefinition*>(p_index.internalPointer());
    QualityDefinition* parentItem = childItem->parent;

    if (parentItem == _rootItem)
        return QModelIndex();

    // Create model index
    // Use continuity of vector data to get index
    return createIndex(parentItem->children.size(), 0, parentItem);
}

//---------------------------------------------------------------------------------
int
QualityTreeModel::rowCount(const QModelIndex& p_parent) const
{
    // Validity checks
    if (p_parent.column() > 0)
        return 0;

    // Get parent item
    QualityDefinition* parentItem;
    if (!p_parent.isValid())
        parentItem = _rootItem;
    else
        parentItem = static_cast<QualityDefinition*>(p_parent.internalPointer());

    // Return size of children
    return parentItem->children.size();
}

//---------------------------------------------------------------------------------
int
QualityTreeModel::columnCount(const QModelIndex& p_parent) const
{
    // TODO: This will probably be more soon as we want to show more than just the name
    return 1;
//    if (p_parent.isValid())
//        return static_cast<SkillDefinition*>(p_parent.internalPointer())->columnCount();
//    else
//        return _rootItem->columnCount();
}
