#include "volumerenderingwindow.h"
#include "ui_volumerenderingwindow.h"

#include <vtkutils/vtkutils.h>
#include <vtkutils/vtkwidget.h>

#include <vtkStructuredPointsReader.h>
#include <vtkRenderer.h>
#include <vtkPiecewiseFunction.h>
#include <vtkColorTransferFunction.h>
#include <vtkVolumeProperty.h>
#include <vtkImageClip.h>
#include <vtkFixedPointVolumeRayCastMapper.h>

#include <QFileDialog>

VolumeRenderingWindow::VolumeRenderingWindow(QWidget *parent) :
    QWidget(parent),
    m_ui(new Ui::VolumeRenderingWindow)
{
    m_ui->setupUi(this);
    setWindowTitle(tr("Volume Rendering"));

    m_vtkWidget = new VtkUtils::VtkWidget(this);
    m_ui->previewLayout->addWidget(m_vtkWidget);
}

VolumeRenderingWindow::~VolumeRenderingWindow()
{
    delete m_ui;
}

void VolumeRenderingWindow::on_openButton_clicked()
{
    QString file = QFileDialog::getOpenFileName(this, tr("Open Volume File"), ".");
    if (file.isEmpty())
        return;

    m_ui->fileEdit->setText(file);

    VTK_CREATE(vtkStructuredPointsReader, reader);
    reader->SetFileName(file.toUtf8().data());
    reader->Update();

    // Create a transfer function mapping scalar value to opacity
    VTK_CREATE(vtkPiecewiseFunction, oTFun);
    oTFun->AddSegment(0, 1.0, 256, 0.1);

    VTK_CREATE(vtkColorTransferFunction, cTFun);
    cTFun->AddRGBPoint(0, 1.0, 1.0, 1.0);
    cTFun->AddRGBPoint(255, 1.0, 1.0, 1.0);

    // Need to crop to actually see minimum intensity
    VTK_CREATE(vtkImageClip, clip);
    clip->SetInputConnection(reader->GetOutputPort());
    clip->SetOutputWholeExtent(0,66,0,66,30,37);
    clip->ClipDataOn();

    VTK_CREATE(vtkVolumeProperty, property);
    property->SetScalarOpacity(oTFun);
    property->SetColor(cTFun);
    property->SetInterpolationTypeToLinear();

    VTK_CREATE(vtkFixedPointVolumeRayCastMapper, mapper);
    mapper->SetBlendModeToMinimumIntensity();
    mapper->SetInputConnection(clip->GetOutputPort());

    VTK_CREATE(vtkVolume, volume);
    volume->SetMapper(mapper);
    volume->SetProperty(property);

    m_vtkWidget->defaultRenderer()->AddViewProp(volume);
}
