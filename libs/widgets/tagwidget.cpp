#include "tagwidget.h"

#include <3dparty/flowlayout.h>

#include <QLabel>

namespace Widgets
{

class TagWidgetPrivate
{
public:
    QStringList tags;
    FlowLayout* layout = nullptr;
    QList<QLabel*> labels;

    void configLabel(QLabel* label);
};

void TagWidgetPrivate::configLabel(QLabel *label)
{
    if (!label)
        return;

    QString style;
    style = "padding:3px;border:none;border-radius:5px;background-color:rgb(72,145,220);font:bold 14px;color:white";

    label->setStyleSheet(style);
}

TagWidget::TagWidget(QWidget *parent) : QWidget(parent)
{
    d_ptr = new TagWidgetPrivate;
    d_ptr->layout = new FlowLayout;
    setLayout(d_ptr->layout);
}

TagWidget::~TagWidget()
{
    delete d_ptr;
}

void TagWidget::addTag(const QString& tagName)
{
    if (d_ptr->tags.contains(tagName))
        return;

    QLabel* label = new QLabel(this);
    d_ptr->labels.append(label);
    d_ptr->configLabel(label);
    label->setText(tagName);
    d_ptr->layout->addWidget(label);
}

void TagWidget::addTags(const QStringList& tags)
{
    foreach (const QString& tag, tags)
        addTag(tag);
}

void TagWidget::clear()
{
    d_ptr->tags.clear();
    qDeleteAll(d_ptr->labels);
    d_ptr->labels.clear();

    d_ptr->layout->clear();
}

QStringList TagWidget::tags() const
{
    return d_ptr->tags;
}


} // namespace Widgets
