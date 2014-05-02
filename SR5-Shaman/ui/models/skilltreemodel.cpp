#include "skilltreemodel.h"

#include <QAbstractItemView>
#include <QSpinBox>

#include "rules/rules.h"
#include "data/appstatus.h"
#include "data/dictionary.h"

//---------------------------------------------------------------------------------
SkillTreeModel::SkillTreeModel(bool p_advancedMode)
    : _advancedMode(p_advancedMode)
    , _itemView(NULL)
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

    // Clean up item controls
    if (_advancedMode)
    {
        QMap<SkillDefinition*, SkillControlElements*>::iterator it;
        for (it = _itemControls.begin(); it != _itemControls.end(); ++it)
        {
            if ((*it)->increases)
            {
                delete (*it)->increases;
            }

            delete (*it);
        }
    }
}

//---------------------------------------------------------------------------------
void
SkillTreeModel::updateItemControls()
{
    // In advanced mode
    // Make sure the item has all the controls it needs and apply them
    if (_advancedMode)
    {
        // Iterate over all categories
        for (unsigned int i = 0; i < _rootItem->children.size(); ++i)
        {
            QModelIndex categoryIndex = index(i, 0);
            // Iterate over all skills inside the category
            for (unsigned int j = 0; j < _rootItem->children[i]->children.size(); ++j)
            {
                SkillDefinition* item = _rootItem->children[i]->children[j];

                // Create required controls
                SkillControlElements* elements;
                if (!_itemControls.contains(item))
                {
                    // Create elements
                    elements = new SkillControlElements();
                    _itemControls.insert(item, elements);

                    // All in-/decreasable skills need a spin box
                    elements->increases = new QSpinBox();
                    QModelIndex itemIndex = index(j, 1, categoryIndex);

                    _itemView->setIndexWidget(itemIndex, elements->increases);
                }
            }
        }
    }
}

//---------------------------------------------------------------------------------
QVariant
SkillTreeModel::data(const QModelIndex& p_index, int p_role) const
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

    Qt::ItemFlags flag = QAbstractItemModel::flags(p_index);
    if (p_index.column() == 1)
    {
        flag |= Qt::ItemIsEditable;
    }
    return flag;
}

//---------------------------------------------------------------------------------
QVariant
SkillTreeModel::headerData(int p_section, Qt::Orientation p_orientation, int p_role) const
{
    if (p_role != Qt::DisplayRole)
        return QVariant();

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
            return QVariant(Dictionary::getTranslation("VIEW_TOTAL_VALUE"));
        }
    }

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
    return _advancedMode ? 4 : 1;
}
