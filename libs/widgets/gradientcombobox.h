#ifndef GRADIENTCOMBOBOX_H
#define GRADIENTCOMBOBOX_H

#include <QComboBox>

#include "widgets_global.h"

class QListWidgetItem;
namespace Widgets
{

class GradientComboBoxPrivate;
class WIDGETS_EXPORT GradientComboBox : public QComboBox
{
    Q_OBJECT
public:
    explicit GradientComboBox(QWidget* parent = nullptr);
    ~GradientComboBox();

    void setCurrentIndex(int index);
    int currentIndex() const;

    QString currentName() const;
    QColor currentColor1() const;
    QColor currentColor2() const;

    static QPair<QColor, QColor> colorPair(int index);

    void showPopup();
    void hidePopup();

signals:
    void colorsChanged(const QString& name, const QColor& clr1, const QColor& clr2);

protected:
    void paintEvent(QPaintEvent *e);
    void resizeEvent(QResizeEvent* e);
    QSize minimumSizeHint() const;

private slots:
    void onActivated(int index);

private:
    GradientComboBoxPrivate* d_ptr;
    Q_DISABLE_COPY(GradientComboBox)
};

} // namespace Widgets
#endif // GRADIENTCOMBOBOX_H
