#include "imagetree.h"

#include <widgets/imagecompareboard.h>

#include <QMenu>
#include <QContextMenuEvent>
#include <QDebug>
#include <QFileDialog>
#include <QFileInfo>

ImageTree::ImageTree(QWidget* parent) : QTreeWidget(parent)
{
    m_rootItem = new QTreeWidgetItem;
    m_rootItem->setText(0, tr("Images"));
    addTopLevelItem(m_rootItem);

    connect(this, SIGNAL(itemClicked(QTreeWidgetItem*,int)), this, SLOT(onItemClicked(QTreeWidgetItem*,int)));
}

void ImageTree::contextMenuEvent(QContextMenuEvent *e)
{
    QTreeWidgetItem* item = itemAt(e->pos());

    QMenu* menu = nullptr;
    if (!item || itemType(item) == IT_Root) {
        createImageMenu();
        menu = m_imageContextMenu;
    } else if (itemType(item) == IT_Image) {
        createProcessorMenu();;
        menu = m_processorContextMenu;
    }

    menu->exec(e->globalPos());
}

ImageTree::ItemType ImageTree::itemType(QTreeWidgetItem* item)
{
    if (!item) {
        qDebug() << "FileTree::itemType: null tree item.";
        return IT_Image;
    }

    return static_cast<ItemType>(item->data(0, ItemTypeRole).toInt());
}

void ImageTree::setItemType(QTreeWidgetItem* item, ItemType type)
{
    if (!item) {
        qDebug() << "FileTree::setItemType: null tree item.";
        return;
    }
    item->setData(0, ItemTypeRole, type);
}


void ImageTree::createImageMenu()
{
    if (!m_imageContextMenu) {
        m_imageContextMenu = new QMenu(this);

        QAction* importImgAct = new QAction(tr("Image image"), this);
        connect(importImgAct, SIGNAL(triggered()), this, SLOT(importImage()));
        m_imageContextMenu->addAction(importImgAct);
    }
}

void ImageTree::createProcessorMenu()
{
    if (!m_processorContextMenu) {
        m_processorContextMenu = new QMenu(this);
    }
}

void ImageTree::importImage()
{
    QString img = QFileDialog::getOpenFileName(this, tr("Select Image"), ".", "PNG Files(*.png);;JPG Files(*.jpg);;BMP Files(*.bmp)");
    if (img.isEmpty())
        return;

    QFileInfo fi(img);
    QTreeWidgetItem* imgItem = new QTreeWidgetItem;
    imgItem->setText(0, fi.baseName());
    imgItem->setToolTip(0, img);
    setItemType(imgItem, IT_Image);
    m_rootItem->addChild(imgItem);
    expandItem(m_rootItem);

    emit imageAdded(img);
}

void ImageTree::onItemClicked(QTreeWidgetItem* item, int column)
{
    if (itemType(item) == IT_Image) {
        QString file = item->toolTip(column);
        emit imageChanged(file);
    }
}

