#include "xyplotactorwindow.h"

#include <utils/tablemodel.h>

#include <vtkutils/vtkwidget.h>
#include <vtkutils/vtkutils.h>

#include <vtkRenderer.h>
#include <vtkXYPlotActor.h>
#include <vtkActor2D.h>
#include <vtkFieldData.h>
#include <vtkDataObject.h>
#include <vtkXYPlotActor.h>
#include <vtkColor.h>
#include <vtkConeSource.h>
#include <vtkArrowSource.h>
#include <vtkCylinderSource.h>
#include <vtkProperty2D.h>
#include <vtkPlatonicSolidSource.h>

#include <QDebug>

XYPlotActorWindow::XYPlotActorWindow(QWidget* parent) : PlotActorWindow(parent)
{
    setWindowTitle(tr("XY Plot Actor"));
}

void XYPlotActorWindow::createPlotActor()
{
    // prevent multiple creations
    if (m_plotActor)
        return;

    m_plotActor = vtkXYPlotActor::New();

    vtkXYPlotActor* xyPlotActor = vtkXYPlotActor::SafeDownCast(m_plotActor);

    // make the margins take less space
    layoutActor(xyPlotActor);
//    xyPlotActor->GetPositionCoordinate()->SetValue(0.05, 0.05);
//    xyPlotActor->GetPosition2Coordinate()->SetValue(0.8, 0.8);

//    xyPlotActor->SetChartBox(1);
//    xyPlotActor->SetChartBorder(1);
    xyPlotActor->ExchangeAxesOff();
    xyPlotActor->SetLabelFormat("%g");
    xyPlotActor->SetXTitle("Level");
    xyPlotActor->SetYTitle("Frequency");
    xyPlotActor->SetXValuesToValue();
    xyPlotActor->LegendOn();
    xyPlotActor->SetGlyphSize(0.05);

    m_vtkWidget->defaultRenderer()->AddActor(m_plotActor);
}

void XYPlotActorWindow::render()
{
    vtkXYPlotActor* xyplotActor = vtkXYPlotActor::SafeDownCast(m_plotActor);
    xyplotActor->RemoveAllDataSetInputConnections(); // clear plot

    ///
    int cols = m_tableModel->columnCount();
    int rows = m_tableModel->rowCount();

    for (int col = 0; col < cols; ++col) {
        vtkFieldData* field = vtkFieldData::New();
        vtkDataObject* data = vtkDataObject::New();

        ///
        VTK_CREATE(vtkDoubleArray, xArr);
        xArr->SetName("x");
        for (int row = 0; row < rows; ++row)
            xArr->InsertTuple1(row, row);

        ///
        QString colName = QString("column#%1").arg(col + 1);
        VTK_CREATE(vtkDoubleArray, values);
        values->SetName(colName.toUtf8().data());
        for (int row = 0; row < rows; ++row)
            values->InsertTuple1(row, m_tableModel->data(row, col));
        field->AddArray(values);
        field->AddArray(xArr);

        VTK_CREATE(vtkPlatonicSolidSource, cone);
        cone->SetSolidTypeToOctahedron();
        cone->Update();
        xyplotActor->SetPlotSymbol(col, cone->GetOutput());

        data->SetFieldData(field);
        xyplotActor->AddDataObjectInput(data);
        xyplotActor->SetPointComponent(col, 1);

        vtkColor3ub color = PlotActorWindow::coolColor(col);
        double clr[3];
        Utils::vtkColor(QColor(color.GetRed(), color.GetGreen(), color.GetBlue()), clr);
        xyplotActor->SetPlotColor(col, clr);
        xyplotActor->SetPlotLabel(col, tr("Line#%1").arg(col + 1).toUtf8().data());
    }

    updateWidget();
}

