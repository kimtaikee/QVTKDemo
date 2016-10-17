#include "mainwidget.h"
#include "ui_mainwidget.h"
#include "filetree.h"

#include <vtkutils/vtkutils.h>
#include <vtkutils/vtkwidget.h>
#include <vtkutils/filereaderutils.h>

#include <vtkPolyDataMapper.h>
#include <vtkPolyDataReader.h>
#include <vtkActor.h>
#include <vtkSmartPointer.h>
#include <vtkPolyData.h>
#include <vtkCylinderSource.h>
#include <vtkDataReader.h>
#include <vtkGenericDataObjectReader.h>
#include <vtkOBJReader.h>
#include <vtkDataSetMapper.h>
#include <vtkMultiBlockDataSet.h>
#include <vtkUnstructuredGrid.h>
#include <vtkPlane.h>

#include <QDebug>
#include <QThreadPool>
#include <QFileInfo>

MainWidget::MainWidget(QWidget *parent) :
    QWidget(parent),
    m_ui(new Ui::MainWidget)
{
    m_fileTree = new FileTree(this);
    m_ui->setupUi(this);
    m_ui->treeWidgetLayout->addWidget(m_fileTree);
    setWindowTitle(tr("Import files & process data"));

    connect(m_fileTree, SIGNAL(importFile(QString)), this, SLOT(onImportFile(QString)));
    connect(m_fileTree, SIGNAL(setCurrentFile(QString)), this, SLOT(onSetCurrentFile(QString)));
    connect(m_fileTree, SIGNAL(filterAdded(QString,int)), this, SLOT(onFilterAdded(QString,int)));
}

MainWidget::~MainWidget()
{
    delete m_ui;
}

void MainWidget::onImportFile(const QString& file)
{
    if (!m_fileVtkWidgetMap.contains(file)) {

        const QFileInfo fi(file);
        const QString ext = fi.suffix().toLower();

        VtkUtils::AbstractFileReader* fileReader = nullptr;

        if (ext == "obj") {
            fileReader = new VtkUtils::ObjFileReader();
            connect(fileReader, SIGNAL(finished()), this, SLOT(onObjFileReaderFinished()));
        } else if (ext == "vtk") {
            fileReader = new VtkUtils::VtkFileReader();
            connect(fileReader, SIGNAL(finished()), this, SLOT(onVtkFileReaderFinished()));
        } else if (ext == "stl") {
            fileReader = new VtkUtils::StlFileReader();
            connect(fileReader, SIGNAL(finished()), this, SLOT(onStlFileReaderFinished()));
        } else if (ext == "cdb") {
            fileReader = new VtkUtils::AnsysFileReader();
            connect(fileReader, SIGNAL(finished()), this, SLOT(onAnsysFileReaderFinished()));
        } else if (ext == "bdf") {
            fileReader = new VtkUtils::NastranFileReader();
            connect(fileReader, SIGNAL(finished()), this, SLOT(onNastranFileReaderFinished()));
        } else if (ext == "cas") {
            fileReader = new VtkUtils::FluentFileReader();
            connect(fileReader, SIGNAL(finished()), this, SLOT(onFluentFileReaderFinished()));
        } else if (ext == "vrml") {

        } else if (ext == "ply") {
            fileReader = new VtkUtils::PlyFileReader();
            connect(fileReader, SIGNAL(finished()), this, SLOT(onPlyFileReaderFinished()));
        } else if (ext == "pdb") {
            fileReader = new VtkUtils::PdbFileReader();
            connect(fileReader, SIGNAL(finished()), this, SLOT(onPdbFileReaderFinished()));
        }

        if (!fileReader) {
            qDebug() << "importFile: null file reader.";
            return;
        }

        fileReader->setFileName(file);
        fileReader->setAutoDelete(false);

        QThreadPool::globalInstance()->start(fileReader);
    }
}

void MainWidget::onSetCurrentFile(const QString& file)
{
    if (!m_fileVtkWidgetMap.contains(file))
        return;

    VtkUtils::VtkWidget* vtkWidget = m_fileVtkWidgetMap.value(file);
    m_ui->stackedWidget->setCurrentWidget(vtkWidget);
}

void MainWidget::onObjFileReaderFinished()
{
    VtkUtils::ObjFileReader* stlReader = qobject_cast<VtkUtils::ObjFileReader*>(sender());
    createWidgetFromData<vtkPolyData, vtkPolyDataMapper>(stlReader->fileName(), stlReader->dataObject());
    stlReader->deleteLater();
}

void MainWidget::onDynaFileReaderFinished()
{

}

void MainWidget::onFluentFileReaderFinished()
{
    VtkUtils::FluentFileReader* fluentReader = qobject_cast<VtkUtils::FluentFileReader*>(sender());
    vtkMultiBlockDataSet* output = vtkMultiBlockDataSet::New();
    output->ShallowCopy(fluentReader->dataObject());

    createWidgetFromData<vtkUnstructuredGrid, vtkDataSetMapper>(fluentReader->fileName(), output->GetBlock(0));
    fluentReader->deleteLater();
}

void MainWidget::onVrmlFileReaderFinished()
{

}

void MainWidget::onStlFileReaderFinished()
{
    VtkUtils::StlFileReader* stlReader = qobject_cast<VtkUtils::StlFileReader*>(sender());
    createWidgetFromData<vtkPolyData, vtkPolyDataMapper>(stlReader->fileName(), stlReader->dataObject());
    stlReader->deleteLater();
}

void MainWidget::onVtkFileReaderFinished()
{
    VtkUtils::VtkFileReader* vtkReader = qobject_cast<VtkUtils::VtkFileReader*>(sender());
    vtkDataSet* dataSet = vtkDataSet::SafeDownCast(vtkReader->dataObject());
    vtkPolyData* polyData = vtkPolyData::SafeDownCast(vtkReader->dataObject());
    if (dataSet)
        createWidgetFromData<vtkDataSet, vtkDataSetMapper>(vtkReader->fileName(), vtkReader->dataObject());
    else if (polyData)
        createWidgetFromData<vtkPolyData, vtkPolyDataMapper>(vtkReader->fileName(), vtkReader->dataObject());
    vtkReader->deleteLater();
}

void MainWidget::onNastranFileReaderFinished()
{
    VtkUtils::NastranFileReader* nastranReader = qobject_cast<VtkUtils::NastranFileReader*>(sender());
    createWidgetFromData<vtkDataSet, vtkDataSetMapper>(nastranReader->fileName(), nastranReader->dataObject());
    nastranReader->deleteLater();
}

void MainWidget::onAnsysFileReaderFinished()
{
    VtkUtils::AnsysFileReader* ansysReader = qobject_cast<VtkUtils::AnsysFileReader*>(sender());
    vtkDataSet* dataSet = vtkDataSet::SafeDownCast(ansysReader->dataObject());
    vtkPolyData* polyData = vtkPolyData::SafeDownCast(ansysReader->dataObject());
    if (dataSet)
        createWidgetFromData<vtkDataSet, vtkDataSetMapper>(ansysReader->fileName(), ansysReader->dataObject());
    else if (polyData)
        createWidgetFromData<vtkPolyData, vtkPolyDataMapper>(ansysReader->fileName(), ansysReader->dataObject());
    ansysReader->deleteLater();
}

void MainWidget::onPlyFileReaderFinished()
{
    VtkUtils::PlyFileReader* plyReader = qobject_cast<VtkUtils::PlyFileReader*>(sender());
    createWidgetFromData<vtkPolyData, vtkPolyDataMapper>(plyReader->fileName(), plyReader->dataObject());
    plyReader->deleteLater();
}

void MainWidget::onPdbFileReaderFinished()
{
    VtkUtils::PdbFileReader* pdbReader = qobject_cast<VtkUtils::PdbFileReader*>(sender());
    createWidgetFromData<vtkPolyData, vtkPolyDataMapper>(pdbReader->fileName(), pdbReader->dataObject());
    pdbReader->deleteLater();
}

void MainWidget::onFilterAdded(const QString& file, int filterType)
{
    vtkDataObject* inputData = m_fileDataObjectMap.value(file);
    qDebug() << "input data: " << inputData;

    VtkUtils::VtkWidget* widget = m_fileVtkWidgetMap.value(file);
    widget->setActorsVisible(false);

//    QScopedPointer<VtkUtils::AbstractDataFilter> filter;
    switch (filterType) {
    case VtkUtils::FT_Clip:
    {
        vtkPolyData* polyData = vtkPolyData::SafeDownCast(inputData);
        VtkUtils::ClipFilter* clip = new VtkUtils::ClipFilter;
        clip->filter()->GenerateClipScalarsOn();

        double origin[3];
        double* center = polyData->GetCenter();
        origin[0] = center[0];
        origin[1] = center[1];
        origin[2] = center[2];

        double normal[3] = {0, 0, 1};

        vtkPlane* plane = vtkPlane::New();
        plane->SetOrigin(center);
        plane->SetNormal(normal);

        clip->filter()->SetClipFunction(plane);
        clip->setInput(polyData);
        clip->run();

        vtkActor* actor = VtkUtils::createActorFromData(clip->output());
        widget->addActor(actor);
    }
        break;

    case VtkUtils::FT_Slice:
    {
        vtkPolyData* polyData = vtkPolyData::SafeDownCast(inputData);
        VtkUtils::SliceFilter* slice = new VtkUtils::SliceFilter;
        slice->filter()->GenerateCutScalarsOn();

        double origin[3];
        double* center = polyData->GetCenter();
        origin[0] = center[0];
        origin[1] = center[1];
        origin[2] = center[2];

        double normal[3] = {0, 0, 1};

        vtkPlane* plane = vtkPlane::New();
        plane->SetOrigin(center);
        plane->SetNormal(normal);

        slice->filter()->SetCutFunction(plane);
        slice->setInput(polyData);
        slice->run();

        vtkActor* actor = VtkUtils::createActorFromData(slice->output());
        widget->addActor(actor);
    }
//        filter.reset(new VtkUtils::SliceFilter);
        break;

    case VtkUtils::FT_Isosurface:
//        filter.reset(new VtkUtils::IsosurfaceFilter);
        break;

    case VtkUtils::FT_Threshold:
//        filter.reset(new VtkUtils::ThresholdFilter);
        break;

    case VtkUtils::FT_Smooth:
//        filter.reset(new VtkUtils::SmoothFilter);
        break;
    }

    widget->update();
//    filter->set
}

template <class DataObject, class Mapper>
void MainWidget::createWidgetFromData(const QString& file, vtkDataObject* dataObj)
{
    if (!dataObj)
        return;

    m_fileDataObjectMap.insert(file, dataObj);

    DataObject* data = DataObject::SafeDownCast(dataObj);

    if (!data) {
        qDebug() << "ansys parsing: null data set.";
        return;
    }

    vtkSmartPointer<Mapper> mapper(Mapper::New());
    mapper->SetInputData(data);
    mapper->SetColorModeToMapScalars();
    mapper->Update();

    vtkActor* actor = vtkActor::New();
    actor->SetMapper(mapper);

    VtkUtils::VtkWidget* vtkWidget = new VtkUtils::VtkWidget(this);
    vtkWidget->setBackgroundColor(QColor(72, 102, 99));
    vtkWidget->addActor(actor);

    m_fileVtkWidgetMap.insert(file, vtkWidget);
    m_ui->stackedWidget->addWidget(vtkWidget);
    m_ui->stackedWidget->setCurrentWidget(vtkWidget);
}



