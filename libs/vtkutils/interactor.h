#ifndef INTERACTOR_H
#define INTERACTOR_H

#define DEFAULT 0
#define BOXCUTTER 1
#define LINEPROBER 2
#define PLANECUTTER 3
#define SPHERECUTTER 4
#define POINTMOVER 5

#include <QObject>

namespace ComDataGui3D
{
    class Graph3D;
    class PostProcessor;
}

class vtkRenderer;
class InteractorPrivate;
class Interactor : public QObject
{
    Q_OBJECT
public:
    explicit Interactor(QObject* parent = nullptr);
    virtual ~Interactor();

    virtual int type() const;

    virtual void attach(ComDataGui3D::PostProcessor* pp);
    virtual void detach();
    bool isHooked() const;

    virtual void update();

    virtual void setVisible(bool visible);
    virtual bool isVisible() const;

protected:
    ComDataGui3D::Graph3D* graph() const;
    vtkRenderer* renderer() const;
    ComDataGui3D::PostProcessor* processor() const;
    double* bounds() const;

    void setDefaultInteractorStyle();
    virtual void setSpecialInteractorStyle();

private:
    InteractorPrivate* d_ptr;
    Q_DISABLE_COPY(Interactor)
};

#endif // INTERACTOR_H
