#ifndef ABSTRACTCONTOURGENERATOR_H
#define ABSTRACTCONTOURGENERATOR_H

#include <utils/signalledrunable.h>

class vtkPolyData;
class AbstractContourGeneratorPrivate;
class AbstractContourGenerator : public Utils::SignalledRunnable
{
    Q_OBJECT
public:

    enum Type { Smooth, Banded };

    AbstractContourGenerator();
    virtual ~AbstractContourGenerator();

    void setPolyData(vtkPolyData* polyData);
    vtkPolyData* polyData();

    void setNumberOfContours(int number);
    int numberOfContours() const;

    void setGenerateLines(bool gen);
    bool generateLines() const;

    void setGenerateLabels(bool gen);
    bool generateLabels() const;

    void run();

    virtual int type() const = 0;

    vtkPolyData* contourData() const;
    vtkPolyData* contourLinesData() const;
    vtkPolyData* contourLabelsData() const;

signals:
    void contourReady(vtkPolyData* data);
    void contourLinesReady(vtkPolyData* data);
    void contourLabelsReady(vtkPolyData* data);

protected:
    virtual void createContour() = 0;

    void setContourData(vtkPolyData* data);
    void setContourLinesData(vtkPolyData* data);
    void setContourLabelsData(vtkPolyData* data);

private:
    AbstractContourGeneratorPrivate* d_ptr;
    Q_DISABLE_COPY(AbstractContourGenerator)
};

#endif // ABSTRACTCONTOURGENERATOR_H
