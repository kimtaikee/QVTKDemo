#include "plotactorwindow.h"
#include "ui_plotactorwindow.h"

#include <utils/tablemodel.h>

#include <widgets/colorpushbutton.h>

#include <vtkutils/vtkutils.h>
#include <vtkutils/vtkwidget.h>

#include <vtkActor2D.h>
#include <vtkColorSeries.h>

#include <QCheckBox>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QComboBox>
#include <QLineEdit>

PlotActorWindow::PlotActorWindow(QWidget *parent) :
    QWidget(parent),
    m_ui(new Ui::PlotActorWindow)
{
    m_ui->setupUi(this);
    m_vtkWidget = new VtkUtils::VtkWidget(this);
    m_ui->previewLayout->addWidget(m_vtkWidget);

    initTableModel();

    m_ui->rowsSpinBox->setValue(m_tableModel->rowCount());
    m_ui->colsSpinBox->setValue(m_tableModel->columnCount());
    m_ui->minSpinBox->setValue(m_tableModel->randomMin());
    m_ui->maxSpinBox->setValue(m_tableModel->randomMax());

    m_ui->dataTable->setModel(m_tableModel);
}

PlotActorWindow::~PlotActorWindow()
{
    delete m_ui;

    VtkUtils::vtkSafeDelete(m_plotActor);
}

vtkColor3ub PlotActorWindow::coolColor(int index)
{
    VTK_CREATE(vtkColorSeries, colorSeries);
    colorSeries->SetColorScheme(vtkColorSeries::COOL);
    int numOfColors = colorSeries->GetNumberOfColors();
    return colorSeries->GetColor(index % numOfColors);
}

vtkColor3ub PlotActorWindow::warmColor(int index)
{
    VTK_CREATE(vtkColorSeries, colorSeries);
    colorSeries->SetColorScheme(vtkColorSeries::WARM);
    int numOfColors = colorSeries->GetNumberOfColors();
    return colorSeries->GetColor(index % numOfColors);
}

void PlotActorWindow::updateWidget()
{
    if (m_vtkWidget)
        m_vtkWidget->update();
}

void PlotActorWindow::onModelDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight)
{
    render();
}

void PlotActorWindow::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);

    createPlotActor();
    render();
}

void PlotActorWindow::createPlotActor()
{
    // no impl
}

vtkDataObject* PlotActorWindow::createPlotData()
{
    vtkFieldData* field = vtkFieldData::New();
    vtkDataObject* data = vtkDataObject::New();

    VTK_CREATE(vtkDoubleArray, xArr);
    xArr->SetName("X");
    VTK_CREATE(vtkDoubleArray, scalarArr);
    scalarArr->SetName("Scalar");

    for (int i = 0; i < DataSize; ++i) {
        xArr->InsertTuple1(i, i);
        scalarArr->InsertTuple1(i, Utils::random(0, 10));
    }

    field->AddArray(xArr);
    field->AddArray(scalarArr);
    data->SetFieldData(field);

    return data;
}

void PlotActorWindow::initTableModel()
{
    if (m_tableModel)
        return;

    m_tableModel = new Utils::TableModel(2, 10, this);
    connect(m_tableModel, SIGNAL(dataChanged(QModelIndex,QModelIndex)), this, SLOT(onModelDataChanged(QModelIndex,QModelIndex)));
    m_tableModel->setHorizontalHeaderData(QVariantList() << "Y" << "Y");
    m_tableModel->random();
}

void PlotActorWindow::render()
{
    // no impl
}

void PlotActorWindow::layoutActor(vtkActor2D *actor)
{
    actor->GetPositionCoordinate()->SetValue(0.05, 0.05);
    actor->GetPosition2Coordinate()->SetValue(0.8, 0.8);
}

void PlotActorWindow::hookCheckBox(QCheckBox* cb)
{
    // prevent multiple connetions
    disconnect(cb, SIGNAL(stateChanged(int)), this, SLOT(onCheckBoxStateChanged(int)));
    connect(cb, SIGNAL(stateChanged(int)), this, SLOT(onCheckBoxStateChanged(int)));
}

void PlotActorWindow::hookSpinBox(QSpinBox* sb)
{
    // prevent multiple connetions
    disconnect(sb, SIGNAL(valueChanged(int)), this, SLOT(onSpinBoxValueChanged(int)));
    connect(sb, SIGNAL(valueChanged(int)), this, SLOT(onSpinBoxValueChanged(int)));
}

void PlotActorWindow::hookDoubleSpinBox(QDoubleSpinBox* dsb)
{
    // prevent multiple connetions
    disconnect(dsb, SIGNAL(valueChanged(double)), this, SLOT(onDoubleSpinBoxValueChanged(double)));
    connect(dsb, SIGNAL(valueChanged(double)), this, SLOT(onDoubleSpinBoxValueChanged(double)));
}

void PlotActorWindow::hookLineEdit(QLineEdit* le)
{
    // prevent multiple connetions
    disconnect(le, SIGNAL(textChanged(QString)), this, SLOT(onLineEditTextChanged(QString)));
    connect(le, SIGNAL(textChanged(QString)), this, SLOT(onLineEditTextChanged(QString)));
}

void PlotActorWindow::hookComboBox(QComboBox* cb)
{
    // prevent multiple connetions
    disconnect(cb, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));
    connect(cb, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));
}

void PlotActorWindow::hookGroupBox(QGroupBox* gb)
{
    disconnect(gb, SIGNAL(clicked(bool)), this, SLOT(onGroupBoxChecked(bool)));
    connect(gb, SIGNAL(clicked(bool)), this, SLOT(onGroupBoxChecked(bool)));
}

void PlotActorWindow::hookColorButton(Widgets::ColorPushButton* cpb)
{
    disconnect(cpb, SIGNAL(colorChanged(QColor)), this, SLOT(onColorChanged(QColor)));
    connect(cpb, SIGNAL(colorChanged(QColor)), this, SLOT(onColorChanged(QColor)));
}

void PlotActorWindow::on_rowsSpinBox_valueChanged(int arg1)
{
    if (m_tableModel) {
        m_tableModel->resize(m_tableModel->columnCount(), arg1);
        m_tableModel->random();
        emit m_tableModel->layoutChanged();
        render();
    }
}

void PlotActorWindow::on_colsSpinBox_valueChanged(int arg1)
{
    if (m_tableModel) {
        QVariantList horLabels;
        for (int i = 0; i < arg1; ++i)
            horLabels << "Y";
        m_tableModel->setHorizontalHeaderData(horLabels);

        m_tableModel->resize(arg1, m_tableModel->rowCount());
        m_tableModel->random();
        emit m_tableModel->layoutChanged();
        render();
    }
}

void PlotActorWindow::on_randomButton_clicked()
{
    if (m_tableModel)
        m_tableModel->random();
    render();
}

void PlotActorWindow::on_minSpinBox_valueChanged(int arg1)
{
    if (m_tableModel) {
        m_tableModel->random(arg1, m_tableModel->randomMax());
        render();
    }
}

void PlotActorWindow::on_maxSpinBox_valueChanged(int arg1)
{
    if (m_tableModel) {
        m_tableModel->random(m_tableModel->randomMin(), arg1);
        render();
    }
}
