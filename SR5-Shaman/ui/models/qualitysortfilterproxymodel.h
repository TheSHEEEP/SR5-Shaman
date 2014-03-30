#ifndef QUALITYSORTFILTERPROXYMODEL_H
#define QUALITYSORTFILTERPROXYMODEL_H

#include <QSortFilterProxyModel>
#include <QStringList>

#include "rules/qualityrules.h"
#include "qualitytreemodel.h"

#define QUALITY_FILTERMASK_ID_CONTAINS    1  // Use the filterIDContains (getFilterIDContains) for filtering
#define QUALITY_FILTERMASK_ID_EQUALS      2  // Use the filterIDEquals (getFilterIDEquals) for filtering

class QualitySortFilterProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT

public:
    /**
     * @brief Constructor.
     */
    QualitySortFilterProxyModel(QObject* p_parent = 0);

    /**
     * @brief Destructor.
     */
    virtual ~QualitySortFilterProxyModel();

    /**
     * @brief Has to be overridden in order not to screw up the display, it seems.
     *          See here: http://schneide.wordpress.com/2010/04/26/wrestling-with-qts-model-view-api-filtering-in-treemodels/
     */
    int columnCount(const QModelIndex& p_parent ) const;

    /**
     * @brief If the filter will show empty categories.
     */
    void setShowEmptyCategories(bool p_show);

    /**
     * @brief Sets the filter mask to use.
     *          Use defines like QUALITY_FILTERMASK_ID_CONTAINS here to set how the filtering behaves.
     */
    void setFilterMask(int p_mask);

    /**
     * @brief This filter will only show items whose ID contains an entry in the passed QStringList.
     * @note    The ID does not have to be equal with an entry, but only contain an entry as a substring.
     */
    void setFilterIDContains(const QStringList& p_filterIDs);

    /**
     * @brief Returns the contains filter for IDs.
     */
    QStringList& getFilterIDContains();

    /**
     * @brief This filter will only show items whose ID is equal to an entry in the passed QStringList.
     */
    void setFilterIDEquals(const QStringList& p_filterIDs);

    /**
     * @brief Returns the equals filter for IDs.
     */
    QStringList& getFilterIDEquals();

    /**
     * @brief Applies the filtering, updating all views using this.
     */
    void applyFilter();

protected:
    /**
     * @brief Will filter the rows depending on the conditions set in this instance.
     * @param p_sourceRow       The source row to check.
     * @param p_sourceParent    The source parent to check.
     * @return True if the row is accepted.
     */
    virtual bool filterAcceptsRow(int p_row, const QModelIndex& p_parent) const;

    /**
     * @brief Will return true if the left model index is less than the right one.
     *          This implementation uses alphabetic order.
     */
    virtual bool lessThan(const QModelIndex& p_left, const QModelIndex& p_right) const;

private:
    bool        _showEmptyCategories;
    int         _filterMask;

    QStringList                     _filterIDContains;
    QStringList                     _filterIDEquals;

    /**
     * @brief Checks the item according to the filters conditions.
     */
    bool filterAcceptsItem(QualityDefinition* p_item) const;
};

//---------------------------------------------------------------------------------
inline
void
QualitySortFilterProxyModel::setShowEmptyCategories(bool p_show)
{
    _showEmptyCategories = p_show;
}

//---------------------------------------------------------------------------------
inline
void
QualitySortFilterProxyModel::setFilterMask(int p_mask)
{
    _filterMask = p_mask;
}

//---------------------------------------------------------------------------------
inline
void
QualitySortFilterProxyModel::setFilterIDContains(const QStringList& p_filterIDs)
{
    _filterIDContains = p_filterIDs;
}

//---------------------------------------------------------------------------------
inline
QStringList&
QualitySortFilterProxyModel::getFilterIDContains()
{
    return _filterIDContains;
}

//---------------------------------------------------------------------------------
inline
void
QualitySortFilterProxyModel::setFilterIDEquals(const QStringList& p_filterIDs)
{
    _filterIDEquals = p_filterIDs;
}

//---------------------------------------------------------------------------------
inline
QStringList&
QualitySortFilterProxyModel::getFilterIDEquals()
{
    return _filterIDEquals;
}

#endif // QUALITYSORTFILTERPROXYMODEL_H
