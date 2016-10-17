#include "bandedcontourgenerator.h"

#include <vtkutils/vtkutils.h>

#include <vtkPlaneSource.h>
#include <vtkPolyData.h>
#include <vtkDoubleArray.h>
#include <vtkPoints.h>
#include <vtkDelaunay2D.h>
#include <vtkPointData.h>
#include <vtkCleanPolyData.h>
#include <vtkClipPolyData.h>
#include <vtkSmartPointer.h>
#include <vtkContourFilter.h>
#include <vtkAppendPolyData.h>
#include <vtkFloatArray.h>
#include <vtkCellData.h>

#include <QDebug>

BandedContourGenerator::BandedContourGenerator()
{

}

int BandedContourGenerator::type() const
{
    return Banded;
}

void BandedContourGenerator::createContour()
{
    vtkPolyData* polyData = this->polyData();
    if (!polyData) {
        emit finished();
        return;
    }

    VTK_CREATE(vtkDelaunay2D, del);
    del->SetInputData(polyData);
    del->Update();

    double bounds[6];
    del->GetOutput()->GetBounds(bounds);
    double zMin = bounds[4];
    double zMax = bounds[5];
    double xRange = bounds[1] - bounds[0];
    double yRange = bounds[3] - bounds[2];
    double zRange = bounds[5] - bounds[4];

    // add plane
    VTK_CREATE(vtkPlaneSource, planeSource);
    planeSource->SetXResolution(bounds[1] - bounds[0]);
    planeSource->SetYResolution(bounds[3] - bounds[2]);
    planeSource->SetOrigin(bounds[0], bounds[2], bounds[4]);
    planeSource->SetPoint1(bounds[1], bounds[2], bounds[4]);
    planeSource->SetPoint2(bounds[0], bounds[3], bounds[4]);
    planeSource->Update();

    // carve on the plane
    vtkPolyData* surfaceOutput = del->GetOutput();

    VTK_CREATE(vtkDoubleArray, planeScalars);
    planeScalars->SetNumberOfComponents(1);
    planeScalars->SetName("Isovalues");

    VTK_CREATE(vtkPolyData, customPlanePolyData);
    VTK_CREATE(vtkPoints, customPoints);
    double pos[3];
    for (int i = 0; i < surfaceOutput->GetNumberOfPoints(); i++) {
        surfaceOutput->GetPoint(i, pos);
        customPoints->InsertPoint(i, pos[0], pos[1], zMin);
        planeScalars->InsertNextTuple1(pos[2]);
    }

    customPlanePolyData->SetPoints(customPoints);
    customPlanePolyData->GetPointData()->SetScalars(planeScalars);

    VTK_CREATE(vtkDelaunay2D, planeDel);
    planeDel->SetOffset(qMax(xRange, qMax(yRange, zRange)));
    planeDel->SetInputData(customPlanePolyData);
    planeDel->Update();

    vtkPolyData* planePolyData = planeDel->GetOutput();
    //
    double scalarRange[2];
    planePolyData->GetPointData()->GetScalars()->GetRange(scalarRange);

    VTK_CREATE(vtkCleanPolyData, cleanPolyData);

    double delta = (scalarRange[1] - scalarRange[0]) /
            static_cast<double> (numberOfContours() - 1);

    // Keep the clippers alive
    std::vector<vtkSmartPointer<vtkClipPolyData> > clippersLo;
    std::vector<vtkSmartPointer<vtkClipPolyData> > clippersHi;

    VTK_CREATE(vtkAppendPolyData, appendBandedContours);
    VTK_CREATE(vtkPoints, labelPoints);
    VTK_CREATE(vtkDoubleArray, labelScalars);

    for (int i = 0; i < numberOfContours(); i++) {
        double valueLo = scalarRange[0] + static_cast<double> (i) * delta;
        double valueHi = scalarRange[0] + static_cast<double> (i + 1) * delta;
        clippersLo.push_back(vtkSmartPointer<vtkClipPolyData>::New());
        clippersLo[i]->SetValue(valueLo);
        if (i == 0)
            clippersLo[i]->SetInputData(planeDel->GetOutput());
        else
            clippersLo[i]->SetInputConnection(clippersHi[i - 1]->GetOutputPort(1));
        clippersLo[i]->InsideOutOff();
        clippersLo[i]->Update();

        clippersHi.push_back(vtkSmartPointer<vtkClipPolyData>::New());
        clippersHi[i]->SetValue(valueHi);
        clippersHi[i]->SetInputConnection(clippersLo[i]->GetOutputPort());
        clippersHi[i]->GenerateClippedOutputOn();
        clippersHi[i]->InsideOutOn();
        clippersHi[i]->Update();

        if (clippersHi[i]->GetOutput()->GetNumberOfCells() == 0)
            continue;

        VTK_CREATE(vtkFloatArray, cd);
        cd->SetNumberOfComponents(1);
        cd->SetNumberOfTuples(clippersHi[i]->GetOutput()->GetNumberOfCells());
        cd->FillComponent(0, valueLo);

        clippersHi[i]->GetOutput()->GetCellData()->SetScalars(cd);
        appendBandedContours->AddInputConnection(clippersHi[i]->GetOutputPort());

        // get label data
        vtkPolyData* hiPolyData = clippersHi[i]->GetClippedOutput();
        vtkPolyDataMapper* mapper = vtkPolyDataMapper::New();
        mapper->SetInputData(hiPolyData);
        mapper->Update();

        labelScalars->InsertNextTuple1(valueHi);

        double midPoint[3];
        vtkPoints* points = hiPolyData->GetPoints();
        int numOfPoints = points->GetNumberOfPoints();

        vtkIdType midPointId = numOfPoints / 2;
        points->GetPoint(midPointId, midPoint);

//        qDebug() << "Center: " << midPoint[0] << "," << midPoint[1] << "," << midPoint[2];
        labelPoints->InsertNextPoint(midPoint);
    }

    cleanPolyData->SetInputConnection(appendBandedContours->GetOutputPort());
    cleanPolyData->Update();

    //
    setContourData(cleanPolyData->GetOutput());

    if (generateLines()) {
        VTK_CREATE(vtkContourFilter, lineContour);
        lineContour->SetInputConnection(cleanPolyData->GetOutputPort());
        lineContour->GenerateValues(numberOfContours(), zMin, zMax);
        lineContour->Update();
        setContourLinesData(lineContour->GetOutput());
    }

    if (generateLabels()) {
        VTK_CREATE(vtkPolyData, labelPolyData);
        labelPolyData->SetPoints(labelPoints);
        labelPolyData->GetPointData()->SetScalars(labelScalars);
        setContourLabelsData(labelPolyData);
    }

    emit finished();
}
