#include "resourcetreemodel.h"

#include <QAbstractItemView>
#include <QSpinBox>

#include "rules/rules.h"
#include "data/appstatus.h"
#include "data/dictionary.h"
#include "data/character/characterchoices.h"

//---------------------------------------------------------------------------------
ResourceTreeModel::ResourceTreeModel(bool p_selectedResourcesMode)
    : _selectedResourcesMode(p_selectedResourcesMode)
    , _itemView(NULL)
{

}

//---------------------------------------------------------------------------------
ResourceTreeModel::~ResourceTreeModel()
{
    // Children will be deleted recursively
    delete _rootItem;
}

//---------------------------------------------------------------------------------
void
ResourceTreeModel::initialize()
{
    // In normal mode, just use the available resources
    if (!_selectedResourcesMode)
    {
        _rootItem = RESOURCE_RULES->getModelRootItem();
    }
    // In selected resource mode, get the resources from player choices
    else
    {
        _rootItem = CHARACTER_CHOICES->getRootResourceSelection();
    }
}

//---------------------------------------------------------------------------------
void
ResourceTreeModel::update()
{
    emit dataChanged(index(0,0), index(rowCount() - 1, 0));
}

//---------------------------------------------------------------------------------
QVariant
ResourceTreeModel::data(const QModelIndex& p_index, int p_role) const
{
    // Each odd table column has a special color
    if (p_role == Qt::BackgroundRole &&
        (p_index.column() % 2) == 1)
    {
        return QVariant(QBrush(APPSTATUS->getHelperColors().tableOddBg));
    }

    // Validity checks
    if (!p_index.isValid())
        return QVariant();

    if (p_role != Qt::DisplayRole)
        return QVariant();

    ResourceDefinition* item = static_cast<ResourceDefinition*>(p_index.internalPointer());

    QVariant result;
    result.setValue((void*)item);
    return result;
}

//---------------------------------------------------------------------------------
Qt::ItemFlags
ResourceTreeModel::flags(const QModelIndex& p_index) const
{
    // Validity checks
    if (!p_index.isValid())
        return 0;

    Qt::ItemFlags flag = QAbstractItemModel::flags(p_index);
    ResourceDefinition* item = static_cast<ResourceDefinition*>(p_index.internalPointer());

    /*// The second and fourth column may be editable
    // If it is NOT a category or child of a group or a native language
    if (p_index.column() == 1 &&
        !item->isCategory &&
        (!item->isLanguage || !CHARACTER_CHOICES->getIsNativeLanguage(item->id)))
    {
        flag |= Qt::ItemIsEditable;
    }
    else if (p_index.column() == 3 &&
             !item->isCategory)
    {
        flag |= Qt::ItemIsEditable;
    }*/
    return flag;
}

//---------------------------------------------------------------------------------
QVariant
ResourceTreeModel::headerData(int p_section, Qt::Orientation p_orientation, int p_role) const
{
    // Alignment
    if (p_role == Qt::TextAlignmentRole)
    {
        switch (p_section)
        {
        case 0:
            return Qt::AlignLeft;
        case 1:
        case 2:
        case 3:
        case 4:
            return Qt::AlignHCenter;
        default:
            return Qt::AlignLeft;
        }
    }

    if (p_role != Qt::DisplayRole)
        return QVariant();

    // Text
    if (p_orientation == Qt::Horizontal)
    {
        switch (p_section)
        {
        case 0:
            return QVariant(Dictionary::getTranslation("VIEW_NAME"));
        case 1:
            return QVariant(Dictionary::getTranslation("VIEW_VALUE"));
        case 2:
            return QVariant(Dictionary::getTranslation("VIEW_ATTRIBUTE"));
        case 3:
            return QVariant(Dictionary::getTranslation("VIEW_SPECIALIZATIONS"));
        case 4:
            return QVariant(Dictionary::getTranslation("VIEW_DICE_POOL"));
        }
    }

    return QVariant();
}

//---------------------------------------------------------------------------------
QModelIndex
ResourceTreeModel::index(int p_row, int p_column, const QModelIndex& p_parent) const
{
    // Validity checks
    if (p_parent.isValid() && p_parent.column() != 0)
            return QModelIndex();

    // Get parent item
    ResourceDefinition* parentItem;
    if (!p_parent.isValid())
        parentItem = _rootItem;
    else
        parentItem = static_cast<ResourceDefinition*>(p_parent.internalPointer());

    // Get correct child item and create model index
    ResourceDefinition* childItem = parentItem->children[p_row];
    if (childItem)
        return createIndex(p_row, p_column, childItem);
    else
        return QModelIndex();
}

//---------------------------------------------------------------------------------
QModelIndex
ResourceTreeModel::parent(const QModelIndex& p_index) const
{
    // Validity checks
    if (!p_index.isValid())
        return QModelIndex();

    // Get child and parent items
    ResourceDefinition* childItem = static_cast<ResourceDefinition*>(p_index.internalPointer());
    ResourceDefinition* parentItem = childItem->parent;

    if (parentItem == _rootItem)
        return QModelIndex();

    // Create model index
    // Use continuity of vector data to get index
    return createIndex(parentItem->children.size(), 0, parentItem);
}

//---------------------------------------------------------------------------------
int
ResourceTreeModel::rowCount(const QModelIndex& p_parent) const
{
    // Validity checks
    if (p_parent.column() > 0)
        return 0;

    // Get parent item
    ResourceDefinition* parentItem;
    if (!p_parent.isValid())
        parentItem = _rootItem;
    else
        parentItem = static_cast<ResourceDefinition*>(p_parent.internalPointer());

    // Return size of children
    return parentItem->children.size();
}

//---------------------------------------------------------------------------------
int
ResourceTreeModel::columnCount(const QModelIndex& p_parent) const
{
    return 5;
}
