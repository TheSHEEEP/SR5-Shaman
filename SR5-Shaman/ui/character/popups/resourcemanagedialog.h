#ifndef RESOURCEMANAGEDIALOG_H
#define RESOURCEMANAGEDIALOG_H

#include <QDialog>

namespace Ui {
class ResourceManageDialog;
}

/**
 * @brief This dialog is responsible for giving the user possibilities to manage a single
 *          resource like a weapon, clothing, etc.
 */
class ResourceManageDialog : public QDialog
{
    Q_OBJECT

public:
    /**
     * @brief Constructor.
     * @param p_id  The ID of the item to manage. This is the ID of the CHOSEN resource, not the one in
     *              the resource rules.
     */
    explicit ResourceManageDialog(QWidget* p_parent, const QString& p_id);

    /**
     * @brief Destructor.
     **/
    ~ResourceManageDialog();

    /**
     * @brief Applies the current translation to this tab.
     */
    void applyTranslation();

private:
    Ui::ResourceManageDialog* ui;

    QString     _id;
};

#endif // RESOURCEMANAGEDIALOG_H
