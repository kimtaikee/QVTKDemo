#include "filetree.h"

#include <vtkutils/datafilter.h>

#include <QMenu>
#include <QContextMenuEvent>
#include <QDebug>
#include <QHeaderView>
#include <QFileDialog>
#include <QDebug>

FileTree::FileTree(QWidget* parent) : QTreeWidget(parent)
{
    init();
}

void FileTree::contextMenuEvent(QContextMenuEvent *e)
{
    QTreeWidgetItem* item = this->itemAt(e->pos());

    QMenu* contextMenu = nullptr;

    if (!item || itemType(item) == IT_Root) {
        createFileContextMenu();
        contextMenu = m_fileContextMenu;
    } else if (itemType(item) == IT_File ||
               itemType(item) == IT_Filter) {
        createFilterContextMenu();
        contextMenu = m_filterContextMenu;
    }

    contextMenu->exec(e->globalPos());
}

FileTree::ItemType FileTree::itemType(QTreeWidgetItem* item)
{
    if (!item) {
        qDebug() << "FileTree::itemType: null tree item.";
        return IT_File;
    }

    return static_cast<ItemType>(item->data(0, ItemTypeRole).toInt());
}

void FileTree::setItemType(QTreeWidgetItem* item, ItemType type)
{
    if (!item) {
        qDebug() << "FileTree::setItemType: null tree item.";
        return;
    }
    item->setData(0, ItemTypeRole, type);
}

bool FileTree::addFile()
{
    static QString previousDir = ".";
    QString filter = "ANSYS Files(*.obj);;VTK Files(*.vtk);;STL Files(*.stl);;ANSYS Files(*.cdb);;"
                     "Fluent Files(*.cas);;PLY Files(*.ply);;PDB Files(*.pdb)";
    QString file = QFileDialog::getOpenFileName(this, tr("Import File"), previousDir, filter);
    if (file.isEmpty())
        return false;

    if (m_files.contains(file)) {
        qDebug() << "FileTree::addFile: already add this file.";
        return false;
    }

    QFileInfo fi(file);
    previousDir = fi.canonicalPath();
    QTreeWidgetItem* fileItem = new QTreeWidgetItem;
    fileItem->setText(0, fi.fileName());
    fileItem->setToolTip(0, file);

    setItemType(fileItem, IT_File);
    m_rootItem->addChild(fileItem);
    expandItem(m_rootItem);

    m_files.append(file);

    emit importFile(file);

    return true;
}

void FileTree::addFilter()
{
    QTreeWidgetItem* currItem = this->currentItem();
    if (itemType(currItem) == IT_File) {
        QTreeWidgetItem* currItem = this->currentItem();
        if (!currItem)
            return;

        const QString file = currItem->toolTip(0);
        QAction* act = qobject_cast<QAction*>(sender());
        int filterType = act->data().toInt();

        QTreeWidgetItem* filterItem = new QTreeWidgetItem;
        setItemType(filterItem, IT_Filter);
        filterItem->setText(0, VtkUtils::filterName(static_cast<VtkUtils::FilterType>(filterType)));
        currItem->addChild(filterItem);

        expandItem(currItem);

        qDebug() << "add filter for file: " << file;
        emit filterAdded(file, filterType);
    }
}

void FileTree::onItemClicked(QTreeWidgetItem *item, int column)
{
    Q_UNUSED(column)
    ItemType type = itemType(item);

    if (type == IT_File) {
        QVariant data = item->toolTip(0);
        emit setCurrentFile(data.toString());
    }
}

void FileTree::init()
{
    setAlternatingRowColors(true);
    header()->hide();
    connect(this, SIGNAL(itemClicked(QTreeWidgetItem*,int)), SLOT(onItemClicked(QTreeWidgetItem*,int)));

    m_rootItem = new QTreeWidgetItem();
    setItemType(m_rootItem, IT_Root);
    m_rootItem->setText(0, tr("Files"));
    addTopLevelItem(m_rootItem);
}

void FileTree::createFileContextMenu()
{
    if (!m_fileContextMenu) {
        m_fileContextMenu = new QMenu(tr("Context"), this);

        QAction* addFileAct = new QAction(tr("Add File"), this);
        connect(addFileAct, SIGNAL(triggered()), this, SLOT(addFile()));

        m_fileContextMenu->addAction(addFileAct);
    }
}

void FileTree::createFilterContextMenu()
{
    if (!m_filterContextMenu) {
        m_filterContextMenu = new QMenu(tr("Processor"), this);

        QAction* filterAct = createFilterAction("Clip", VtkUtils::FT_Clip);
        m_filterContextMenu->addAction(filterAct);
        filterAct = createFilterAction("Slice", VtkUtils::FT_Slice);
        m_filterContextMenu->addAction(filterAct);
        filterAct = createFilterAction("Smooth", VtkUtils::FT_Smooth);
        m_filterContextMenu->addAction(filterAct);
        filterAct = createFilterAction("Decimate", VtkUtils::FT_Decimate);
        m_filterContextMenu->addAction(filterAct);
    }
}

QAction* FileTree::createFilterAction(const QString& text, VtkUtils::FilterType type)
{
    QAction* act = new QAction(text);
    act->setData(type);
    connect(act, SIGNAL(triggered()), this, SLOT(addFilter()));
    return act;
}


