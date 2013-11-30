#ifndef SKILLTREEMODEL_H
#define SKILLTREEMODEL_H

#include <QAbstractItemModel>
#include <QModelIndex>
#include <QVariant>
#include <vector>
/**
 * @brief Helper class that holds a skill model item.
 */
struct SkillItem
{
    QString                 id;
    std::vector<SkillItem*> children;
};

/**
 * @brief This model is used to organize skills in a tree view.
 * @author  TheSHEEEP
 */
class SkillTreeModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    /**
     * @brief Constructor.
     */
    SkillTreeModel();

    /**
     * @brief Destructor.
     */
    ~SkillTreeModel();

    // TODO: here, create basic skill rules
};

#endif // SKILLTREEMODEL_H
