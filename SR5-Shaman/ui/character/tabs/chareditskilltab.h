#ifndef CHAREDITSKILLTAB_H
#define CHAREDITSKILLTAB_H

#include <QWidget>

namespace Ui {
class CharEditSkillTab;
}

class SkillSortFilterProxyModel;
class SkillDelegate;

/**
 * @brief This tab holds information about all character skills.
 *          New skills can also be added here, and existing skills changed.
 * @author  TheSHEEEP
 */
class CharEditSkillTab : public QWidget
{
    Q_OBJECT

public:
    /**
     * @brief Constructor.
     */
    explicit CharEditSkillTab(QWidget *parent = 0);

    /**
     * @brief Destructor.
     */
    ~CharEditSkillTab();

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
    virtual void keyPressEvent(QKeyEvent* p_keyEvent);

signals:
    void guidedNextStep();
    void disableNext();

private:
    Ui::CharEditSkillTab*   ui;

    SkillSortFilterProxyModel*  _skillsFilter;
    SkillDelegate*              _skillsDelegate;

    /**
     * @brief During guided creation, checks if all conditions are met to continue with the creation.
     */
    void checkContinue();

private slots:
    /**
     * @brief Will enable/disable the remove skill button.
     * @param p_current     The currently selected item.
     * @param p_previous    The previously selected item.
     */
    void handleSkillChanged(const QModelIndex& p_current, const QModelIndex& p_previous);

    /**
     * @brief Will remove/reset the selected skill.
     */
    void on_btnRemoveSkill_clicked();
};

#endif // CHAREDITSKILLTAB_H