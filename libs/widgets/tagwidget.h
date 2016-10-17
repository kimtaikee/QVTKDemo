#ifndef TAGWIDGET_H
#define TAGWIDGET_H

#include <QWidget>

#include "widgets_global.h"

namespace Widgets
{
class TagWidgetPrivate;
class WIDGETS_EXPORT TagWidget : public QWidget
{
    Q_OBJECT
public:
    explicit TagWidget(QWidget *parent = 0);
    ~TagWidget();

    void addTag(const QString& tagName);
    void addTags(const QStringList& tags);
    void clear();
    QStringList tags() const;

private:
    Q_DISABLE_COPY(TagWidget)
    TagWidgetPrivate* d_ptr;
};

} // namespace Widgets

#endif // TAGWIDGET_H
