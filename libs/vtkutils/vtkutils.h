#ifndef VTKUTILS_H
#define VTKUTILS_H

#include <vtkSmartPointer.h>
#include <vtkPolyDataMapper.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkDoubleArray.h>
#include <vtkDelaunay2D.h>
#include <vtkActor.h>
#include <vtkPointData.h>
#include <vtkSphereSource.h>
#include <vtkConeSource.h>
#include <vtkCylinderSource.h>
#include <vtkCubeSource.h>

#include <utils/utils.h>
#include <utils/point3f.h>
#include <utils/vector4f.h>

#include "vtkutils_global.h"

// macroes
#ifndef VTK_CREATE
#define VTK_CREATE(TYPE, NAME) vtkSmartPointer<TYPE> NAME = vtkSmartPointer<TYPE>::New()
#endif

class vtkImageData;
class QVTKWidget;
class vtkPolyDataAlgorithm;
namespace VtkUtils
{

QImage VTKUTILS_EXPORT vtkImageDataToQImage(vtkImageData* imageData);
void VTKUTILS_EXPORT qImageToVtkImage(QImage& img, vtkImageData* imageData);
QImage VTKUTILS_EXPORT vtkWidgetSnapshot(QVTKWidget* widget);

template <class T>
void vtkInitOnce(T** obj)
{
    if (*obj == nullptr)
        *obj = T::New();
}

template <class T>
void vtkInitOnce(vtkSmartPointer<T>& obj)
{
    if (!obj)
        obj = vtkSmartPointer<T>::New();
}

template <class T>
void vtkSafeDelete(T* obj)
{
    if (obj)
        obj->Delete();
}

static inline vtkActor* createSourceActor(vtkPolyDataAlgorithm* algo)
{
    vtkActor* actor = vtkActor::New();
    vtkSmartPointer<vtkPolyDataMapper> mapper(vtkPolyDataMapper::New());
    mapper->SetInputConnection(algo->GetOutputPort());
    actor->SetMapper(mapper);

    return actor;
}

template <class T>
class SourceSetter
{
public:
    void config(T* source)
    {
        Q_UNUSED(source)
        // no impl
    }
};

template <>
class SourceSetter<vtkSphereSource>
{
public:
    void config(vtkSphereSource* source)
    {

    }
};

template <>
class SourceSetter<vtkConeSource>
{
public:
    void config(vtkConeSource* source)
    {

    }
};

template <class T>
static inline vtkActor* createSourceActor(SourceSetter<T>* setter = nullptr)
{
    vtkActor* actor = vtkActor::New();

    vtkSmartPointer<T> source(T::New());

    if (setter)
        setter->config(source);

    vtkSmartPointer<vtkPolyDataMapper> mapper(vtkPolyDataMapper::New());
    mapper->SetInputConnection(source->GetOutputPort());
    actor->SetMapper(mapper);

    return actor;
}

static vtkActor* createSurfaceActor(const QList<Utils::Point3F>& points, const QList<qreal>& scalars = QList<qreal>())
{
    vtkSmartPointer<vtkPoints> vtkpoints(vtkPoints::New());
    foreach (const Utils::Point3F& p3f, points)
        vtkpoints->InsertNextPoint(p3f.x, p3f.y, p3f.z);

    vtkSmartPointer<vtkPolyData> polyData(vtkPolyData::New());
    polyData->SetPoints(vtkpoints);

    vtkSmartPointer<vtkDoubleArray> scalarArray(vtkDoubleArray::New());
    scalarArray->SetName("scalar");
    foreach (qreal scalar, scalars)
        scalarArray->InsertNextTuple1(scalar);

//    vtkSmartPointer<vtkPointData> pointdata(vtkPointData::New());
//    pointdata->SetScalars(dataArray);
    vtkpoints->SetData(scalarArray);

    vtkSmartPointer<vtkDelaunay2D> del(vtkDelaunay2D::New());
    del->SetInputData(polyData);
    del->Update();

    vtkSmartPointer<vtkPolyDataMapper> mapper(vtkPolyDataMapper::New());
    mapper->SetInputConnection(del->GetOutputPort());

    vtkActor* actor = vtkActor::New();
    actor->SetMapper(mapper);

    return actor;
}

template <class DataObject, class Mapper = vtkPolyDataMapper>
static inline vtkActor* createActorFromData(DataObject* data)
{
    vtkActor* actor = vtkActor::New();

    VTK_CREATE(Mapper, mapper);
    mapper->SetInputData(data);
    actor->SetMapper(mapper);

    return actor;
}

void VTKUTILS_EXPORT exportActorToFile(vtkActor* actor, const QString& outfile);

}
#endif // VTKUTILS_H
