#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QWidget>
#include <QMap>
//#include <widgets/imagecompareboard.h>

namespace Ui
{
    class MainWidget;
}

namespace Widgets
{
    class ImageCompareBoard;
}

class ImageTree;
class MainWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MainWidget(QWidget *parent = 0);
    ~MainWidget();

private slots:
    void onImageAdded(const QString& image);
    void onImageChanged(const QString& image);

private:
    Ui::MainWidget *m_ui;

    QMap<QString, Widgets::ImageCompareBoard*> m_imageBoardMap;
    ImageTree* m_imageTree;
};

#endif // MAINWIDGET_H
