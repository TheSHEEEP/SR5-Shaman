#ifndef GLOBALEVENTFILTER_H
#define GLOBALEVENTFILTER_H

#include <QObject>
#include <QEvent>
#include <QKeyEvent>

class MainSplitView;

/**
 * @brief Thhis class is responsible for propagating some events to certain UI instances.
 */
class GlobalEventFilter : public QObject
{
public:
    /**
     * @brief Constructor.
     * @param p_mainSplitView   The MainSplitView to propagate some evnts to.
     */
    GlobalEventFilter(MainSplitView* p_mainSplitView);

    /**
     * @brief Assign the main split view.
     */
    void setMainSplitView(MainSplitView* p_mainSplitView)
    {
        _mainSplitView = p_mainSplitView;
    }

protected:
    /**
     * @brief Filter the events.
     */
    bool eventFilter(QObject* p_sender, QEvent* p_event);

private:
    MainSplitView*  _mainSplitView;
};

#endif // GLOBALEVENTFILTER_H
