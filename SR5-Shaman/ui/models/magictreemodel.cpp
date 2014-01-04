#include "magictreemodel.h"

#include "rules/rules.h"
#include "data/appstatus.h"


//---------------------------------------------------------------------------------
MagicModelItem::MagicModelItem(MagicModelItem* p_parent)
    : isCategory(false)
    , itemType(MAGICITEMTYPE_INVALID)
    , id("")
    , parent(p_parent)
{
    children.clear();
}

//---------------------------------------------------------------------------------
MagicModelItem::MagicModelItem(const MagicModelItem& p_other)
{
    isCategory = p_other.isCategory;
    itemType = p_other.itemType;
    id = p_other.id;
    parent = p_other.parent;
    children = p_other.children;
}

//---------------------------------------------------------------------------------
MagicModelItem::~MagicModelItem()
{

}

//---------------------------------------------------------------------------------
bool
MagicModelItem::hasChild(const QString& p_id) const
{
    for (unsigned int i = 0; i < children.size(); ++i)
    {
        if (children[i]->id == p_id)
        {
            return true;
        }
    }
    return false;
}

//---------------------------------------------------------------------------------
MagicModelItem*
MagicModelItem::getChild(const QString& p_id) const
{
    for (unsigned int i = 0; i < children.size(); ++i)
    {
        if (children[i]->id == p_id)
        {
            return children[i];
        }
    }
    return NULL;
}

//---------------------------------------------------------------------------------
MagicTreeModel::MagicTreeModel()
{
    // Construct root item
    _rootItem = new MagicModelItem();
    _rootItem->id = "DEFINITION";
    _rootItem->parent = NULL;

    // Add categories
    // Spells
    MagicModelItem* category = new MagicModelItem(_rootItem);
    category->isCategory = true;
    category->id = "CATEGORY_SPELLS";
    category->itemType = MAGICITEMTYPE_SPELL;
    _rootItem->children.push_back(category);
    // Adept powers
    category = new MagicModelItem(_rootItem);
    category->isCategory = true;
    category->id = "CATEGORY_ADEPT_POWERS";
    category->itemType = MAGICITEMTYPE_ADEPT_POWER;
    _rootItem->children.push_back(category);
    // Complex forms
    category = new MagicModelItem(_rootItem);
    category->isCategory = true;
    category->id = "CATEGORY_COMPLEX_FORMS";
    category->itemType = MAGICITEMTYPE_COMPLEX_FORM;
    _rootItem->children.push_back(category);
}

//---------------------------------------------------------------------------------
MagicTreeModel::~MagicTreeModel()
{
    // Children will be deleted recursively
    delete _rootItem;
}

//---------------------------------------------------------------------------------
void
MagicTreeModel::initialize()
{
    // Get the spell categories from the rules
    const QMap<QString, SpellCategoryDefinition*>& spellCatDefs  = MAGIC_RULES->getAllSpellCategoryDefinitions();
    QMap<QString, SpellDefinition*> spellDefs;
    QMap<QString, SpellCategoryDefinition*>::const_iterator it;
    QMap<QString, SpellDefinition*>::const_iterator spellIt;
    MagicModelItem* topCategory = _rootItem->children[0];
    MagicModelItem* spellCategory = NULL;
    MagicModelItem* newItem = NULL;
    SpellCategoryDefinition* spellCatDef = NULL;
    for (it = spellCatDefs.begin(); it != spellCatDefs.end(); ++it)
    {
        spellCatDef = *it;

        // Create the item
        spellCategory = new MagicModelItem();

        // Get the correct category
        category = _rootItem->children[(*it)->type];
        spellCategory->parent = topCategory;

        // Assign the definition id
        spellCategory->id = it.key();

        // Assign the type
        spellCategory->itemType = MAGICITEMTYPE_SPELL;

        // Get all spells of this type and add it
        spellDefs = MAGIC_RULES->getAllSpellDefinitionsByCategory(spellCategory->id);

        // TODO: here

        // Add to spells category
        topCategory->children.push_back(newItem);
    }
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
    if (p_orientation == Qt::Horizontal && p_section == Qt::DisplayRole)
        return QVariant("Wat");

    return QVariant("Hello");
}

//---------------------------------------------------------------------------------
QModelIndex
SkillTreeModel::index(int p_row, int p_column, const QModelIndex& p_parent) const
{
    // Validity checks
    if (p_parent.isValid() && p_parent.column() != 0)
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
    SkillModelItem* childItem = static_cast<SkillModelItem*>(p_index.internalPointer());
    SkillModelItem* parentItem = childItem->parent;

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
