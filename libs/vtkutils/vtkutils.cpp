#include "vtkutils.h"
#include "actorexporter.h"

#include <vtkDiskSource.h>
#include <vtkPlaneSource.h>
#include <vtkActor.h>
#include <vtkPolyDataMapper.h>
#include <vtkSphereSource.h>
#include <vtkQImageToImageSource.h>
#include <vtkImageData.h>
#include <vtkDelaunay2D.h>
#include <vtkPoints.h>
#include <vtkPointData.h>
#include <vtkPolyData.h>
#include <vtkDoubleArray.h>
#include <vtkIntArray.h>
#include <vtkRenderWindow.h>
#include <vtkVRMLExporter.h>
#include <vtkRenderer.h>

#include <QVTKWidget.h>

#include <QDebug>
#include <QThreadPool>

namespace VtkUtils
{

QImage vtkImageDataToQImage(vtkImageData* imageData)
{
    if (!imageData)
        return QImage();

    /// \todo retrieve just the UpdateExtent
    int width = imageData->GetDimensions()[0];
    int height = imageData->GetDimensions()[1];
    QImage image(width, height, QImage::Format_RGB32);
    QRgb* rgbPtr = reinterpret_cast<QRgb*>(image.bits()) + width * (height-1);
    unsigned char* colorsPtr = reinterpret_cast<unsigned char*>(
                imageData->GetScalarPointer());
    // mirror vertically
    for(int row = 0; row < height; ++row) {
        for (int col = 0; col < width; ++col) {
            // Swap rgb
            *(rgbPtr++) = QColor(colorsPtr[0], colorsPtr[1], colorsPtr[2]).rgb();
            colorsPtr +=  3;
        }
        rgbPtr -= width * 2;
    }
    return image;
}

void qImageToVtkImage(QImage& img, vtkImageData* imageData)
{
    if (!imageData) {
        qDebug() << "null image data.";
        return;
    }

    vtkSmartPointer<vtkQImageToImageSource> qimageToImageSource = vtkSmartPointer<vtkQImageToImageSource>::New();
    qimageToImageSource->SetQImage(&img);
    qimageToImageSource->Update();
    imageData->DeepCopy(qimageToImageSource->GetOutput());
}

QImage vtkWidgetSnapshot(QVTKWidget* widget)
{
    if (!widget)
        return QImage();
    return vtkImageDataToQImage(widget->cachedImage());
}

void exportActorToFile(vtkActor* actor, const QString& outfile)
{
    VtkUtils::ActorExporter* exporter = new VtkUtils::ActorExporter(actor, outfile);
    QThreadPool::globalInstance()->start(exporter);
}

}
