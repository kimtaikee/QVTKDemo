#ifndef WIDGET_H
#define WIDGET_H

#define vtkRenderingCore_AUTOINIT 3(vtkInteractionStyle,vtkRenderingFreeType,vtkRenderingOpenGL)
#define vtkRenderingVolume_AUTOINIT 1(vtkRenderingVolumeOpenGL)

#include <vtkAutoInit.h>

VTK_MODULE_INIT(vtkRenderingOpenGL)
VTK_MODULE_INIT(vtkInteractionStyle)

#include <QVTKWidget.h>

class VTKWidget : public QVTKWidget
{
    Q_OBJECT
public:
    explicit VTKWidget(QWidget* parent = nullptr);

};

#endif
