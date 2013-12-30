#ifndef SKILLSORTFILTERMODEL_H
#define SKILLSORTFILTERMODEL_H

#include <QSortFilterProxyModel>
#include <QStringList>

#include "rules/skillrules.h"

#define SKILL_FILTERMASK_ID_CONTAINS    1  // Use the filterIDContains (getFilterIDContains) for filtering
#define SKILL_FILTERMASK_ID_EQUALS      2  // Use the filterIDEquals (getFilterIDEquals) for filtering
#define SKILL_FILTERMASK_TYPE           4  // Use the filterTypes (getFilterTypes) for filtering

class SkillModelItem;

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
     * @brief Has to be overridden in order not to screw up the display, it seems.
     *          See here: http://schneide.wordpress.com/2010/04/26/wrestling-with-qts-model-view-api-filtering-in-treemodels/
     */
    int columnCount(const QModelIndex& p_parent ) const;

    /**
     * @brief If the filter will show empty categories.
     */
    void setShowEmptyCategories(bool p_show);

    /**
     * @brief If this is true, skill groups will be shown.
     */
    void setShowSkillGroups(bool p_skillGroups);

    /**
     * @brief If this is true, skill that are not inside  skill group will be shown.
     */
    void setShowNormalSkills(bool p_skillGroups);

    /**
     * @brief Sets the filter mask to use.
     *          Use defines like SKILL_FILTERMASK_ID_CONTAINS here to set how the filtering behaves.
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
     * @brief This filter will only show items whose type is one of those in the passed vector.
     */
    void setFilterTypes(const std::vector<SkillType>& p_filterTypes);

    /**
     * @brief Returns the filter for types.
     */
    std::vector<SkillType>& getFilterTypes();

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
    bool        _showSkillGroups;
    bool        _showNormalSkills;
    int         _filterMask;

    QStringList             _filterIDContains;
    QStringList             _filterIDEquals;
    std::vector<SkillType>  _filterTypes;

    /**
     * @brief Checks the item according to the filters conditions.
     */
    bool filterAcceptsItem(SkillModelItem* p_item) const;
};

//---------------------------------------------------------------------------------
inline
void
SkillSortFilterProxyModel::setShowEmptyCategories(bool p_show)
{
    _showEmptyCategories = p_show;
}

//---------------------------------------------------------------------------------
inline
void
SkillSortFilterProxyModel::setShowSkillGroups(bool p_skillGroups)
{
    _showSkillGroups = p_skillGroups;
}

//---------------------------------------------------------------------------------
inline
void
SkillSortFilterProxyModel::setShowNormalSkills(bool p_normalSkills)
{
    _showNormalSkills = p_normalSkills;
}

//---------------------------------------------------------------------------------
inline
void
SkillSortFilterProxyModel::setFilterMask(int p_mask)
{
    _filterMask = p_mask;
}

//---------------------------------------------------------------------------------
inline
void
SkillSortFilterProxyModel::setFilterIDContains(const QStringList& p_filterIDs)
{
    _filterIDContains = p_filterIDs;
}

//---------------------------------------------------------------------------------
inline
QStringList&
SkillSortFilterProxyModel::getFilterIDContains()
{
    return _filterIDContains;
}

//---------------------------------------------------------------------------------
inline
void
SkillSortFilterProxyModel::setFilterIDEquals(const QStringList& p_filterIDs)
{
    _filterIDEquals = p_filterIDs;
}

//---------------------------------------------------------------------------------
inline
QStringList&
SkillSortFilterProxyModel::getFilterIDEquals()
{
    return _filterIDEquals;
}

//---------------------------------------------------------------------------------
inline
void
SkillSortFilterProxyModel::setFilterTypes(const std::vector<SkillType>& p_filterTypes)
{
    _filterTypes = p_filterTypes;
}

//---------------------------------------------------------------------------------
inline
std::vector<SkillType>&
SkillSortFilterProxyModel::getFilterTypes()
{
    return _filterTypes;
}

#endif // SKILLSORTFILTERMODEL_H
