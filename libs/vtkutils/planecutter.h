#ifndef PLANECUTTER_H
#define PLANECUTTER_H

#include "interactor.h"

class vtkPolyData;
class PlaneCutterPrivate;
class PlaneCutter : public Interactor
{
    Q_OBJECT
public:
    explicit PlaneCutter(QObject* parent = nullptr);
    ~PlaneCutter();

    int type() const;

    void update();

    void attach(ComDataGui3D::PostProcessor* pp);
    void detach();

    void setOrigin(double* origin, bool update = true);
    double* origin() const;

    void setNormal(double* normal, bool update = true);
    double* normal() const;

    void setBoxData(vtkPolyData* data);

    void setArrowVisible(bool visible);
    bool isArrowVisible() const;

    void setCutPlaneVisible(bool visible);
    bool isCutPlaneVisible() const;

    void setVisible(bool visible);
    bool isVisible() const;

signals:
    void stickPointsChanged(double* topPot, double* bottomPot);
    void originChanged(double* origin);
    void normalChanged(double* normal);

private slots:
    void onMouseReleased();
    void onUpdate();
    void onOriginChanged(double* pos);
    void onNormalChanged(bool positive, double* pos);
    void onPlaneChanged(double* pos);

protected:
    void setSpecialInteractorStyle();

private:
    PlaneCutterPrivate* d_ptr;
    Q_DISABLE_COPY(PlaneCutter)
};

#endif // PLANECUTTER_H
