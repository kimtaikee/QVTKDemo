#ifndef WIDGET_H
#define WIDGET_H

#define vtkRenderingCore_AUTOINIT 3(vtkInteractionStyle,vtkRenderingFreeType,vtkRenderingOpenGL2)
#define vtkRenderingVolume_AUTOINIT 1(vtkRenderingVolumeOpenGL2)

#include <vtkAutoInit.h>

VTK_MODULE_INIT(vtkRenderingOpenGL2)
VTK_MODULE_INIT(vtkInteractionStyle)

#include <QVTKWidget.h>

namespace Utils
{
    class TableModel;
}

class vtkActor;
class VTKWidget : public QVTKWidget
{
    Q_OBJECT
public:
    explicit VTKWidget(QWidget* parent = nullptr);

    void setModel(Utils::TableModel* model);

private slots:
    void onModelDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight);

private:
    void constructSurfaceFromModel(Utils::TableModel* model);

    vtkActor* m_surfaceActor = nullptr;
};

#endif
