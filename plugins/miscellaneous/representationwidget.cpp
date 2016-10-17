#include "representationwidget.h"
#include "ui_representationwidget.h"

#include <vtkutils/vtkutils.h>
#include <vtkutils/vtkwidget.h>

#include <vtkArrowSource.h>
#include <vtkProperty.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkDelaunay2D.h>
#include <vtkPolyDataMapper.h>
#include <vtkRenderer.h>

#include <QtMath>
#include <QDebug>

using namespace VtkUtils;

static const int a = 10;
static const int b = 15;
static const int c = 18;
static const int n1 = 1;
static const int n2 = 0.75;
static const int n3 = n2;
static const int stepsize = 8;

static vtkPoints* createPoints()
{
    vtkPoints* points = vtkPoints::New();
    qreal x, y, z;

    for (int i = 1; i < stepsize; ++i) {
        qreal v = i * M_PI / (stepsize * 2);
        for (int j = 1; j < stepsize; ++j) {
            qreal u = j * M_PI / (stepsize * 2);
            x = a * (qPow(qSin(u),n1)) * (qPow(qCos(v),n2));
            y = b * (qPow(qSin(u),n1)) * (qPow(qSin(v),n2));
            z = c * (qPow(qCos(u),n3));
            points->InsertNextPoint(x, y, z);

            qDebug() << "(" << x << "," << y << "," << z << ")";
        }
    }
    return points;
}

RepresentationWidget::RepresentationWidget(QWidget *parent) :
    QWidget(parent),
    m_ui(new Ui::RepresentationWidget)
{
    m_ui->setupUi(this);

    // default to surface
    m_ui->surfaceRadioButton->setChecked(true);

    m_vtkWidget = new VtkWidget(this);
    m_ui->previewLayout->addWidget(m_vtkWidget);
    m_actor = VtkUtils::createSourceActor<vtkArrowSource>();
    m_actor->GetProperty()->SetPointSize(5);

    m_vtkWidget->addActor(m_actor);
    m_vtkWidget->setBackgroundColor(QColor(132, 155, 189));
    m_vtkWidget->showOrientationMarker();
//    m_vtkWidget->defaultRenderer()->ResetCamera();
    setWindowTitle(tr("Actor Representations"));
}

RepresentationWidget::~RepresentationWidget()
{
    delete m_ui;
}

void RepresentationWidget::on_surfaceRadioButton_clicked(bool checked)
{
    if (checked) {
        // show surface
        m_actor->GetProperty()->SetRepresentationToSurface();
        m_vtkWidget->update();
    }

}

void RepresentationWidget::on_pointsRadioButton_clicked(bool checked)
{
    if (checked) {
        // show points
        m_actor->GetProperty()->SetRepresentationToPoints();
        m_vtkWidget->update();
    }
}

void RepresentationWidget::on_wireframeRadioButton_clicked(bool checked)
{
    if (checked) {
        // show wireframe
        m_actor->GetProperty()->SetRepresentationToWireframe();
        m_vtkWidget->update();
    }
}
