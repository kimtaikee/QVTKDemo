#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>

namespace Ui {
class Dialog;
}

class VTKWidget;
class Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog(QWidget *parent = 0);
    ~Dialog();

private slots:
    void on_lineEdit_textChanged(const QString &arg1);

private:
    Ui::Dialog *ui;
    VTKWidget* m_vtkWidget = nullptr;
};

#endif // DIALOG_H
