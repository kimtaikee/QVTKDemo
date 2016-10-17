#ifndef VTKWIDGET_H
#define VTKWIDGET_H

#define vtkRenderingCore_AUTOINIT 3(vtkInteractionStyle,vtkRenderingFreeType,vtkRenderingOpenGL) // ogl2
#define vtkRenderingVolume_AUTOINIT 1(vtkRenderingVolumeOpenGL)

#include <vtkAutoInit.h>

VTK_MODULE_INIT(vtkRenderingOpenGL) // ogl2
VTK_MODULE_INIT(vtkInteractionStyle)

#include <QVTKWidget.h>

#include "vtkutils_global.h"

class vtkActor;
class vtkProp;
namespace VtkUtils
{

class VtkWidgetPrivate;
class VTKUTILS_EXPORT VtkWidget : public QVTKWidget
{
    Q_OBJECT
public:
    explicit VtkWidget(QWidget* parent = nullptr);
    virtual ~VtkWidget();

    void setMultiViewports(bool multi = true);
    bool multiViewports() const;

    void addActor(vtkProp* actor, const QColor& clr = Qt::black);
    void addViewProp(vtkProp* prop);
    QList<vtkProp*> actors() const;

    void setActorsVisible(bool visible);
    void setActorVisible(vtkProp* actor, bool visible);
    bool actorVisible(vtkProp* actor);

    void setBackgroundColor(const QColor& clr);
    QColor backgroundColor() const;

    vtkRenderer* defaultRenderer();
    bool defaultRendererTaken() const;

    void showOrientationMarker(bool show = true);

protected:
    void setBounds(double* bounds);

    double xMin() const;
    double xMax() const;
    double yMin() const;
    double yMax() const;
    double zMin() const;
    double zMax() const;

private:
    VtkWidgetPrivate* d_ptr;
    Q_DISABLE_COPY(VtkWidget)
};

} // namespace VtkUtils
#endif // VTKWIDGET_H
