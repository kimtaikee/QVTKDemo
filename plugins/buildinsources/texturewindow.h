#ifndef TEXTUREWINDOW_H
#define TEXTUREWINDOW_H

#include <QWidget>

#include <vtkSmartPointer.h>

namespace Ui
{
    class TextureWindow;
}

namespace VtkUtils
{
    class VtkWidget;
}

class vtkActor;
class vtkImageData;
class QListWidgetItem;
class TextureWindow : public QWidget
{
    Q_OBJECT
public:
    explicit TextureWindow(QWidget *parent = 0);
    ~TextureWindow();

private slots:
    void on_pushButton_clicked();
    void on_textureList_itemClicked(QListWidgetItem *item);
    void on_shapeTypeCombo_currentIndexChanged(int index);

private:
    void useDefaultTexture();
    void useImageData(vtkImageData* imageData);
    void loadResTextures();

private:
    enum Type { Sphere, Plane, Cylinder, Surface };

    Ui::TextureWindow *m_ui;
    Type m_type = Sphere;
    vtkSmartPointer<vtkImageData> m_currImageData;
    vtkSmartPointer<vtkActor> m_actor;
    VtkUtils::VtkWidget* m_vtkWidget = nullptr;
};

#endif // TEXTUREWINDOW_H
