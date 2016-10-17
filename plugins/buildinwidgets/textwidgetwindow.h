#ifndef TEXTWIDGETWINDOW_H
#define TEXTWIDGETWINDOW_H

#include "basewidgetwindow.h"

namespace Ui
{
    class TextWidgetConfig;
}

class vtkTextWidget;
class vtkTextRepresentation;
class TextWidgetWindow : public BaseWidgetWindow
{
    Q_OBJECT
public:
    explicit TextWidgetWindow(QWidget* parent = nullptr);

protected:
    void createWidget();
    void createUi();

private slots:
    void on_textEdit_textChanged(const QString &arg1);
    void on_positionCombo_currentIndexChanged(int index);
    void on_resizableCheckBox_toggled(bool checked);
    void on_selectableCheckBox_toggled(bool checked);

private:
    Ui::TextWidgetConfig* m_configUi = nullptr;
    vtkSmartPointer<vtkTextWidget> m_text;
    vtkSmartPointer<vtkTextRepresentation> m_rep;
};

#endif // TEXTWIDGETWINDOW_H
