#ifndef ABSTRACTIMAGEPROCESSOR_H
#define ABSTRACTIMAGEPROCESSOR_H

#include "vtkutils_global.h"

#include <utils/signalledrunable.h>

#include <vtkImageGradient.h>
#include <vtkImageLaplacian.h>
#include <vtkJPEGReader.h>

namespace VtkUtils
{

class VTKUTILS_EXPORT AbstractImageProcessor : public Utils::SignalledRunnable
{
public:
    AbstractImageProcessor();

    void setInputData(vtkImageData* input) { m_imageData = input; }
    vtkImageData* inputData() const { return m_imageData; }

protected:
    vtkImageData* m_imageData = nullptr;
};

template <class T>
class ImageProcessorTempl : public AbstractImageProcessor
{
public:
    ImageProcessorTempl()
    {
        m_algorithm = T::New();
    }

    T* algorithm() const { return m_algorithm; }

    void run()
    {
        if (!m_imageData) {
            emit finished();
            return;
        }

        m_algorithm->SetInputData(m_imageData);
        m_algorithm->Update();
        emit finished();
    }

protected:
    T* m_algorithm = nullptr;
};


} // namespace VtkUtils
#endif // ABSTRACTIMAGEPROCESSOR_H
