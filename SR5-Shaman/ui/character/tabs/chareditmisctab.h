#ifndef CHAREDITMISCTAB_H
#define CHAREDITMISCTAB_H

#include <QWidget>

namespace Ui {
class CharEditMiscTab;
}

/**
 * @brief This tab holds character information like name, nick, portrait and metatype.
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
    virtual void showEvent(QShowEvent* p_event);

signals:
    void guidedNextStep();

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

private:
    Ui::CharEditMiscTab *ui;

    /**
     * @brief During guided creation, checks if all conditions are met to continue with the creation.
     */
    void checkContinue();
};

#endif // CHAREDITMISCTAB_H
