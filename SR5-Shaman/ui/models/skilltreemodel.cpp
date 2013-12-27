#include "skilltreemodel.h"

#include "rules/rules.h"
#include "data/appstatus.h"


//---------------------------------------------------------------------------------
SkillModelItem::SkillModelItem(SkillModelItem* p_parent)
    : isCategory(false)
    , type(SKILL_TYPE_INVALID)
    , id("")
    , parent(p_parent)
{
    children.clear();
}

//---------------------------------------------------------------------------------
SkillModelItem::SkillModelItem(const SkillModelItem& p_other)
{
    isCategory = p_other.isCategory;
    type = p_other.type;
    id = p_other.id;
    parent = p_other.parent;
    children = p_other.children;
}

//---------------------------------------------------------------------------------
SkillModelItem::~SkillModelItem()
{
    // Do not delete if this is a group - group skills are deleted by the parent category
    if (id != "" &&
        !SKILL_RULES->getDefinition(id).isGroup)
    {
        for (unsigned int i = 0; i < children.size(); ++i)
        {
            delete children[i];
        }
    }
}

//---------------------------------------------------------------------------------
bool
SkillModelItem::hasChild(const QString& p_id) const
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
SkillModelItem*
SkillModelItem::getChild(const QString& p_id) const
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
SkillTreeModel::SkillTreeModel()
{
    // Construct root item
    _rootItem = new SkillModelItem();
    _rootItem->id = "DEFINITION";
    _rootItem->parent = NULL;

    // Add categories
    // THE ORDER IS VERY IMPORTANT HERE, DEPENDS ON THE SKILL TYPE ENUM
    // Combat
    SkillModelItem* category = new SkillModelItem(_rootItem);
    category->isCategory = true;
    category->id = "CATEGORY_COMBAT";
    category->type = SKILL_TYPE_COMBAT;
    _rootItem->children.push_back(category);
    // Physical
    category = new SkillModelItem(_rootItem);
    category->isCategory = true;
    category->id = "CATEGORY_PHYSICAL";
    category->type = SKILL_TYPE_PHYSICAL;
    _rootItem->children.push_back(category);
    // Social
    category = new SkillModelItem(_rootItem);
    category->isCategory = true;
    category->id = "CATEGORY_SOCIAL";
    category->type = SKILL_TYPE_SOCIAL;
    _rootItem->children.push_back(category);
    // Magical
    category = new SkillModelItem(_rootItem);
    category->isCategory = true;
    category->id = "CATEGORY_MAGIC";
    category->type = SKILL_TYPE_MAGIC;
    _rootItem->children.push_back(category);
    // Resonance
    category = new SkillModelItem(_rootItem);
    category->isCategory = true;
    category->id = "CATEGORY";
    category->type = SKILL_TYPE_RESONANCE;
    _rootItem->children.push_back(category);
    // Technical
    category = new SkillModelItem(_rootItem);
    category->isCategory = true;
    category->id = "CATEGORY_TECHNICAL";
    category->type = SKILL_TYPE_TECHNICAL;
    _rootItem->children.push_back(category);
    // Vehicle
    category = new SkillModelItem(_rootItem);
    category->isCategory = true;
    category->id = "CATEGORY_VEHICLE";
    category->type = SKILL_TYPE_VEHICLE;
    _rootItem->children.push_back(category);
    // Knowledge
    category = new SkillModelItem(_rootItem);
    category->isCategory = true;
    category->id = "CATEGORY_KNOWLEDGE";
    category->type = SKILL_TYPE_KNOWLEDGE;
    _rootItem->children.push_back(category);
    // Groups - this is a workaround
    category = new SkillModelItem(_rootItem);
    category->isCategory = true;
    category->id = "CATEGORY_GROUPS";
    category->type = NUM_SKILL_TYPES;
    _rootItem->children.push_back(category);
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
    // TODO: It sucks somehow that the tree model items and actual skill definition struct are different things.
    //          Could this be improved?
    //          One problem is that the skill definitions do not store categories.

    // Get the skills from the rules
    const QMap<QString, SkillDefinition*>& definitions  = SKILL_RULES->getAllDefinitions();
    QMap<QString, SkillDefinition*>::const_iterator it;
    SkillModelItem* category = NULL;
    SkillModelItem* newItem = NULL;
    SkillDefinition* skill = NULL;
    SkillModelItem* groupCategory = _rootItem->children[_rootItem->children.size() - 1];
    for (it = definitions.begin(); it != definitions.end(); ++it)
    {
        skill = *it;

        // If this is a group, ignore it
        if (skill->isGroup)
        {
            continue;
        }

        // Create the item
        newItem = new SkillModelItem();

        // Get the correct category
        category = _rootItem->children[(*it)->type];
        newItem->parent = category;

        // Assign the definition id
        newItem->id = it.key();

        // Assign the type
        newItem->type = (*it)->type;

        // Is this also in a group?
        if ((*it)->group != "none")
        {
            // Do we need to create the group?
            SkillModelItem* groupItem;
            if (!groupCategory->hasChild((*it)->group))
            {
                groupItem = new SkillModelItem(groupCategory);
                groupItem->id = (*it)->group;
                groupCategory->children.push_back(groupItem);
            }
            groupItem = groupCategory->getChild((*it)->group);

            // Duplicate the item - cannot have the same item in multiple parents, it seems
            // TODO: Is there a better way to do this?
            SkillModelItem* newItem2 = new SkillModelItem(*newItem);
            newItem2->parent = groupItem;

            // Add to the group
            groupItem->children.push_back(newItem2);
        }

        // Add to category
        category->children.push_back(newItem);
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
