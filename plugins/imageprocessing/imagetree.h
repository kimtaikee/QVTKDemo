#ifndef IMAGETREE_H
#define IMAGETREE_H

#include <QTreeWidget>
#include <QMap>

class QMenu;
class ImageTree : public QTreeWidget
{
    Q_OBJECT
public:
    enum ItemType { IT_Root, IT_Image, IT_Processor, IT_Count };
    enum { ItemTypeRole = Qt::UserRole + 4};

    explicit ImageTree(QWidget* parent = nullptr);

signals:
    void imageAdded(const QString& image);
    void imageChanged(const QString& image);

protected:
    void contextMenuEvent(QContextMenuEvent* e);

    ItemType itemType(QTreeWidgetItem* item);
    void setItemType(QTreeWidgetItem* item, ItemType type);

    void createImageMenu();
    void createProcessorMenu();

private slots:
    void importImage();
    void onItemClicked(QTreeWidgetItem* item, int column);

private:
    QTreeWidgetItem* m_rootItem = nullptr;
    QMenu* m_imageContextMenu = nullptr;
    QMenu* m_processorContextMenu = nullptr;
};

#endif // IMAGETREE_H
