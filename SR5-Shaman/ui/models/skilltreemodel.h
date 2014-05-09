#ifndef SKILLTREEMODEL_H
#define SKILLTREEMODEL_H

#include <QAbstractItemModel>
#include <QModelIndex>
#include <QVariant>
#include <QMap>
#include <vector>

#include "rules/skillrules.h"

class QAbstractItemView;
class QSpinBox;

/**
 * @brief The SkillControlElements struct
 */
struct SkillControlElements
{
public:
    SkillControlElements()
        : increases(NULL)
    {     }

    QSpinBox*   increases;
};

/**
 * @brief This model is used to organize skills in a tree view.
 * @author  TheSHEEEP
 */
class SkillTreeModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    /**
     * @brief Constructor.
     */
    SkillTreeModel(bool p_advancedMode = false);

    /**
     * @brief Destructor.
     */
    ~SkillTreeModel();

    /**
     * @brief Will get all skills from the rules.
     */
    void initialize();

    /**
     * @brief Emits the data changed signal.
     * Call this after you added/removed a skill.
     */
    void update();

    /**
     * @brief Sets the abstract item view (required for advanced mode).
     */
    void setItemView(QAbstractItemView* p_view);

    /**
     * @brief Adds a skill to the model.
     * @param p_id  The ID of the skill to add.
     */
    void addSkill(const QString& p_id);

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
    SkillDefinition*     _rootItem;

    bool                                            _advancedMode;
    QAbstractItemView*                              _itemView;
};


//---------------------------------------------------------------------------------
inline
void
SkillTreeModel::setItemView(QAbstractItemView* p_view)
{
    _itemView = p_view;
}

#endif // SKILLTREEMODEL_H
