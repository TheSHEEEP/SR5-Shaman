#include "globaleventfilter.h"

#include "ui/mainsplitview.h"

//---------------------------------------------------------------------------------
GlobalEventFilter::GlobalEventFilter(MainSplitView* p_mainSplitView)
    : _mainSplitView(p_mainSplitView)
{

}

//---------------------------------------------------------------------------------
bool
GlobalEventFilter::eventFilter(QObject* p_sender, QEvent* p_event)
{
    if (p_event->type() == QEvent::KeyPress)
    {
        // Propagate to the currently active tab
        QKeyEvent* keyEvent = (QKeyEvent*)p_event;
        _mainSplitView->propagateKeyEventToActiveTab(keyEvent);
    }

    return false;
}
