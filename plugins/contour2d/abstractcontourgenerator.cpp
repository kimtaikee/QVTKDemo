#include "abstractcontourgenerator.h"

#include <vtkSmartPointer.h>
#include <vtkPolyData.h>

class AbstractContourGeneratorPrivate
{
public:
    vtkSmartPointer<vtkPolyData> polyData;
    vtkSmartPointer<vtkPolyData> contourData;
    vtkSmartPointer<vtkPolyData> contourLinesData;
    vtkSmartPointer<vtkPolyData> contourLabelsData;

    int numberOfContours = 10;
    bool generateLines = true;
    bool generateLabels = true;
};

AbstractContourGenerator::AbstractContourGenerator()
{
    d_ptr = new AbstractContourGeneratorPrivate;
}

AbstractContourGenerator::~AbstractContourGenerator()
{
    delete d_ptr;
}

void AbstractContourGenerator::setPolyData(vtkPolyData* polyData)
{
    d_ptr->polyData = polyData;
}

vtkPolyData* AbstractContourGenerator::polyData()
{
    return d_ptr->polyData;
}

void AbstractContourGenerator::setNumberOfContours(int number)
{
    d_ptr->numberOfContours = number;
}

int AbstractContourGenerator::numberOfContours() const
{
    return d_ptr->numberOfContours;
}

void AbstractContourGenerator::setGenerateLines(bool gen)
{
    d_ptr->generateLines = gen;
}

bool AbstractContourGenerator::generateLines() const
{
    return d_ptr->generateLines;
}

void AbstractContourGenerator::setGenerateLabels(bool gen)
{
    d_ptr->generateLabels = gen;
}

bool AbstractContourGenerator::generateLabels() const
{
    return d_ptr->generateLabels;
}

void AbstractContourGenerator::run()
{
    createContour();
}

vtkPolyData* AbstractContourGenerator::contourData() const
{
    return d_ptr->contourData;
}

vtkPolyData* AbstractContourGenerator::contourLinesData() const
{
    return d_ptr->contourLinesData;
}

vtkPolyData* AbstractContourGenerator::contourLabelsData() const
{
    return d_ptr->contourLabelsData;
}

void AbstractContourGenerator::setContourData(vtkPolyData* data)
{
    d_ptr->contourData = data;
}

void AbstractContourGenerator::setContourLinesData(vtkPolyData* data)
{
    d_ptr->contourLinesData = data;
}

void AbstractContourGenerator::setContourLabelsData(vtkPolyData* data)
{
    d_ptr->contourLabelsData = data;
}



