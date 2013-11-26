#include "comboprioritydelegate.h"

#include <QDebug>
#include <QComboBox>
#include <QListView>
#include <QEvent>

#include "data/character/characterchoices.h"

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

            // Is the index taken by another priority?
            Priority prioAtIndex = CHARACTER_CHOICES->getPriorityAtIndex(checkIndex);
            if (prioAtIndex != PRIORITY_INVALID &&
                checkIndex != currentIndex)
            {
                // Set the suffix
                QString suffix = tr(" ... Taken by ");
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

                // Change the text
                QString previousText = box->itemText(i);
                int stringIndex = previousText.indexOf(" ... ");
                if (stringIndex != -1)
                {
                    previousText = previousText.left(stringIndex);
                }
                box->setItemText(i, previousText + suffix);

            } // END index taken
        } // END item iteration
    } // END if mouse pressed event

    return false;
}

//---------------------------------------------------------------------------------
ComboPriorityDelegate::ComboPriorityDelegate(QComboBox *parent)
    : QItemDelegate(parent)
{
    // Not-so-nice workaround to be able to change the item texts
    _filter = new PriorityEventFilter();
    parent->installEventFilter(_filter);
}

//---------------------------------------------------------------------------------
ComboPriorityDelegate::~ComboPriorityDelegate()
{
    delete _filter;
}

//---------------------------------------------------------------------------------
void
ComboPriorityDelegate::paint(QPainter* p_painter, const QStyleOptionViewItem& p_option, const QModelIndex& p_index) const
{
    // Get current index
    // Workaround with parent is required because p_index.data() holds the text instead of the actual item data
    QComboBox* ptr = (QComboBox*)parent();
    int currentIndex = ptr->itemData(p_index.row()).toInt();
    if (currentIndex < -1 || currentIndex > 4)
    {
        qCritical() << QString("Passed model does have an invalid index: %1").arg(p_index.data().toString());
        return;
    }

    // Is the index taken by another priority?
    Priority prioAtIndex = CHARACTER_CHOICES->getPriorityAtIndex(currentIndex);
    QStyleOptionViewItem newOption(p_option);
    if (prioAtIndex != PRIORITY_INVALID &&
        ptr->currentIndex() != p_index.row())
    {
        // Set the color
        newOption.palette.setColor(QPalette::Text, QColor(255, 0, 0));
    }

    QItemDelegate::paint(p_painter, newOption, p_index);
}
