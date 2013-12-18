#ifndef CHAREDITMAGICTAB_H
#define CHAREDITMAGICTAB_H

#include <QWidget>

namespace Ui {
class CharEditMagicTab;
}

class SkillSortFilterProxyModel;
class SkillDelegate;

/**
 * @brief This tab holds all information about the character's magic type, selection of magical / resonance skills
 *          and the selection of spells / complex forms.
 * @author  TheSHEEEP
 */
class CharEditMagicTab : public QWidget
{
    Q_OBJECT

public:
    /**
     * @brief Constructor.
     */
    explicit CharEditMagicTab(QWidget *parent = 0);

    /**
     * @brief Destructor.
     */
    ~CharEditMagicTab();

    /**
     * @brief Initializes this character tab.
     */
    void initialize();

    /**
     * @brief Will show/hide the elements specific to the guided creation.
     */
    virtual void showEvent(QShowEvent* /*unused*/);

signals:
    void guidedNextStep();
    void disableNext();

private:
    Ui::CharEditMagicTab*       ui;
    SkillSortFilterProxyModel*  _skillsAvailableFilter;
    SkillDelegate*              _skillsAvailableDelegate;
    SkillSortFilterProxyModel*  _skillsFilter;
    SkillDelegate*              _skillsDelegate;


    /**
     * @brief Updates all displayed values.
     */
    void updateValues();

    /**
     * @brief During guided creation, checks if all conditions are met to continue with the creation.
     */
    void checkContinue();

    /**
     * @brief Checks if the skills views should be shown or hidden and updates them.
     */
    void showHideSkillsViews();

private slots:
    /**
     * @brief Emits the guidedNextStep signal.
     */
    void on_btnGuidedContinue_clicked();

    /**
     * @brief Will update the character values and views to present new values for selected aspect.
     * @param p_index   The new selected index.
     */
    void on_cbAspect_currentIndexChanged(int p_index);

    /**
     * @brief Will enable/disable the remove or add skill button.
     * @param p_current     The currently selected item.
     * @param p_previous    The previously selected item.
     */
    void handleSkillChanged(const QModelIndex& p_current, const QModelIndex& p_previous);

    /**
     * @brief Will add the selected skill to the list of selected skills.
     */
    void on_btnAddSkill_clicked();
    void on_btnRemoveSkill_clicked();
};

#endif // CHAREDITMAGICTAB_H
