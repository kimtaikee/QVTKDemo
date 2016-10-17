#include "lightplugin.h"
#include "mainwidget.h"
#include "galaxywindow.h"

#include <QMap>

#include <vtkutils/vtkwidget.h>
#include <vtkutils/vtkutils.h>

#include <vtkLight.h>
#include <vtkActor.h>
#include <vtkSphereSource.h>
#include <vtkSmartPointer.h>
#include <vtkLightActor.h>
#include <vtkRenderer.h>
#include <vtkCamera.h>

LightPlugin::LightPlugin()
{

}

LightPlugin::~LightPlugin()
{

}

QString LightPlugin::category() const
{
    return QObject::tr("Light");
}

QList<QWidget*> LightPlugin::examples() const
{
    QList<QWidget*> widgetList;

    VtkUtils::VtkWidget* vtkWidget = new VtkUtils::VtkWidget();
    vtkWidget->setWindowTitle(tr("Light demo"));

    vtkSmartPointer<vtkSphereSource> sphereSrc = vtkSphereSource::New();
    sphereSrc->SetPhiResolution(30);
    sphereSrc->SetThetaResolution(20);
    sphereSrc->Update();

    // Display where the light is
//    vtkSmartPointer<vtkLight> light = vtkSmartPointer<vtkLight>::New();
//    light->SetPositional(true); // without this line, the program crashes
//    vtkSmartPointer<vtkLightActor> lightActor = vtkSmartPointer<vtkLightActor>::New();
//    lightActor->SetLight(light);
//    vtkWidget->defaultRenderer()->AddViewProp(lightActor);

    VTK_CREATE(vtkLight, light1);
    light1->SetColor(0, 1, 0);
    light1->SetPosition(0, 0, 1);
//    light1->SetFocalPoint(vtkWidget->defaultRenderer()->GetActiveCamera()->GetFocalPoint());
    vtkWidget->defaultRenderer()->AddLight(light1);
    vtkWidget->defaultRenderer()->ResetCamera();

    vtkActor* actor = VtkUtils::createSourceActor(sphereSrc.GetPointer());
    vtkWidget->addActor(actor);

    widgetList.append(vtkWidget);
    widgetList.append(new GalaxyWindow);

    return widgetList;
}

QStringList LightPlugin::keywords() const
{
    QStringList words;
    words << "light";
    return words;
}

