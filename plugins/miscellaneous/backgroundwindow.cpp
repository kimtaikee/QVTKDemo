#include "backgroundwindow.h"
#include "ui_backgroundwindow.h"

#include <utils/utils.h>
#include <utils/signalblocker.h>

#include <vtkutils/vtkutils.h>
#include <vtkutils/vtkwidget.h>

#include <vtkSuperquadricSource.h>
#include <vtkActor.h>
#include <vtkRenderer.h>
#include <vtkPolyDataMapper.h>
#include <vtkJPEGReader.h>
#include <vtkPNGReader.h>
#include <vtkBMPReader.h>
#include <vtkImageActor.h>
#include <vtkRenderWindow.h>
#include <vtkCamera.h>
#include <vtkImageData.h>

#include <QFileDialog>
#include <QFileInfo>
#include <QDebug>

BackgroundWindow::BackgroundWindow(QWidget *parent) :
    QWidget(parent),
    m_ui(new Ui::BackgroundWindow)
{
    m_ui->setupUi(this);
    m_vtkWidget = new VtkUtils::VtkWidget(this);
    m_vtkWidget->showOrientationMarker();
    m_ui->previewLayout->addWidget(m_vtkWidget);

    Utils::SignalBlocker sb(m_ui->thetaRoundnessSpinBox);
    sb.addObject(m_ui->phiRoundnessSpinBox);
    sb.addObject(m_ui->thetaResolutionSpinBox);
    sb.addObject(m_ui->phiResolutionSpinBox);
    sb.addObject(m_ui->thicknessSpinBox);

    m_ui->thetaRoundnessSpinBox->setValue(3.1);
    m_ui->phiRoundnessSpinBox->setValue(1.2);
    m_ui->thetaResolutionSpinBox->setValue(16);
    m_ui->phiResolutionSpinBox->setValue(16);
    m_ui->thicknessSpinBox->setValue(0.3333);

    // MUST BE put after setupUi
    setWindowTitle(tr("Background style"));
    m_ui->color2GroupBox->hide();
    m_ui->imageFileGroupBox->hide();

    VtkUtils::vtkInitOnce(m_superquadricSource);
    m_superquadricSource->SetPhiRoundness(3.1);
    m_superquadricSource->SetThetaRoundness(1.2);
    m_superquadricSource->Update();

    VTK_CREATE(vtkPolyDataMapper, mapper);
    mapper->SetInputConnection(m_superquadricSource->GetOutputPort());

    VtkUtils::vtkInitOnce(m_modelActor);
    m_modelActor->SetMapper(mapper);

    double clr[3];
    QColor defaultColor(102, 102, 102);
    Utils::vtkColor(defaultColor, clr);
    m_vtkWidget->defaultRenderer()->AddActor(m_modelActor);
    m_vtkWidget->defaultRenderer()->SetBackground(clr); // set default bg color
    m_ui->colorButton->setCurrentColor(defaultColor);
    m_vtkWidget->showOrientationMarker();
    m_vtkWidget->defaultRenderer()->ResetCamera();

    connect(m_ui->colorButton, SIGNAL(colorChanged(QColor)), this, SLOT(onColorChanged(QColor)));
    connect(m_ui->color2Button, SIGNAL(colorChanged(QColor)), this, SLOT(onColorChanged(QColor)));
}

BackgroundWindow::~BackgroundWindow()
{
    delete m_ui;
}

void BackgroundWindow::on_styleCombo_currentIndexChanged(int index)
{
    switch (index) {
    case 0:
        m_ui->colorButton->show();
        m_ui->color2GroupBox->hide();
        m_ui->imageFileGroupBox->hide();
        m_vtkWidget->defaultRenderer()->SetGradientBackground(0);
        break;

    case 1:
        m_ui->imageFileGroupBox->hide();
        m_ui->color2GroupBox->show();
        m_ui->color1GroupBox->show();
        m_vtkWidget->defaultRenderer()->SetGradientBackground(1);
        break;

    case 2:
        m_ui->color1GroupBox->hide();
        m_ui->color2GroupBox->hide();
        m_ui->imageFileGroupBox->show();
        break;
    }
}

void BackgroundWindow::on_browseButton_clicked()
{
    static QString previousDir = ".";
    static const QString imgFilter = "PNG Files(*.png);;JPG Files(*.jpg);;BMP Files(*.bmp)";
    QString imgFile = QFileDialog::getOpenFileName(this, tr("Select Image"), previousDir, imgFilter);
    if (imgFile.isEmpty())
        return;

    m_ui->fileEdit->setText(imgFile);
    QFileInfo fi(imgFile);
    previousDir = fi.canonicalFilePath();
    const QString ext = fi.suffix().toLower();

    // handle img file
    VTK_CREATE(vtkImageData, imageData);
    if (ext == "jpg") {
        VTK_CREATE(vtkJPEGReader, jpegReader);
        if(!jpegReader->CanReadFile(imgFile.toUtf8().data())) {
            qDebug() << "Error reading file " << imgFile;
            return;
        }

        jpegReader->SetFileName (imgFile.toUtf8().data());
        jpegReader->Update();
        imageData = jpegReader->GetOutput();
    } else if (ext == "png") {
        VTK_CREATE(vtkPNGReader, pngReader);
        if(!pngReader->CanReadFile(imgFile.toUtf8().data())) {
            qDebug() << "Error reading file " << imgFile;
            return;
        }

        pngReader->SetFileName (imgFile.toUtf8().data());
        pngReader->Update();
        imageData = pngReader->GetOutput();
    } else if (ext == "bmp") {
        VTK_CREATE(vtkBMPReader, bmpReader);
        if(!bmpReader->CanReadFile(imgFile.toUtf8().data())) {
            qDebug() << "Error reading file " << imgFile;
            return;
        }

        bmpReader->SetFileName (imgFile.toUtf8().data());
        bmpReader->Update();
        imageData = bmpReader->GetOutput();
    } else {
        return;
    }

    // Create an image actor to display the image
    VtkUtils::vtkInitOnce(m_imageActor);
    m_imageActor->SetInputData(imageData);

    // Create a renderer to display the image in the background
    VtkUtils::vtkInitOnce(m_bgRenderer);

    // a background layer and a foreground layer
    m_bgRenderer->SetLayer(0);
    m_bgRenderer->InteractiveOff();
    m_bgRenderer->AddActor(m_imageActor);
    m_vtkWidget->defaultRenderer()->SetLayer(1);
    m_vtkWidget->GetRenderWindow()->SetNumberOfLayers(2);
    m_vtkWidget->GetRenderWindow()->AddRenderer(m_bgRenderer);

    double origin[3];
    double spacing[3];
    int extent[6];
    imageData->GetOrigin( origin );
    imageData->GetSpacing( spacing );
    imageData->GetExtent( extent );

    vtkCamera* camera = m_bgRenderer->GetActiveCamera();
    camera->ParallelProjectionOn();

    double xc = origin[0] + 0.5*(extent[0] + extent[1])*spacing[0];
    double yc = origin[1] + 0.5*(extent[2] + extent[3])*spacing[1];
//    double xd = (extent[1] - extent[0] + 1)*spacing[0];
    double yd = (extent[3] - extent[2] + 1)*spacing[1];
    double d = camera->GetDistance();
    camera->SetParallelScale(0.5*yd);
    camera->SetFocalPoint(xc,yc,0.0);
    camera->SetPosition(xc,yc,d);
}

void BackgroundWindow::onColorChanged(const QColor &clr)
{
    Widgets::ColorPushButton* button = qobject_cast<Widgets::ColorPushButton*>(sender());
    double vtkClr[3];
    Utils::vtkColor(clr, vtkClr);

    if (button == m_ui->colorButton)
        m_vtkWidget->defaultRenderer()->SetBackground(vtkClr);
    else if (button == m_ui->color2Button)
        m_vtkWidget->defaultRenderer()->SetBackground2(vtkClr);

    m_vtkWidget->update();
}

void BackgroundWindow::on_thetaResolutionSpinBox_valueChanged(int arg1)
{
    m_superquadricSource->SetThetaResolution(arg1);
    m_superquadricSource->Update();
    m_vtkWidget->update();
}

void BackgroundWindow::on_phiResolutionSpinBox_valueChanged(int arg1)
{
    m_superquadricSource->SetPhiResolution(arg1);
    m_superquadricSource->Update();
    m_vtkWidget->update();
}

void BackgroundWindow::on_thetaRoundnessSpinBox_valueChanged(double arg1)
{
    m_superquadricSource->SetThetaRoundness(arg1);
    m_superquadricSource->Update();
    m_vtkWidget->update();
}

void BackgroundWindow::on_phiRoundnessSpinBox_valueChanged(double arg1)
{
    m_superquadricSource->SetPhiRoundness(arg1);
    m_superquadricSource->Update();
    m_vtkWidget->update();
}

void BackgroundWindow::on_thicknessSpinBox_valueChanged(double arg1)
{
    m_superquadricSource->SetThickness(arg1);
    m_superquadricSource->Update();
    m_vtkWidget->update();
}

void BackgroundWindow::on_shapeComboBox_currentIndexChanged(int index)
{
    m_superquadricSource->SetToroidal(index == 1);
    m_superquadricSource->Update();
    m_vtkWidget->update();
}
