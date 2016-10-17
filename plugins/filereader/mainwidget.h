#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QWidget>
#include <QMap>

namespace Ui
{
    class MainWidget;
}

namespace VtkUtils
{
    class VtkWidget;
    class AbstractFileReader;
}

class vtkDataObject;
class FileTree;
class MainWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MainWidget(QWidget *parent = 0);
    ~MainWidget();

private slots:
    void onImportFile(const QString& file);
    void onSetCurrentFile(const QString& file);
    void onObjFileReaderFinished();
    void onDynaFileReaderFinished();
    void onFluentFileReaderFinished();
    void onVrmlFileReaderFinished();
    void onStlFileReaderFinished();
    void onVtkFileReaderFinished();
    void onNastranFileReaderFinished();
    void onAnsysFileReaderFinished();
    void onPlyFileReaderFinished();
    void onPdbFileReaderFinished();
    void onFilterAdded(const QString& file, int filterType);

private:
    template <class DataObject, class Mapper>
    void createWidgetFromData(const QString& file, vtkDataObject* dataObj);

private:
    Ui::MainWidget *m_ui;
    FileTree* m_fileTree;
    QMap<QString, VtkUtils::VtkWidget*> m_fileVtkWidgetMap;
    QMap<QString, vtkDataObject* > m_fileDataObjectMap;
};

#endif // MAINWIDGET_H
