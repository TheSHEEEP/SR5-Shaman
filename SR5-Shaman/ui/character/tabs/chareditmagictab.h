#ifndef CHAREDITMAGICTAB_H
#define CHAREDITMAGICTAB_H

#include <QWidget>

namespace Ui {
class CharEditMagicTab;
}

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
    Ui::CharEditMagicTab* ui;

    /**
     * @brief Updates all displayed values.
     */
    void updateValues();

    /**
     * @brief During guided creation, checks if all conditions are met to continue with the creation.
     */
    void checkContinue();

private slots:
    /**
     * @brief Emits the guidedNextStep signal.
     */
    void on_btnGuidedContinue_clicked();
};

#endif // CHAREDITMAGICTAB_H
