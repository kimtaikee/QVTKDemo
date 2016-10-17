#ifndef VTKPLOTPIEWIDGET_H
#define VTKPLOTPIEWIDGET_H

#include "vtkplotwidget.h"

namespace VtkUtils
{
class VtkPlotPieWidgetPrivate;
class VTKUTILS_EXPORT VtkPlotPieWidget : public VtkPlotWidget
{
    Q_OBJECT
public:
    explicit VtkPlotPieWidget(QWidget* parent = nullptr);
    ~VtkPlotPieWidget();

    vtkContextItem* chart() const;

private:
    VtkPlotPieWidgetPrivate* d_ptr;
    Q_DISABLE_COPY(VtkPlotPieWidget)
};

} // namespace VtkUtils
#endif // VTKPLOTPIEWIDGET_H
