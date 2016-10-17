#ifndef DATAFILTER_H
#define DATAFILTER_H

#include <QRunnable>
#include <QObject>

#include <utils/signalledrunable.h>

#include <vtkSmartPointer.h>
#include <vtkClipPolyData.h>
#include <vtkCutter.h>
#include <vtkSmoothPolyDataFilter.h>
#include <vtkDecimatePro.h>
#include <vtkThreshold.h>
#include <vtkStreamTracer.h>
#include <vtkContourFilter.h>
#include <vtkExtractEdges.h>
#include <vtkTubeFilter.h>
#include <vtkDelaunay2D.h>
#include <vtkGlyph3D.h>
#include <vtkRuledSurfaceFilter.h>
#include <vtkVertexGlyphFilter.h>
#include <vtkApproximatingSubdivisionFilter.h>
#include <vtkCleanPolyData.h>
#include <vtkGenericContourFilter.h>
#include <vtkDensifyPolyData.h>
#include <vtkHedgeHog.h>
#include <vtkMarchingCubes.h>
#include <vtkMarchingSquares.h>
#include <vtkRibbonFilter.h>
#include <vtkHull.h>
#include <vtkSplineFilter.h>
#include <vtkShrinkFilter.h>
#include <vtkStripper.h>
#include <vtkProteinRibbonFilter.h>
#include <vtkQuadricDecimation.h>
#include <vtkTriangleFilter.h>

#include "vtkutils_global.h"

namespace VtkUtils
{

enum FilterType { FT_Clip, FT_Cut, FT_Slice, FT_Isosurface,
                  FT_Threshold, FT_Streamline, FT_Smooth, FT_Decimate, FT_Count };

QString VTKUTILS_EXPORT filterName(FilterType type);

class VTKUTILS_EXPORT AbstractDataFilter : public Utils::SignalledRunnable
{
public:
    AbstractDataFilter() {}
    virtual ~AbstractDataFilter() {}
};

template <class Filter, class InputDataObject = vtkPolyData, class OutputDataObject = vtkPolyData>
class DataFilter : public AbstractDataFilter
{
public:
    DataFilter()
    {
        m_filter = Filter::New();
    }

    virtual void run()
    {
        m_filter->SetInputData(m_inputData);
        m_filter->Update();
        emit finished();
    }

    virtual void setInput(InputDataObject* input)
    {
        m_inputData = input;
    }

    OutputDataObject* output() const
    {
        return m_filter->GetOutput();
    }

    Filter* filter() const
    {
        return m_filter;
    }

protected:
    vtkSmartPointer<Filter> m_filter;
    InputDataObject* m_inputData;
};

class VTKUTILS_EXPORT ClipFilter : public DataFilter<vtkClipPolyData> {};
class VTKUTILS_EXPORT CutterFilter : public DataFilter<vtkCutter> {};
class VTKUTILS_EXPORT SliceFilter : public DataFilter<vtkCutter> {};
class VTKUTILS_EXPORT DecimateProFilter : public DataFilter<vtkDecimatePro> {};
class VTKUTILS_EXPORT SmoothFilter : public DataFilter<vtkSmoothPolyDataFilter> {};
class VTKUTILS_EXPORT StreamTracerFilter : public DataFilter<vtkStreamTracer> {};
class VTKUTILS_EXPORT IsosurfaceFilter : public DataFilter<vtkContourFilter> {};
class VTKUTILS_EXPORT ExtractEdgesFilter : public DataFilter<vtkExtractEdges> {};
class VTKUTILS_EXPORT TubeFilter : public DataFilter<vtkTubeFilter> {};
class VTKUTILS_EXPORT Delaunay2DFilter : public DataFilter<vtkDelaunay2D> {};
class VTKUTILS_EXPORT Glyph3DFilter : public DataFilter<vtkGlyph3D> {};
class VTKUTILS_EXPORT RuledSurfaceFilter : public DataFilter<vtkRuledSurfaceFilter> {};
class VTKUTILS_EXPORT VertexGlyphFilter : public DataFilter<vtkVertexGlyphFilter> {};

} // namespace VtkUtils
#endif // DATAFILTER_H
