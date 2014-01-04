#ifndef MAGICTREEMODEL_H
#define MAGICTREEMODEL_H

#include <QAbstractItemModel>
#include <QModelIndex>
#include <QVariant>
#include <vector>

#include "rules/magicrules.h"

enum MagicItemType
{
    MAGICITEMTYPE_INVALID = -1,
    MAGICITEMTYPE_SPELL,
    MAGICITEMTYPE_ADEPT_POWER,
    MAGICITEMTYPE_COMPLEX_FORM,
    NUM_MAGICITEMTYPES
};

/**
 * @brief Helper class that holds a magic model item.
 *          This is used for spells, complex forms and adept powers.
 */
class MagicModelItem
{
public:
    /**
     * @brief Constructor.
     */
    MagicModelItem(MagicModelItem* p_parent = NULL);

    /**
     * @brief Copy constructor.
     */
    MagicModelItem(const MagicModelItem& p_other);

    /**
     * @brief Destructor.
     */
    ~MagicModelItem();

    /**
     * @brief Returns true if this item has a child with the passed value.
     * @param p_id  The ID to look for.
     */
    bool hasChild(const QString& p_id) const;

    /**
     * @brief Returns the child with the passed value. Or NULL, if no child was found.
     */
    MagicModelItem* getChild(const QString& p_id) const;

    bool                            isCategory;
    MagicItemType                   itemType;
    QString                         id; // Unique ID, use for lookup in magic rules

    MagicModelItem*                 parent;
    std::vector<MagicModelItem*>    children;
};

Q_DECLARE_METATYPE(MagicModelItem);

/**
 * @brief This model is used to organize skills in a tree view.
 * @author  TheSHEEEP
 */
class MagicTreeModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    /**
     * @brief Constructor.
     */
    MagicTreeModel();

    /**
     * @brief Destructor.
     */
    ~MagicTreeModel();

    /**
     * @brief Will get all spells, adept powers and complex forms from the rules.
     */
    void initialize();

    /**
     * @brief Returns the data of the passed model index.
     */
    QVariant data(const QModelIndex& p_index, int p_role) const;

    /**
     * @brief Returns the flags of the passed model index.
     */
    Qt::ItemFlags flags(const QModelIndex& p_index) const;

    /**
     * @brief Returns the header data fpr the passed values.
     * @param p_section     The required section.
     * @param p_orientation The required orientation.
     * @param p_role        The required role.
     */
    QVariant headerData(int p_section, Qt::Orientation p_orientation, int p_role = Qt::DisplayRole) const;

    /**
     * @brief Returns the model index of the passed values.
     * @param p_row     The row to look at.
     * @param p_column  The column to look at.
     * @param p_parent  The parent to search in.
     */
    QModelIndex index(int p_row, int p_column, const QModelIndex& p_parent = QModelIndex()) const;

    /**
     * @brief Returns the parent of the passed model index.
     */
    QModelIndex parent(const QModelIndex& p_index) const;

    /**
     * @brief Returns the row count of the passed model index.
     */
    int rowCount(const QModelIndex& p_parent = QModelIndex()) const;

    /**
     * @brief Returns the column count of the passed model index.
     */
    int columnCount(const QModelIndex& p_parent = QModelIndex()) const;

private:
    MagicModelItem*     _rootItem;
};


#endif // MAGICTREEMODEL_H
