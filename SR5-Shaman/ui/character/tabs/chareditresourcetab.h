#ifndef CHAREDITRESOURCETAB_H
#define CHAREDITRESOURCETAB_H

#include <QWidget>

namespace Ui {
class CharEditResourceTab;
}
class ResourceTreeModel;
class ResourceSortFilterProxyModel;
class ResourceDelegate;
class PriorityEventFilter;

class CharEditResourceTab : public QWidget
{
    Q_OBJECT

public:
    /**
     * @brief Constructor.
     */
    explicit CharEditResourceTab(QWidget *parent = 0);

    /**
     * @brief Destructor.
     */
    ~CharEditResourceTab();

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
     * @brief Will adjust the size of the skills view columns.
     */
    virtual void resizeEvent(QResizeEvent* p_event);

    /**
     * @brief Some shortcuts for this tab.
     * @param p_keyEvent    The event with the pressed key(s).
     */
    virtual void keyPressedEvent(QKeyEvent* p_keyEvent);

signals:
    void guidedNextStep();
    void disableNext();

private:
    Ui::CharEditResourceTab *ui;

    ResourceTreeModel*             _resourceModel;
    ResourceSortFilterProxyModel*  _resourceFilter;
    ResourceDelegate*              _resourceDelegate;
    PriorityEventFilter*           _filter;

    QVector<int>   _expandedResources;

    /**
     * @brief During guided creation, checks if all conditions are met to continue with the creation.
     */
    void checkContinue();

    /**
     * @brief Updates all displayed values.
     */
    void updateValues();

    /**
     * @brief Will store the current view state (what is expanded).
     */
    void storeViewState();

    /**
     * @brief Will restore the stored view state (what was expanded).
     */
    void restoreViewState();

private slots:

    /**
     * @brief Refreshes the skill view.
     */
    void forceViewUpdate();

    /**
     * @brief Will set the selected priority for skills.
     * @param p_index   The now selected index.
     */
    void on_cbPriority_currentIndexChanged(int p_index);

    /**
     * @brief Will enable/disable the remove item button.
     * @param p_current     The currently selected item.
     * @param p_previous    The previously selected item.
     */
    void handleResourceChanged(const QModelIndex& p_current, const QModelIndex& p_previous);

    /**
     * @brief Will update the purchased nuyen.
     */
    void on_spinBuyNuyen_valueChanged(int arg1);
};

#endif // CHAREDITRESOURCETAB_H
