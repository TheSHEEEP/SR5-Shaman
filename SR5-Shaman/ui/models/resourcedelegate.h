#ifndef RESOURCEDELEGATE_H
#define RESOURCEDELEGATE_H

#include <QItemDelegate>

class ResourceDefinition;

/**
 * @brief This delegate can be configred to display a Resource in different variations.
 *          Default is to display the translation.
 */
class ResourceDelegate : public QItemDelegate
{
    Q_OBJECT

public:
    /**
     * @brief Constructor.
     */
    ResourceDelegate();

    /**
     * @brief Destructor.
     */
    ~ResourceDelegate();

    /**
     * @brief Creates an editor widget for some items.
     */
    QWidget* createEditor(QWidget* p_parent, const QStyleOptionViewItem& p_option,
                          const QModelIndex& p_index) const;

    /**
     * @brief Sets the editor's data from the passed model index.
     */
    void setEditorData(QWidget* p_editor, const QModelIndex& p_index) const;

    /**
     * @brief Updates the models data by reading from the editor.
     */
    void setModelData(QWidget* p_editor, QAbstractItemModel* p_model,
                      const QModelIndex& p_index) const;

    /**
     * @brief Returns the correct size hint for the passed index.
     * @param p_option  The style options.
     * @param p_index   The index.
     */
    QSize sizeHint(const QStyleOptionViewItem& p_option, const QModelIndex& p_index) const;

    /**
     * @brief Defines how the Resource will be painted.
     * @param p_painter The painter to use.
     * @param p_option  The option with the default display parameters.
     * @param p_index   The model index to display.
     */
    void paint(QPainter* p_painter, const QStyleOptionViewItem& p_option, const QModelIndex& p_index) const;

signals:

private slots:
};

#endif // RESOURCEDELEGATE_H
