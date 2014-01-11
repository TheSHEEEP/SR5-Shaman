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
    for (unsigned int i = 0; i < children.size(); ++i)
    {
        delete children[i];
    }
    children.clear();
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
    MagicModelItem* topCategory = _rootItem->children[MAGICITEMTYPE_SPELL];
    MagicModelItem* spellCategory = NULL;
    MagicModelItem* newItem = NULL;
    SpellCategoryDefinition* spellCatDef = NULL;
    for (it = spellCatDefs.begin(); it != spellCatDefs.end(); ++it)
    {
        spellCatDef = *it;

        // Create the item
        spellCategory = new MagicModelItem();

        // Get the correct category
        topCategory = _rootItem->children[MAGICITEMTYPE_SPELL];
        spellCategory->parent = topCategory;

        // Assign the definition id
        spellCategory->id = it.key();

        // Assign the type
        spellCategory->itemType = MAGICITEMTYPE_SPELL;

        // Get all spells of this type and add them
        spellDefs = MAGIC_RULES->getAllSpellDefinitionsByCategory(spellCategory->id);
        for (spellIt = spellDefs.begin(); spellIt != spellDefs.end(); ++spellIt)
        {
            newItem = new MagicModelItem();
            newItem->id = spellIt.key();
            newItem->isCategory = false;
            newItem->itemType = MAGICITEMTYPE_SPELL;
            newItem->parent = spellCategory;
            spellCategory->children.push_back(newItem);
        }

        // Add to spells category
        topCategory->children.push_back(spellCategory);
    }

    // Get the adept powers from the rules
    const QMap<QString, AdeptPowerDefinition*>& adeptDefs  = MAGIC_RULES->getAllAdeptPowerDefinitions();
    QMap<QString, AdeptPowerDefinition*>::const_iterator it2;
    topCategory = _rootItem->children[MAGICITEMTYPE_ADEPT_POWER];
    newItem = NULL;
    AdeptPowerDefinition* powerDef = NULL;
    for (it2 = adeptDefs.begin(); it2 != adeptDefs.end(); ++it2)
    {
        powerDef = *it2;

        // Create the item
        newItem = new MagicModelItem();

        // Get the correct category
        topCategory = _rootItem->children[MAGICITEMTYPE_ADEPT_POWER];
        newItem->parent = topCategory;

        // Assign the definition id
        newItem->id = it2.key();

        // Assign the type
        newItem->itemType = MAGICITEMTYPE_ADEPT_POWER;

        // Add to adept powers category
        topCategory->children.push_back(newItem);
    }

    // Get the complex forms from the rules
    const QMap<QString, ComplexFormDefinition*>& formDefs  = MAGIC_RULES->getAllComplexFormDefinitions();
    QMap<QString, ComplexFormDefinition*>::const_iterator it3;
    topCategory = _rootItem->children[MAGICITEMTYPE_COMPLEX_FORM];
    newItem = NULL;
    ComplexFormDefinition* formDef = NULL;
    for (it3 = formDefs.begin(); it3 != formDefs.end(); ++it3)
    {
        formDef = *it3;

        // Create the item
        newItem = new MagicModelItem();

        // Get the correct category
        topCategory = _rootItem->children[MAGICITEMTYPE_COMPLEX_FORM];
        newItem->parent = topCategory;

        // Assign the definition id
        newItem->id = it3.key();

        // Assign the type
        newItem->itemType = MAGICITEMTYPE_COMPLEX_FORM;

        // Add to adept powers category
        topCategory->children.push_back(newItem);
    }
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

    MagicModelItem* item = static_cast<MagicModelItem*>(p_index.internalPointer());

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
    MagicModelItem* parentItem;
    if (!p_parent.isValid())
        parentItem = _rootItem;
    else
        parentItem = static_cast<MagicModelItem*>(p_parent.internalPointer());

    // Get correct child item and create model index
    MagicModelItem* childItem = parentItem->children[p_row];
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
    MagicModelItem* childItem = static_cast<MagicModelItem*>(p_index.internalPointer());
    MagicModelItem* parentItem = childItem->parent;

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
    MagicModelItem* parentItem;
    if (!p_parent.isValid())
        parentItem = _rootItem;
    else
        parentItem = static_cast<MagicModelItem*>(p_parent.internalPointer());

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
//        return static_cast<SkillModelItem*>(p_parent.internalPointer())->columnCount();
//    else
//        return _rootItem->columnCount();
}
