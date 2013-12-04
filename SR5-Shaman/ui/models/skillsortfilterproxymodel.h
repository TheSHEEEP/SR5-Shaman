#ifndef SKILLSORTFILTERMODEL_H
#define SKILLSORTFILTERMODEL_H

#include <QSortFilterProxyModel>

class SkillSortFilterProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT

public:
    /**
     * @brief Constructor.
     */
    SkillSortFilterProxyModel(QObject* p_parent = 0);

    /**
     * @brief Destructor.
     */
    virtual ~SkillSortFilterProxyModel();

    /**
     * @brief Has to be overridden in order not to screw up the display.
     *          See here: http://schneide.wordpress.com/2010/04/26/wrestling-with-qts-model-view-api-filtering-in-treemodels/
     */
    int columnCount(const QModelIndex& p_parent ) const;

protected:
    /**
     * @brief Will filter the rows depending on the conditions set in this instance.
     * @param p_sourceRow       The source row to check.
     * @param p_sourceParent    The source parent to check.
     * @return True if the row is accepted.
     */
    bool filterAcceptsRow(int p_sourceRow, const QModelIndex& p_sourceParent) const;

    /**
     * @brief Will return true if the left model index is less than the right one.
     *          This implementation uses alphabetic order.
     */
    bool lessThan(const QModelIndex& p_left, const QModelIndex& p_right) const;

private:
};

#endif // SKILLSORTFILTERMODEL_H
