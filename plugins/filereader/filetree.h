#ifndef FILETREE_H
#define FILETREE_H

#include <QTreeWidget>

#include <vtkutils/datafilter.h>

class FileTree : public QTreeWidget
{
    Q_OBJECT
public:
    enum ItemType { IT_Root, IT_File, IT_Filter };
    enum { ItemTypeRole = Qt::UserRole + 3};

    explicit FileTree(QWidget* parent = nullptr);

signals:
    void importFile(const QString& file);
    void setCurrentFile(const QString& file);
    void filterAdded(const QString& file, int filterType);

protected:
    void contextMenuEvent(QContextMenuEvent *e);

    ItemType itemType(QTreeWidgetItem* item);
    void setItemType(QTreeWidgetItem* item, ItemType type);

private slots:
    bool addFile();
    void addFilter();
    void onItemClicked(QTreeWidgetItem *item, int column);

private:
    void init();
    void createFileContextMenu();
    void createFilterContextMenu();
    QAction* createFilterAction(const QString& text, VtkUtils::FilterType type);

    QMenu* m_fileContextMenu = nullptr;
    QMenu* m_filterContextMenu = nullptr;
    QTreeWidgetItem* m_rootItem = nullptr;
    QStringList m_files;
};

#endif // FILETREE_H
