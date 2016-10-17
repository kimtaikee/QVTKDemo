#include "mainwidget.h"
#include "imagetree.h"
#include "ui_mainwidget.h"

#include <widgets/imagecompareboard.h>

#include <vtkutils/vtkutils.h>
#include <vtkutils/abstractimageprocessor.h>

#include <vtkJPEGReader.h>
#include <vtkPNGReader.h>
#include <vtkBMPReader.h>
#include <vtkImageReader2.h>
#include <vtkSmartPointer.h>
#include <vtkImageGradient.h>
#include <vtkExtractVOI.h>
#include <vtkImageGaussianSmooth.h>
#include <vtkImageThreshold.h>
#include <vtkImageButterworthLowPass.h>
#include <vtkImageButterworthHighPass.h>
#include <vtkImageData.h>

#include <QHeaderView>
#include <QFileInfo>
#include <QThreadPool>

MainWidget::MainWidget(QWidget *parent) :
    QWidget(parent),
    m_ui(new Ui::MainWidget)
{
    m_imageTree = new ImageTree(this);
    m_imageTree->setAlternatingRowColors(true);
    m_ui->setupUi(this);
    m_ui->treeLayout->addWidget(m_imageTree);
    m_imageTree->header()->hide();
    setWindowTitle(tr("Images processing"));

    connect(m_imageTree, SIGNAL(imageAdded(QString)), this, SLOT(onImageAdded(QString)));
    connect(m_imageTree, SIGNAL(imageChanged(QString)), this, SLOT(onImageChanged(QString)));
}

MainWidget::~MainWidget()
{
    delete m_ui;
}

void MainWidget::onImageAdded(const QString& image)
{
    if (!m_imageBoardMap.contains(image)) {
        Widgets::ImageCompareBoard* board = new Widgets::ImageCompareBoard(this);
        m_imageBoardMap.insert(image, board);
        m_ui->stackedWidget->addWidget(board);
        m_ui->stackedWidget->setCurrentWidget(board);
        board->show();

        QPixmap pixmap(image);
        QImage img = pixmap.toImage();
        board->setOriginalImage(img);

        QFileInfo fi(image);
        const QString ext = fi.completeSuffix().toLower();

        vtkSmartPointer<vtkImageReader2> reader;
        if (ext == "jpg")
            reader = vtkJPEGReader::New();
        else if (ext == "png")
            reader = vtkPNGReader::New();

        if (reader.Get()) {
            reader->SetFileName(image.toUtf8().data());
            reader->Update();

//            int dims[3];
//            reader->GetOutput()->GetDimensions(dims);

//            vtkSmartPointer<vtkExtractVOI> extractVOI =
//            vtkSmartPointer<vtkExtractVOI>::New();

//            extractVOI->SetInputConnection(reader->GetOutputPort());
//            extractVOI->SetVOI(dims[0]/4.,3.*dims[0]/4.,dims[1]/4.,3.*dims[1]/4., 0, 0);

//            vtkSmartPointer<vtkImageGradient> imageGradient = vtkImageGradient::New();
//            imageGradient->SetDimensionality(2);
//            imageGradient->SetHandleBoundaries(true);
//            imageGradient->SetInputConnection(reader->GetOutputPort());
//            imageGradient->Update();

            vtkImageGaussianSmooth* pGaussian = vtkImageGaussianSmooth::New();
            pGaussian->SetInputData(reader->GetOutput());
            pGaussian->SetDimensionality(2);
            pGaussian->SetStandardDeviations(1, 1);
            pGaussian->Update();

//            vtkImageThreshold* threshold = vtkImageThreshold::New();
//            threshold->SetInputConnection(reader->GetOutputPort());
//            threshold->ThresholdByUpper(100);
//            threshold->SetInValue(255);
//            threshold->SetOutValue(0);
//            threshold->Update();

            vtkImageButterworthHighPass* lowPass = vtkImageButterworthHighPass::New();
            lowPass->SetInputConnection(reader->GetOutputPort());
            lowPass->SetXCutOff(0.1);
            lowPass->SetYCutOff(0.1);
            lowPass->Update();

            QImage convertedImage = VtkUtils::vtkImageDataToQImage(pGaussian->GetOutput());
            board->setComparedImage(convertedImage);
        }
    }
}

void MainWidget::onImageChanged(const QString& image)
{
    if (m_imageBoardMap.contains(image)) {
        auto widget = m_imageBoardMap.value(image);
        m_ui->stackedWidget->setCurrentWidget(widget);
    }
}


