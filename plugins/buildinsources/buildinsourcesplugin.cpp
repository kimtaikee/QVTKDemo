#include "buildinsourcesplugin.h"
#include "texturewindow.h"

#include <vtkutils/vtkutils.h>
#include <vtkutils/vtkwidget.h>

#include <vtkPlaneSource.h>
#include <vtkCylinderSource.h>
#include <vtkSphereSource.h>
#include <vtkArrowSource.h>
#include <vtkCubeSource.h>
#include <vtkLineSource.h>
#include <vtkConeSource.h>
#include <vtkDiskSource.h>
#include <vtkEarthSource.h>
#include <vtkJPEGReader.h>
#include <vtkPNGReader.h>
#include <vtkTexture.h>
#include <vtkTextureMapToSphere.h>
#include <vtkColorSeries.h>
#include <vtkPlatonicSolidSource.h>
#include <vtkPointSource.h>

#include <QMap>

BuildinSourcePlugin::BuildinSourcePlugin()
{

}

BuildinSourcePlugin::~BuildinSourcePlugin()
{

}

QString BuildinSourcePlugin::category() const
{
    return QObject::tr("Build-in Sources");
}

template <class T>
static inline VtkUtils::VtkWidget* createSourceWidget(const QString& title)
{
    VtkUtils::VtkWidget* vtkwidget = new VtkUtils::VtkWidget;
    vtkwidget->setWindowTitle(title);
    vtkwidget->addActor(VtkUtils::createSourceActor<T>());
    return vtkwidget;
}

static inline QColor color3ubToQColor(const vtkColor3ub& c3ub)
{
    return QColor(c3ub.GetRed(), c3ub.GetGreen(), c3ub.GetBlue());
}

static inline vtkActor* createPlatonicSolidActor(const vtkSmartPointer<vtkPlatonicSolidSource>& source)
{
    VTK_CREATE(vtkPolyDataMapper, mapper);
    mapper->SetInputConnection(source->GetOutputPort());
    vtkActor* actor = vtkActor::New();
    actor->SetMapper(mapper);
    return actor;
}

QList<QWidget*> BuildinSourcePlugin::examples() const
{
    QList<QWidget*> widgetList;

    VTK_CREATE(vtkColorSeries, coolColor);
    coolColor->SetColorScheme(vtkColorSeries::COOL);
    int counter = 0;

    VtkUtils::VtkWidget* widget1 = new VtkUtils::VtkWidget();
    widget1->setWindowTitle(tr("Multiple Sources"));
    widget1->setMultiViewports(true);
    widget1->addActor(VtkUtils::createSourceActor<vtkArrowSource>());
    widget1->addActor(VtkUtils::createSourceActor<vtkCubeSource>());
    widget1->addActor(VtkUtils::createSourceActor<vtkCylinderSource>());
    widget1->addActor(VtkUtils::createSourceActor<vtkDiskSource>());
    widget1->addActor(VtkUtils::createSourceActor<vtkEarthSource>());
    widget1->addActor(VtkUtils::createSourceActor<vtkLineSource>());
    widget1->addActor(VtkUtils::createSourceActor<vtkPlaneSource>());
    widget1->addActor(VtkUtils::createSourceActor<vtkSphereSource>());
    widget1->addActor(VtkUtils::createSourceActor<vtkConeSource>());
    widget1->addActor(VtkUtils::createSourceActor<vtkPointSource>());

    VtkUtils::VtkWidget* widget2 = new VtkUtils::VtkWidget();
    widget2->setMultiViewports(true);
    widget2->setWindowTitle("Platonic Solid Source");
    VTK_CREATE(vtkPlatonicSolidSource, toCube);
    toCube->SetSolidTypeToCube();
    widget2->addActor(createPlatonicSolidActor(toCube));

    VTK_CREATE(vtkPlatonicSolidSource, toDodecahedron);
    toDodecahedron->SetSolidTypeToDodecahedron();
    widget2->addActor(createPlatonicSolidActor(toDodecahedron));

    VTK_CREATE(vtkPlatonicSolidSource, toIcosahedron);
    toIcosahedron->SetSolidTypeToIcosahedron();
    widget2->addActor(createPlatonicSolidActor(toIcosahedron));

    VTK_CREATE(vtkPlatonicSolidSource, toOctahedron);
    toOctahedron->SetSolidTypeToOctahedron();
    widget2->addActor(createPlatonicSolidActor(toOctahedron));

    VTK_CREATE(vtkPlatonicSolidSource, toTetrahedron);
    toTetrahedron->SetSolidTypeToTetrahedron();
    widget2->addActor(createPlatonicSolidActor(toTetrahedron));

    widgetList.append(widget1);
    widgetList.append(widget2);
    widgetList.append(new TextureWindow);

    return widgetList;
}

QStringList BuildinSourcePlugin::keywords() const
{
    QStringList words;
    words << "arrow" << "cube" << "cylinder" << "disk"
          << "earth" << "line" << "plane" << "sphere"
          << "cone" << "texure" << "platonic solid";
    return words;
}
