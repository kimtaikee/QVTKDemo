#include "imagecompareboard.h"

#include <QLabel>
#include <QScrollArea>
#include <QSplitter>
#include <QHBoxLayout>
#include <QScrollBar>

namespace Widgets
{

class ImageCompareBoardPrivate
{
public:
    QLabel* originalImgLabel;
    QLabel* comparedImgLabel;

    QImage originalImg;
    QImage comparedImg;
};

ImageCompareBoard::ImageCompareBoard(QWidget *parent) : QWidget(parent)
{
    d_ptr = new ImageCompareBoardPrivate;

    d_ptr->originalImgLabel = new QLabel(this);
    d_ptr->comparedImgLabel = new QLabel(this);

    QHBoxLayout* layout = new QHBoxLayout;

    QSplitter* splitter = new QSplitter(Qt::Horizontal, this);
    splitter->setHandleWidth(1);
    QScrollArea* originScrollArea = new QScrollArea(this);
    originScrollArea->setWidgetResizable(true);
    QScrollArea* compareScrollArea = new QScrollArea(this);
    compareScrollArea->setWidgetResizable(true);
    originScrollArea->setWidget(d_ptr->originalImgLabel);
    compareScrollArea->setWidget(d_ptr->comparedImgLabel);
    splitter->addWidget(originScrollArea);
    splitter->addWidget(compareScrollArea);

    layout->addWidget(splitter);
    setLayout(layout);

    // sync with each other
    connect(originScrollArea->horizontalScrollBar(), SIGNAL(valueChanged(int)), compareScrollArea->horizontalScrollBar(), SLOT(setValue(int)));
    connect(compareScrollArea->horizontalScrollBar(), SIGNAL(valueChanged(int)), originScrollArea->horizontalScrollBar(), SLOT(setValue(int)));
    connect(compareScrollArea->verticalScrollBar(), SIGNAL(valueChanged(int)), originScrollArea->verticalScrollBar(), SLOT(setValue(int)));
    connect(originScrollArea->verticalScrollBar(), SIGNAL(valueChanged(int)), compareScrollArea->verticalScrollBar(), SLOT(setValue(int)));

    // hide them initially
    d_ptr->originalImgLabel->hide();
    d_ptr->comparedImgLabel->hide();
}

ImageCompareBoard::~ImageCompareBoard()
{
    delete d_ptr;
}

void ImageCompareBoard::setOriginalImage(const QImage& img)
{
    d_ptr->originalImgLabel->setPixmap(QPixmap::fromImage(img));
    d_ptr->originalImgLabel->show();
}

QImage ImageCompareBoard::originalImage() const
{
    return d_ptr->originalImg;
}

void ImageCompareBoard::setComparedImage(const QImage& img)
{
    d_ptr->comparedImgLabel->setPixmap(QPixmap::fromImage(img));
    d_ptr->comparedImgLabel->show();
}

QImage ImageCompareBoard::comparedImage() const
{
    return d_ptr->comparedImg;
}

} // namespace Widgets
