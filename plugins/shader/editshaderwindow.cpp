#include "editshaderwindow.h"
#include "ui_editshaderwindow.h"

#include <vtkutils/vtkwidget.h>
#include <vtkutils/vtkutils.h>
#include <vtkutils/filereaderutils.h>

#include <vtkActor.h>
#include <vtkRenderer.h>
#include <vtkPolyDataMapper.h>
#include <vtkDataSet.h>
#include <vtkDataSetMapper.h>
#include <vtkUnstructuredGrid.h>
#include <vtkShader2.h>
#include <vtkShaderProgram2.h>
#include <vtkShader2Collection.h>
#include <vtkOpenGLProperty.h>

#include <QFileInfo>
#include <QThreadPool>
#include <QFileDialog>
#include <QDebug>

EditShaderWindow::EditShaderWindow(QWidget *parent) :
    QWidget(parent),
    m_ui(new Ui::EditShaderWindow)
{
    m_ui->setupUi(this);
    setWindowTitle(tr("Edit Shader"));

    m_vtkWidget = new VtkUtils::VtkWidget(this);
    m_ui->previewLayout->addWidget(m_vtkWidget);
}

EditShaderWindow::~EditShaderWindow()
{
    delete m_ui;
}

void EditShaderWindow::on_importButton_clicked()
{
    static QString previousDir = ".";
    QString filter = "ANSYS Files(*.obj);;VTK Files(*.vtk);;STL Files(*.stl);;ANSYS Files(*.cdb);;"
                     "Fluent Files(*.cas);;PLY Files(*.ply);;PDB Files(*.pdb)";
    QString file = QFileDialog::getOpenFileName(this, tr("Import File"), previousDir, filter);
    if (file.isEmpty())
        return;

    m_ui->importFileEdit->setText(file);

    QFileInfo fi(file);
    previousDir = fi.canonicalPath();
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

void EditShaderWindow::on_applyShaderButton_clicked()
{
    QString fragmentShaderSource(m_ui->shaderProgramEdit->toPlainText());
    qDebug() << "shader program: " << fragmentShaderSource;

    //The GLSL binded program
    VTK_CREATE(vtkShaderProgram2, program);

    // Prepare the fragment shader
    VTK_CREATE(vtkShader2, shader);
    shader->SetType(VTK_SHADER_TYPE_FRAGMENT);
    shader->SetSourceCode(fragmentShaderSource.toStdString().c_str());
    program->GetShaders()->AddItem(shader);

    vtkSmartPointer<vtkOpenGLProperty> openGLproperty =
            static_cast<vtkOpenGLProperty*>(m_modelActor->GetProperty());
    openGLproperty->SetPropProgram(program);
    openGLproperty->ShadingOn();
    m_vtkWidget->update();
}

void EditShaderWindow::onObjFileReaderFinished()
{
    VtkUtils::ObjFileReader* stlReader = qobject_cast<VtkUtils::ObjFileReader*>(sender());
    createActorFromData<vtkPolyData, vtkPolyDataMapper>(stlReader->dataObject());
    stlReader->deleteLater();
}

void EditShaderWindow::onDynaFileReaderFinished()
{

}

void EditShaderWindow::onFluentFileReaderFinished()
{
    VtkUtils::FluentFileReader* fluentReader = qobject_cast<VtkUtils::FluentFileReader*>(sender());
    vtkMultiBlockDataSet* output = vtkMultiBlockDataSet::New();
    output->ShallowCopy(fluentReader->dataObject());

    createActorFromData<vtkUnstructuredGrid, vtkDataSetMapper>(output->GetBlock(0));
    fluentReader->deleteLater();
}

void EditShaderWindow::onVrmlFileReaderFinished()
{

}

void EditShaderWindow::onStlFileReaderFinished()
{
    VtkUtils::StlFileReader* stlReader = qobject_cast<VtkUtils::StlFileReader*>(sender());
    createActorFromData<vtkPolyData, vtkPolyDataMapper>(stlReader->dataObject());
    stlReader->deleteLater();
}

void EditShaderWindow::onVtkFileReaderFinished()
{
    VtkUtils::VtkFileReader* vtkReader = qobject_cast<VtkUtils::VtkFileReader*>(sender());
    vtkDataSet* dataSet = vtkDataSet::SafeDownCast(vtkReader->dataObject());
    vtkPolyData* polyData = vtkPolyData::SafeDownCast(vtkReader->dataObject());
    if (dataSet)
        createActorFromData<vtkDataSet, vtkDataSetMapper>(vtkReader->dataObject());
    else if (polyData)
        createActorFromData<vtkPolyData, vtkPolyDataMapper>(vtkReader->dataObject());
    vtkReader->deleteLater();
}

void EditShaderWindow::onNastranFileReaderFinished()
{
    VtkUtils::NastranFileReader* nastranReader = qobject_cast<VtkUtils::NastranFileReader*>(sender());
    createActorFromData<vtkDataSet, vtkDataSetMapper>(nastranReader->dataObject());
    nastranReader->deleteLater();
}

void EditShaderWindow::onAnsysFileReaderFinished()
{
    VtkUtils::AnsysFileReader* ansysReader = qobject_cast<VtkUtils::AnsysFileReader*>(sender());
    vtkDataSet* dataSet = vtkDataSet::SafeDownCast(ansysReader->dataObject());
    vtkPolyData* polyData = vtkPolyData::SafeDownCast(ansysReader->dataObject());
    if (dataSet)
        createActorFromData<vtkDataSet, vtkDataSetMapper>(ansysReader->dataObject());
    else if (polyData)
        createActorFromData<vtkPolyData, vtkPolyDataMapper>(ansysReader->dataObject());
    ansysReader->deleteLater();
}

void EditShaderWindow::onPlyFileReaderFinished()
{
    VtkUtils::PlyFileReader* plyReader = qobject_cast<VtkUtils::PlyFileReader*>(sender());
    createActorFromData<vtkPolyData, vtkPolyDataMapper>(plyReader->dataObject());
    plyReader->deleteLater();
}

void EditShaderWindow::onPdbFileReaderFinished()
{
    VtkUtils::PdbFileReader* plyReader = qobject_cast<VtkUtils::PdbFileReader*>(sender());
    createActorFromData<vtkPolyData, vtkPolyDataMapper>(plyReader->dataObject());
    plyReader->deleteLater();
}


template <class DataObject, class Mapper>
void EditShaderWindow::createActorFromData(vtkDataObject* dataObj)
{
    if (!dataObj)
        return;

    DataObject* data = DataObject::SafeDownCast(dataObj);

    if (!data) {
        qDebug() << "ansys parsing: null data set.";
        return;
    }

    vtkSmartPointer<Mapper> mapper(Mapper::New());
    mapper->SetInputData(data);
    mapper->Update();

    VtkUtils::vtkInitOnce(m_modelActor);
    m_modelActor->SetMapper(mapper);

    m_vtkWidget->defaultRenderer()->AddActor(m_modelActor);
    m_vtkWidget->defaultRenderer()->ResetCamera();
    m_vtkWidget->update();
}

void EditShaderWindow::on_clear_clicked()
{
    m_ui->shaderProgramEdit->clear();
}
