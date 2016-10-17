#include "pickactorwindow.h"
#include "ui_pickactorwindow.h"

#include <utils/utils.h>
#include <utils/signalblocker.h>

#include <vtkutils/vtkutils.h>
#include <vtkutils/vtkwidget.h>
#include <vtkutils/moveactorinteractorstyle.h>

#include <vtkActor.h>
#include <vtkConeSource.h>
#include <vtkSphereSource.h>
#include <vtkCubeSource.h>
#include <vtkArrowSource.h>
#include <vtkCylinderSource.h>
#include <vtkProperty.h>
#include <vtkRenderer.h>
#include <vtkCamera.h>
#include <vtkRenderWindow.h>
#include <vtkTextActor.h>
#include <vtkVRMLImporter.h>
#include <vtkVRMLExporter.h>
#include <vtk3DSImporter.h>

#include <QDebug>
#include <QTime>
#include <QFileDialog>
#include <QFileInfo>

static const int PopulateSize = 50;
static const int InitBound = 10;
PickActorWindow::PickActorWindow(QWidget *parent) :
    QWidget(parent),
    m_ui(new Ui::PickActorWindow)
{
    m_ui->setupUi(this);

    m_bounds[0] = -InitBound;
    m_bounds[1] = InitBound;
    m_bounds[2] = -InitBound;
    m_bounds[3] = InitBound;
    m_bounds[4] = -InitBound;
    m_bounds[5] = InitBound;

    Utils::SignalBlocker sb;
    sb.addObject(m_ui->xMinSpinBox);
    sb.addObject(m_ui->xMaxSpinBox);
    sb.addObject(m_ui->yMinSpinBox);
    sb.addObject(m_ui->yMaxSpinBox);
    sb.addObject(m_ui->zMinSpinBox);
    sb.addObject(m_ui->zMaxSpinBox);

    m_ui->xMinSpinBox->setValue(-InitBound);
    m_ui->xMaxSpinBox->setValue(InitBound);
    m_ui->yMinSpinBox->setValue(-InitBound);
    m_ui->yMaxSpinBox->setValue(InitBound);
    m_ui->zMinSpinBox->setValue(-InitBound);
    m_ui->zMaxSpinBox->setValue(InitBound);

    m_colorSeries.setScheme(Utils::ColorSeries::Warm);

    m_vtkWidget = new VtkUtils::VtkWidget(this);
    m_ui->previewLayout->addWidget(m_vtkWidget);

    populate();

    setWindowTitle(tr("Pick Actor"));

    VtkUtils::MoveActorInteractorStyle* style = VtkUtils::MoveActorInteractorStyle::New();
    style->SetDefaultRenderer(m_vtkWidget->defaultRenderer());
    m_vtkWidget->GetRenderWindow()->GetInteractor()->SetInteractorStyle(style);

    m_promptActor = vtkTextActor::New();
    m_promptActor->SetInput("Click and drag the models");
    m_vtkWidget->defaultRenderer()->AddActor(m_promptActor);
}

PickActorWindow::~PickActorWindow()
{
    delete m_ui;
}

void PickActorWindow::on_xMinSpinBox_valueChanged(double arg1)
{
    m_bounds[0] = arg1;
}

void PickActorWindow::on_xMaxSpinBox_valueChanged(double arg1)
{
    m_bounds[1] = arg1;
}

void PickActorWindow::on_yMinSpinBox_valueChanged(double arg1)
{
    m_bounds[2] = arg1;
}

void PickActorWindow::on_zMinSpinBox_valueChanged(double arg1)
{
    m_bounds[3] = arg1;
}

void PickActorWindow::on_yMaxSpinBox_valueChanged(double arg1)
{
    m_bounds[4] = arg1;
}

void PickActorWindow::on_zMaxSpinBox_valueChanged(double arg1)
{
    m_bounds[5] = arg1;
}

void PickActorWindow::on_addButton_clicked()
{
    int sourceType = m_ui->sourceCombo->currentIndex();
    addActor(sourceType);
}

void PickActorWindow::populate()
{
    qsrand(QTime::currentTime().msecsSinceStartOfDay());

    for (int i = 0; i < PopulateSize; ++i)
        addActor(qrand() % 5);
}

double PickActorWindow::randomXPos() const
{
    double range = m_bounds[1] - m_bounds[0];
    return m_bounds[0] + ((double)range / 1000) * (rand() % 1000);
}

double PickActorWindow::randomYPos() const
{
    double range = m_bounds[3] - m_bounds[2];
    return m_bounds[2] + ((double)range / 1000) * (rand() % 1000);
}

double PickActorWindow::randomZPos() const
{
    double range = m_bounds[5] - m_bounds[4];
    return m_bounds[4] + ((double)range / 1000) * (rand() % 1000);
}

void PickActorWindow::addActor(int sourceType)
{
    vtkActor* actor = nullptr;
    switch (sourceType) {
    case 0:
        actor = VtkUtils::createSourceActor<vtkConeSource>();
        break;

    case 1:
        actor = VtkUtils::createSourceActor<vtkSphereSource>();
        break;

    case 2:
        actor = VtkUtils::createSourceActor<vtkCubeSource>();
        break;

    case 3:
        actor = VtkUtils::createSourceActor<vtkArrowSource>();
        break;

    case 4:
        actor = VtkUtils::createSourceActor<vtkCylinderSource>();
        break;
    }

    double clr[3];
    double pos[3] = {randomXPos(), randomYPos(), randomZPos()};
    Utils::vtkColor(m_colorSeries.nextColor(), clr);
    actor->GetProperty()->SetColor(clr);
    actor->SetPosition(pos);

    m_vtkWidget->defaultRenderer()->AddActor(actor);
    m_vtkWidget->update();
}


void PickActorWindow::on_importButton_clicked()
{
    static QString previousDir = ".";
    QString importFile = QFileDialog::getOpenFileName(this, tr("Import File"), previousDir, "VRML Files(*.vrml);;3DS Files(*.3ds)");
    if (importFile.isEmpty())
        return;

    QFileInfo fi(importFile);
    previousDir = fi.canonicalFilePath();
    const QString ext = fi.suffix().toLower();
    m_ui->importFileEdit->setText(importFile);


    vtkSmartPointer<vtkImporter> importer;

    if (ext == "vrml") {
        vtkVRMLImporter* vrmlImporter = vtkVRMLImporter::New();
        vrmlImporter->SetFileName(importFile.toLocal8Bit().data());
        importer = vrmlImporter;
    } else if (ext == "3ds") {
        vtk3DSImporter* threedsImporter = vtk3DSImporter::New();
        threedsImporter->SetFileName(importFile.toLocal8Bit().data());
        importer = threedsImporter;
    }

    if (!importer) {
        qDebug() << "null importer, quit.";
        return;
    }

    importer->SetRenderWindow(m_vtkWidget->GetRenderWindow());
    importer->Read();

    m_vtkWidget->update();
}

void PickActorWindow::on_exportButton_clicked()
{
    static QString previousDir = ".";
    QString exportFile = QFileDialog::getSaveFileName(this, tr("Export VRML File"), previousDir, "VRML Files(*.vrml)");
    if (exportFile.isEmpty())
        return;

    QFileInfo fi(exportFile);
    previousDir = fi.canonicalFilePath();
    m_ui->exportFileEdit->setText(exportFile);

    VTK_CREATE(vtkVRMLExporter, exporter);
    exporter->SetRenderWindow(m_vtkWidget->GetRenderWindow());
    exporter->SetFileName(exportFile.toLocal8Bit().data());
    exporter->Write();
}
