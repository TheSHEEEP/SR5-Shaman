#ifndef RESOURCETREEMODEL_H
#define RESOURCETREEMODEL_H

#include <QAbstractItemModel>
#include <QModelIndex>
#include <QVariant>
#include <QMap>
#include <vector>

#include "rules/Resourcerules.h"

class QAbstractItemView;
class QSpinBox;

/**
 * @brief This model is used to organize Resources in a tree view.
 * @author  TheSHEEEP
 */
class ResourceTreeModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    /**
     * @brief Constructor.
     */
    ResourceTreeModel(bool p_selectedResourcesMode = false);

    /**
     * @brief Destructor.
     */
    ~ResourceTreeModel();

    /**
     * @brief Will get all resources from the rules.
     */
    void initialize();

    /**
     * @brief Emits the data changed signal.
     * Call this after you added/removed a Resource.
     */
    void update();

    /**
     * @brief Sets the abstract item view (required for advanced mode).
     */
    void setItemView(QAbstractItemView* p_view);

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
     * @param p_Resource  The Resource to look at.
     * @param p_parent  The parent to search in.
     */
    QModelIndex index(int p_row, int p_Resource, const QModelIndex& p_parent = QModelIndex()) const;

    /**
     * @brief Returns the parent of the passed model index.
     */
    QModelIndex parent(const QModelIndex& p_index) const;

    /**
     * @brief Returns the row count of the passed model index.
     */
    int rowCount(const QModelIndex& p_parent = QModelIndex()) const;

    /**
     * @brief Returns the resource count of the passed model index.
     */
    int columnCount(const QModelIndex& p_parent = QModelIndex()) const;

private:
    ResourceDefinition*     _rootItem;

    bool                _selectedResourcesMode;
    QAbstractItemView*  _itemView;
};


//---------------------------------------------------------------------------------
inline
void
ResourceTreeModel::setItemView(QAbstractItemView* p_view)
{
    _itemView = p_view;
}

#endif // RESOURCETREEMODEL_H
