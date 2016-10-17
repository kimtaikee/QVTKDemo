#ifndef EDITSHADERWINDOW_H
#define EDITSHADERWINDOW_H

#include <QWidget>

#include <vtkSmartPointer.h>

namespace Ui
{
    class EditShaderWindow;
}

namespace VtkUtils
{
    class VtkWidget;
}

class vtkActor;
class vtkDataObject;
class EditShaderWindow : public QWidget
{
    Q_OBJECT

public:
    explicit EditShaderWindow(QWidget *parent = 0);
    ~EditShaderWindow();

private slots:
    void on_importButton_clicked();
    void on_applyShaderButton_clicked();

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
    void on_clear_clicked();

private:
    template <class DataObject, class Mapper>
    void createActorFromData(vtkDataObject* dataObj);

private:
    Ui::EditShaderWindow *m_ui;
    VtkUtils::VtkWidget* m_vtkWidget;
    vtkSmartPointer<vtkActor> m_modelActor;
};

#endif // EDITSHADERWINDOW_H
