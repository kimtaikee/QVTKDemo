#ifndef FILTERWINDOW_H
#define FILTERWINDOW_H

#include <QWidget>
#include <QDebug>

#include <vtkSmartPointer.h>

namespace Ui
{
    class GeneralFilterWindow;
}

namespace VtkUtils
{
    class VtkWidget;
}

namespace Utils
{
    class TableModel;
}

class vtkActor;
class vtkDataObject;
class vtkScalarBarActor;
class vtkLODActor;
class vtkLookupTable;
class FilterWindow : public QWidget
{
    Q_OBJECT
public:
    enum { DefaultRows = 100 };
    explicit FilterWindow(QWidget *parent = 0);
    virtual ~FilterWindow();

    void setFileName(const QString& fileName);
    QString fileName() const;

    virtual void update();
    virtual void apply() = 0;

protected slots:
    void onObjFileReaderFinished();
    void onDynaFileReaderFinished();
    void onFluentFileReaderFinished();
    void onVrmlFileReaderFinished();
    void onStlFileReaderFinished();
    void onVtkFileReaderFinished();
    void onNastranFileReaderFinished();
    void onAnsysFileReaderFinished();
    void onPlyFileReaderFinished();
    void randomTableModel();
    void fireupModelToPointsConverter();
    void onModelToPointsConverterFinished();
    void onPointsToPolyDataConverterFinished();

protected:
    enum DisplayEffect { Transparent, Opaque, Wireframe };
    void setDisplayEffect(DisplayEffect effect);
    DisplayEffect displayEffect() const;

    void applyDisplayEffect();
    void readFile();
    QString fileFilter() const;
    void browseFile();
    void showOrientationMarker(bool show = true);
    void showScalarBar(bool show = true);
    void showOutline(bool show = true);
    void setOutlineColor(const QColor& clr);

    bool isValidPolyData() const;
    bool isValidDataSet() const;

    void setResultData(vtkDataObject* data);
    vtkDataObject* resultData() const;

    void setScalarBarColors(const QColor& clr1, const QColor& clr2);
    QColor color1() const;
    QColor color2() const;

    void setScalarRange(double min, double max);
    double scalarMin() const;
    double scalarMax() const;

    vtkSmartPointer<vtkLookupTable> createLookupTable(double min, double max);

    virtual void modelReady();
    virtual void createUi();
    virtual void dataChanged();
    virtual void colorsChanged();

    template <class DataObject, class Mapper>
    void createActorFromData(vtkDataObject* dataObj);

    template <class ConfigClass>
    void setupConfigWidget(ConfigClass* cc)
    {
        QWidget* configWidget = new QWidget(this);
        cc->setupUi(configWidget);
        m_ui->setupUi(this);
        m_ui->configLayout->addWidget(configWidget);
    }

    void initTableModel();

protected:
    Ui::GeneralFilterWindow* m_ui = nullptr;
    QString m_fileName;
    DisplayEffect m_displayEffect = Transparent;
    vtkDataObject* m_dataObject = nullptr;
    vtkActor* m_modelActor = nullptr;
    vtkLODActor* m_filterActor = nullptr;
    vtkScalarBarActor* m_scalarBar = nullptr;
    vtkActor* m_outlineActor = nullptr;
    VtkUtils::VtkWidget* m_vtkWidget = nullptr;
    Utils::TableModel* m_tableModel = nullptr;
    vtkDataObject* m_resultData = nullptr;
    QColor m_color1 = Qt::blue;
    QColor m_color2 = Qt::red;
    double m_scalarMin = 0.0;
    double m_scalarMax = 1.0;
};

#endif // FILTERWINDOW_H
