#ifndef CHAREDITQUALITYTAB_H
#define CHAREDITQUALITYTAB_H

#include <QWidget>

namespace Ui {
class CharEditQualityTab;
}
class QualitySortFilterProxyModel;
class QualityDelegate;


/**
 * @brief This tab holds information about all character qualities.
 *          New qualities can be chosen and existing ones removed.
 * @author  TheSHEEEP
 */
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

    /**
     * @brief Will show/hide the elements specific to the guided creation.
     */
    virtual void showEvent(QShowEvent* /*unused*/);

    /**
     * @brief Some shortcuts for this tab.
     * @param p_keyEvent    The event with the pressed key(s).
     */
    virtual void keyPressedEvent(QKeyEvent* p_keyEvent);

signals:
    void guidedNextStep();
    void disableNext();

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

    void on_btnGuidedContinue_clicked();

private:
    Ui::CharEditQualityTab *ui;

    QualitySortFilterProxyModel*  _qualitiesAvailableFilter;
    QualityDelegate*              _qualitiesAvailableDelegate;
    QualitySortFilterProxyModel*  _qualitiesFilter;
    QualityDelegate*              _qualitiesDelegate;

    /**
     * @brief Updates all displayed values.
     */
    void updateValues();
};

#endif // CHAREDITQUALITYTAB_H
