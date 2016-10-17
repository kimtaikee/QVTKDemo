#ifndef SURFACE_H
#define SURFACE_H

#include "vtkwidget.h"
#include "vtkutils_global.h"

#include <utils/point3f.h>

class vtkActor;
namespace VtkUtils
{

class SurfacePrivate;
class VTKUTILS_EXPORT Surface : public VtkWidget
{
    Q_OBJECT
public:
    explicit Surface(QWidget* parent = nullptr);
    virtual ~Surface();

    void setPoints(const QList<Utils::Point3F>& points);

protected:
    virtual void renderSurface();
    vtkActor* surfaceActor() const;

private:
    SurfacePrivate* d_ptr;
    Q_DISABLE_COPY(Surface)
};

} // namespace VtkUtils
#endif // SURFACE_H
