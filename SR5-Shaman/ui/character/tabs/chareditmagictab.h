#ifndef CHAREDITMAGICTAB_H
#define CHAREDITMAGICTAB_H

#include <QWidget>

namespace Ui {
class CharEditMagicTab;
}

class SkillSortFilterProxyModel;

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
    SkillSortFilterProxyModel*  _skillFilter;

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
};

#endif // CHAREDITMAGICTAB_H
