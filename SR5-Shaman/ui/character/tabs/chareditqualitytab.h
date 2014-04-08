#ifndef CHAREDITQUALITYTAB_H
#define CHAREDITQUALITYTAB_H

#include <QWidget>

namespace Ui {
class CharEditQualityTab;
}
class QualitySortFilterProxyModel;
class QualityDelegate;


class CharEditQualityTab : public QWidget
{
    Q_OBJECT

public:
    /**
     * @brief Constructor.
     */
    explicit CharEditQualityTab(QWidget *parent = 0);

    /**
     * @brief Destructor.
     */
    ~CharEditQualityTab();

    /**
     * @brief Initializes this tab.
     */
    void initialize();

    /**
     * @brief Applies the current translation to this tab.
     */
    void applyTranslation();

private slots:
    /**
     * @brief Will enable/disable the add/remove buttons depending on the current selection.
     * @param p_current     The currently selected model index.
     * @param p_previous    The previously selected model index.
     */
    void handleQualityChanged(const QModelIndex& p_current, const QModelIndex& p_previous);

    /**
     * @brief Will remove the selected quality.
     */
    void on_btnRemoveQuality_clicked();

    /**
     * @brief Will add the selected quality.
     */
    void on_btnAddQuality_clicked();

private:
    Ui::CharEditQualityTab *ui;

    QualitySortFilterProxyModel*  _qualitiesAvailableFilter;
    QualityDelegate*              _qualitiesAvailableDelegate;
    QualitySortFilterProxyModel*  _qualitiesFilter;
    QualityDelegate*              _qualitiesDelegate;
};

#endif // CHAREDITQUALITYTAB_H
