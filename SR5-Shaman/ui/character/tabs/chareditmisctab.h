#ifndef CHAREDITMISCTAB_H
#define CHAREDITMISCTAB_H

#include <QWidget>

namespace Ui {
class CharEditMiscTab;
}

class PriorityEventFilter;

/**
 * @brief This tab holds character information like name, nick, portrait and metatype.
 *          The kind of magic user is also selected here.
 * @author  TheSHEEEP
 */
class CharEditMiscTab : public QWidget
{
    Q_OBJECT

public:
    /**
     * @brief Constructor.
     */
    explicit CharEditMiscTab(QWidget *parent = 0);

    /**
     * @brief Destructor.
     */
    ~CharEditMiscTab();

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

private slots:
    /**
     * @brief Stores the new metatype and updates priority selection.
     */
    void on_cbMetatype_currentIndexChanged(int index);

    /**
     * @brief Stores the new priority for the metatype.
     */
    void on_cbPriority_currentIndexChanged(int index);

    /**
     * @brief Stores the name of the character.
     */
    void on_leRealName_textEdited(const QString& p_name);

    /**
     * @brief Stores the nick of the character.
     */
    void on_leAlias_textEdited(const QString& p_nick);

    /**
     * @brief Emits the guidedNextStep signal.
     */
    void on_btnGuidedContinue_clicked();

    /**
     * @brief Activates/deactivates the magic type selection.
     */
    void on_checkIsMagicUser_stateChanged(int p_state);

    /**
     * @brief Fills the selection of available priorities for the selected type.
     */
    void on_cbMagicType_currentIndexChanged(int p_index);

    /**
     * @brief Stores the selected magic priority.
     */
    void on_cbMagicPriority_currentIndexChanged(int p_index);

private:
    Ui::CharEditMiscTab*    ui;
    PriorityEventFilter*    _filter;

    /**
     * @brief Updates all displayed elements.
     */
    void updateValues();

    /**
     * @brief During guided creation, checks if all conditions are met to continue with the creation.
     */
    void checkContinue();
};

#endif // CHAREDITMISCTAB_H
