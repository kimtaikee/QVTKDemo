#include "texturewindow.h"
#include "ui_texturewindow.h"

#include <vtkutils/vtkutils.h>
#include <vtkutils/vtkwidget.h>

#include <vtkJPEGReader.h>
#include <vtkPNGReader.h>
#include <vtkBMPReader.h>
#include <vtkTexture.h>
#include <vtkTextureMapToSphere.h>
#include <vtkTextureMapToPlane.h>
#include <vtkTextureMapToCylinder.h>
#include <vtkSphereSource.h>
#include <vtkSmartPointer.h>
#include <vtkImageData.h>
#include <vtkPlaneSource.h>
#include <vtkCylinderSource.h>
#include <vtkParametricRandomHills.h>
#include <vtkParametricFunctionSource.h>
#include <vtkRenderer.h>

#include <QFileDialog>
#include <QFileInfo>
#include <QDebug>

static const int TextureIdRole = Qt::UserRole + 211;

TextureWindow::TextureWindow(QWidget *parent) :
    QWidget(parent),
    m_ui(new Ui::TextureWindow)
{
    m_ui->setupUi(this);
    m_ui->previewLayout->setContentsMargins(2, 2, 2, 2);

    m_ui->textureList->setViewMode(QListWidget::IconMode);
    m_ui->textureList->setIconSize(QSize(120,150));
    m_ui->textureList->setResizeMode(QListWidget::Adjust);

    m_vtkWidget = new VtkUtils::VtkWidget(this);
    m_ui->previewLayout->addWidget(m_vtkWidget);
    setWindowTitle(QObject::tr("Texture Demo"));
    useDefaultTexture();
    loadResTextures();
}

TextureWindow::~TextureWindow()
{
    delete m_ui;
}

void TextureWindow::on_pushButton_clicked()
{
    QString textureFile = QFileDialog::getOpenFileName(this, tr("Open texture file"), ".");
    if (textureFile.isEmpty())
        return;

    m_ui->texturePathEdit->setText(textureFile);

    QFileInfo fi(textureFile);
    const QString ext = fi.suffix().toLower();

    VtkUtils::vtkInitOnce(m_actor);

    VTK_CREATE(vtkSphereSource, sphereSource);
    sphereSource->SetThetaResolution(30);
    sphereSource->SetPhiResolution(30);
    sphereSource->Update();

    vtkSmartPointer<vtkImageReader2> reader;

    if (ext == "png")
        reader = vtkPNGReader::New();
    else if (ext == "jpg")
        reader = vtkJPEGReader::New();
    else if (ext == "bmp")
        reader = vtkBMPReader::New();

    if (!reader.GetPointer()) {
        qDebug() << "Read texture: null reader.";
        return;
    }

    reader->SetFileName(textureFile.toUtf8().data());
    reader->Update();
    useImageData(reader->GetOutput());
}

void TextureWindow::useDefaultTexture()
{
    QFile textureFile(":earthclouds");
    if (!textureFile.open(QIODevice::ReadOnly)) {
        qDebug() << "failed to read default texture file.";
        return;
    }

    QImage textureImg = QImage::fromData(textureFile.readAll());

    VtkUtils::vtkInitOnce(m_actor);

    vtkImageData* imgData = vtkImageData::New();
    VtkUtils::qImageToVtkImage(textureImg, imgData);
    useImageData(imgData);
    imgData->Delete();
}

void TextureWindow::useImageData(vtkImageData *imageData)
{
    if (!imageData)
        return;

    m_currImageData = imageData;

    VTK_CREATE(vtkTexture, texture);
    texture->SetInputData(imageData);
    texture->Update();

    VTK_CREATE(vtkPolyDataMapper, mapper);

    if (m_type == Sphere) {
        VTK_CREATE(vtkSphereSource, sphereSource);
        sphereSource->SetThetaResolution(30);
        sphereSource->SetPhiResolution(30);
        sphereSource->Update();

        VTK_CREATE(vtkTextureMapToSphere, mapToSphere);
        mapToSphere->SetInputData(sphereSource->GetOutput());
        mapToSphere->PreventSeamOn();
        mapper->SetInputConnection(mapToSphere->GetOutputPort());
    } else if (m_type == Plane) {
        VTK_CREATE(vtkPlaneSource, plane);
        plane->Update();

        VTK_CREATE(vtkTextureMapToPlane, mapToPlane);
        mapToPlane->SetInputData(plane->GetOutput());
        mapper->SetInputConnection(mapToPlane->GetOutputPort());
    } else if (m_type == Cylinder) {
        VTK_CREATE(vtkCylinderSource, cylinder);
        cylinder->Update();

        VTK_CREATE(vtkTextureMapToCylinder, mapToCylinder);
        mapToCylinder->SetInputData(cylinder->GetOutput());
        mapper->SetInputConnection(mapToCylinder->GetOutputPort());
    } else if (m_type == Surface) {
        VTK_CREATE(vtkParametricFunctionSource, parametricFunctionSource);
        VTK_CREATE(vtkParametricRandomHills, randomHills);
        parametricFunctionSource->SetParametricFunction(randomHills);
        parametricFunctionSource->Update();

        VTK_CREATE(vtkTextureMapToPlane, mapToPlane);
        mapToPlane->SetInputData(parametricFunctionSource->GetOutput());
        mapper->SetInputConnection(mapToPlane->GetOutputPort());
    }

    m_actor->SetMapper(mapper);
    m_actor->SetTexture(texture);

    m_vtkWidget->addActor(m_actor);
    m_vtkWidget->showOrientationMarker();
}

static inline QListWidgetItem* textureItem(const QString& texture, const QString& text)
{
    QListWidgetItem* item = new QListWidgetItem;
    item->setIcon(QIcon(texture));
    item->setData(TextureIdRole, texture);
    item->setText(text);
    return item;
}

void TextureWindow::loadResTextures()
{
//    m_ui->textureList->addItem(textureItem(":earthclouds"), "Earth Clouds"));
    m_ui->textureList->addItem(textureItem(":texture_earth_clouds", "Earth Clouds"));
    m_ui->textureList->addItem(textureItem(":texture_earth_night", "Earth Night"));
    m_ui->textureList->addItem(textureItem(":texture_earth_surface", "Earth Surface"));
    m_ui->textureList->addItem(textureItem(":texture_jupiter", "Jupiter"));
    m_ui->textureList->addItem(textureItem(":texture_mars", "Mars"));
    m_ui->textureList->addItem(textureItem(":texture_mercury", "Mercury"));
    m_ui->textureList->addItem(textureItem(":texture_moon", "Moon"));
    m_ui->textureList->addItem(textureItem(":texture_neptune", "Neptune"));
    m_ui->textureList->addItem(textureItem(":texture_saturn", "Saturn"));
    m_ui->textureList->addItem(textureItem(":texture_sun", "Sun"));
    m_ui->textureList->addItem(textureItem(":texture_uranus", "Uranus"));
    m_ui->textureList->addItem(textureItem(":texture_venus_atmosphere", "Venus Atmosphere"));
    m_ui->textureList->addItem(textureItem(":texture_venus_surface", "Venus Surface"));
    m_ui->textureList->addItem(textureItem(":texture_pluto", "Pluto"));
    m_ui->textureList->addItem(textureItem(":texture_terrian", "Terrian"));
    m_ui->textureList->addItem(textureItem(":texture_rocky_desert", "Rocky Desert"));
    m_ui->textureList->addItem(textureItem(":texture_sand", "Sand"));
    m_ui->textureList->addItem(textureItem(":texture_desert_smf", "Desert SMF"));
}

void TextureWindow::on_textureList_itemClicked(QListWidgetItem *item)
{
    QImage image;
    image.load(item->data(TextureIdRole).toString());

    VTK_CREATE(vtkImageData, vtkImgData);
    VtkUtils::qImageToVtkImage(image, vtkImgData);
    useImageData(vtkImgData);
}

void TextureWindow::on_shapeTypeCombo_currentIndexChanged(int index)
{
    m_type = static_cast<Type>(index);
    useImageData(m_currImageData);
}
