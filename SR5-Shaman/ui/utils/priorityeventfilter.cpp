#include "priorityeventfilter.h"

#include <QDebug>
#include <QComboBox>
#include <QListView>
#include <QEvent>
#include <QSettings>

#include "data/character/characterchoices.h"
#include "data/appstatus.h"

//---------------------------------------------------------------------------------
PriorityEventFilter::PriorityEventFilter()
{

}

//---------------------------------------------------------------------------------
PriorityEventFilter::~PriorityEventFilter()
{

}

//---------------------------------------------------------------------------------
bool
PriorityEventFilter::eventFilter(QObject* p_obj, QEvent* p_event)
{
    // We need the show event
    if (p_event->type() == QEvent::MouseButtonPress)
    {
        QComboBox* box = (QComboBox*)p_obj;

        // Get current index
        int currentIndex = -2;
        if (box->count() > 0)
        {
            currentIndex = box->itemData(box->currentIndex()).toInt();
        }

        // Iterate over each index
        for(int i = 0; i < box->count(); ++i)
        {
            int checkIndex = box->itemData(i).toInt();
            QString suffix = "";
            QColor color(0, 0, 0);
            // Is the index taken by another priority?
            Priority prioAtIndex = CHARACTER_CHOICES->getPriorityAtIndex(checkIndex);
            if (prioAtIndex != PRIORITY_INVALID &&
                checkIndex != currentIndex)
            {
                // Set the color
                color = APPSTATUS->getHelperColors().cbTaken;

                // Set the suffix
                suffix = tr(" ... Taken by ");
                switch (prioAtIndex)
                {
                case PRIORITY_ATTRIBUTES:
                    suffix.append(tr("attributes"));
                    break;

                case PRIORITY_MAGIC:
                    suffix.append(tr("magic / resonance"));
                    break;

                case PRIORITY_METATYPE:
                    suffix.append(tr("metatype"));
                    break;

                case PRIORITY_RESOURCES:
                    suffix.append(tr("resources"));
                    break;

                case PRIORITY_SKILLS:
                    suffix.append(tr("skills"));
                    break;

                case PRIORITY_NUM_PRIORITIES:
                case PRIORITY_INVALID:
                    break;
                }
            } // END index taken
            else
            {
                color = APPSTATUS->getHelperColors().cbFree;
            }

            // Change the text
            QString previousText = box->itemText(i);
            int stringIndex = previousText.indexOf(" ... ");
            if (stringIndex != -1)
            {
                previousText = previousText.left(stringIndex);
            }
            box->setItemText(i, previousText + suffix);
            box->setItemData(i, color, Qt::ForegroundRole);

        } // END item iteration

        // box->view()->adjustSize();
        // box->updateGeometry();
        box->adjustSize();
    } // END if mouse pressed event

    return false;
}

//---------------------------------------------------------------------------------
void
PriorityEventFilter::handlePrioritySelection(int p_index)
{
    // If an element was selected, remove the possible suffix
    QComboBox* box = (QComboBox*)sender();
    if (box->currentText() != "")
    {
        QString newText = box->itemText(box->currentIndex());
        int index = newText.indexOf(" ... ");
        if (index != -1)
        {
            newText = newText.left(index);
        }
        box->setItemText(box->currentIndex(), newText);
    }
}
