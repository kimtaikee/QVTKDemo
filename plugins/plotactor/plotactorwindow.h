#ifndef PLOTACTORWINDOW_H
#define PLOTACTORWINDOW_H

#include <QWidget>

#include <vtkColor.h>

namespace Ui {
class PlotActorWindow;
}

namespace VtkUtils
{
    class VtkWidget;
}

namespace Utils
{
    class TableModel;
}

namespace Widgets
{
    class ColorPushButton;
}

class vtkActor2D;
class vtkDataObject;

class QCheckBox;
class QSpinBox;
class QDoubleSpinBox;
class QComboBox;
class QLineEdit;
class QGroupBox;

class PlotActorWindow : public QWidget
{
    Q_OBJECT
public:
    enum { DataSize = 10 };
    explicit PlotActorWindow(QWidget *parent = 0);
    virtual ~PlotActorWindow();

    static vtkColor3ub coolColor(int index);
    static vtkColor3ub warmColor(int index);

    void updateWidget();

protected slots:
    virtual void onModelDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight);
    virtual void onCheckBoxStateChanged(int state) { Q_UNUSED(state) }
    virtual void onSpinBoxValueChanged(int value) { Q_UNUSED(value) }
    virtual void onDoubleSpinBoxValueChanged(double value) { Q_UNUSED(value) }
    virtual void onComboBoxIndexChanged(int index) { Q_UNUSED(index) }
    virtual void onLineEditTextChanged(const QString& text) { Q_UNUSED(text) }
    virtual void onGroupBoxChecked(bool checked) { Q_UNUSED(checked) }
    virtual void onColorChanged(const QColor& clr) { Q_UNUSED(clr) }

protected:
    virtual void showEvent(QShowEvent *event);
    virtual void createPlotActor();
    virtual vtkDataObject* createPlotData();
    virtual void initTableModel();
    virtual void render();
    virtual void layoutActor(vtkActor2D* actor);

    void hookCheckBox(QCheckBox* cb);
    void hookSpinBox(QSpinBox* sb);
    void hookDoubleSpinBox(QDoubleSpinBox* dsb);
    void hookLineEdit(QLineEdit* le);
    void hookComboBox(QComboBox* cb);
    void hookGroupBox(QGroupBox* gb);
    void hookColorButton(Widgets::ColorPushButton* cpb);

    template <class UiClass>
    void setupConfigWidget(UiClass* ui)
    {
        if (!ui)
            return;

        QWidget* settignsWidget = new QWidget(this);
        ui->setupUi(settignsWidget);
        m_ui->settingsContainer->setWidget(settignsWidget);
    }

protected:
    Ui::PlotActorWindow *m_ui;
    VtkUtils::VtkWidget* m_vtkWidget = nullptr;
    vtkActor2D* m_plotActor = nullptr;
    Utils::TableModel* m_tableModel = nullptr;

private slots:
    void on_rowsSpinBox_valueChanged(int arg1);
    void on_colsSpinBox_valueChanged(int arg1);
    void on_randomButton_clicked();
    void on_minSpinBox_valueChanged(int arg1);
    void on_maxSpinBox_valueChanged(int arg1);
};

#endif // PLOTACTORWINDOW_H
