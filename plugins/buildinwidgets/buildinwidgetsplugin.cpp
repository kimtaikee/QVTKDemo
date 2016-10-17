#include "buildinwidgetsplugin.h"
#include "distancewidgetwindow.h"
#include "anglewidgetwindow.h"
#include "sliderwidgetwindow.h"
#include "textwidgetwindow.h"
#include "scalarbarwidgetwindow.h"
#include "contourwidgetwindow.h"
#include "camerawidgetwindow.h"

#include <vtkutils/vtkutils.h>
#include <vtkutils/vtkwidget.h>

#include <vtkRenderWindow.h>
#include <vtkActor.h>
#include <vtkConeSource.h>
#include <vtkBoxWidget.h>
#include <vtkImplicitPlaneWidget.h>
#include <vtkSphereWidget.h>
#include <vtkSphereWidget2.h>
#include <vtkPointWidget.h>
#include <vtkLineWidget.h>
#include <vtkImagePlaneWidget.h>
#include <vtkPlaneWidget.h>
#include <vtkSplineWidget.h>
#include <vtkSliderWidget.h>
#include <vtkDistanceWidget.h>
#include <vtkAngleWidget.h>
#include <vtkCommand.h>
#include <vtkTransform.h>
#include <vtkCylinderSource.h>
#include <vtkProperty.h>

#include <QMap>
#include <QDebug>

class BoxCallback : public vtkCommand
{
public:
  static BoxCallback *New()
    { return new BoxCallback; }
  virtual void Execute(vtkObject *caller, unsigned long, void*)
    {
      vtkTransform *t = vtkTransform::New();
      vtkBoxWidget *widget = reinterpret_cast<vtkBoxWidget*>(caller);

      widget->GetTransform(t);
      widget->GetProp3D()->SetUserTransform(t);

      t->Delete();
    }
};

class PlaneCallBack : public vtkCommand
{
public:
    static PlaneCallBack *New() { return new PlaneCallBack; }

    virtual void Execute(vtkObject *caller, unsigned long eventId, void *callData)
    {
        vtkImplicitPlaneWidget* planeWidget = reinterpret_cast<vtkImplicitPlaneWidget*>(caller);
        qDebug() << "origin: " << planeWidget->GetOrigin()[0] << "," << planeWidget->GetOrigin()[1] << "," << planeWidget->GetOrigin()[2];
        qDebug() << "normal: " << planeWidget->GetNormal()[0] << "," << planeWidget->GetNormal()[1] << "," << planeWidget->GetNormal()[2];
    }
};

BuildinWidgetsPlugin::BuildinWidgetsPlugin()
{

}

BuildinWidgetsPlugin::~BuildinWidgetsPlugin()
{

}

QString BuildinWidgetsPlugin::category() const
{
    return QObject::tr("Buildin Widgets");
}

template <class WidgetType, class SourceType = vtkCylinderSource>
static QWidget* createWidget(const QString& title)
{
    VtkUtils::VtkWidget* widget = new VtkUtils::VtkWidget();
    widget->setWindowTitle(title);

    WidgetType* box = WidgetType::New();
    box->SetInteractor(widget->GetInteractor());
    box->SetPlaceFactor(1.25);

    vtkActor* actor = VtkUtils::createSourceActor<SourceType>();
    actor->GetProperty()->SetOpacity(0.7);
    box->SetProp3D(actor);
    box->PlaceWidget();

    BoxCallback *callback = BoxCallback::New();
    box->AddObserver(vtkCommand::LeftButtonPressEvent, callback);
    widget->addActor(actor);
    box->On();

    return widget;
}

QList<QWidget*> BuildinWidgetsPlugin::examples() const
{
    QList<QWidget*> widgetList;

    widgetList.append(createWidget<vtkLineWidget>(tr("Line Widget")));
    widgetList.append(createWidget<vtkBoxWidget>(tr("Box Widget")));
    widgetList.append(createWidget<vtkPointWidget>(tr("Point Widget")));
    widgetList.append(createWidget<vtkImplicitPlaneWidget>(tr("Implicit Plane Widget")));
    widgetList.append(createWidget<vtkSphereWidget>(tr("Sphere Widget")));
    widgetList.append(createWidget<vtkSplineWidget>(tr("Spline Widget")));
    widgetList.append(createWidget<vtkPlaneWidget>(tr("Plane Widget")));
    widgetList.append(new DistanceWidgetWindow());
    widgetList.append(new AngleWidgetWindow());
    widgetList.append(new SliderWidgetWindow());
    widgetList.append(new TextWidgetWindow());
    widgetList.append(new ScalarBarWidgetWindow());
    widgetList.append(new ContourWidgetWindow());
    widgetList.append(new CameraWidgetWindow());

    return widgetList;
}

QStringList BuildinWidgetsPlugin::keywords() const
{
    QStringList words;
    words << "linewidget" << "boxwidget" << "pointwidget" << "implicitplanewidget"
          << "spherewidget" << "imageplanewidget" << "splinewidget";
    return words;
}
