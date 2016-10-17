#include "modeltovtktableconverter.h"
#include "vtkutils.h"

#include <utils/tablemodel.h>
#include <utils/utils.h>

#include <vtkTable.h>

#include <QDebug>

namespace VtkUtils
{

ModelToVtkTableConverter::ModelToVtkTableConverter(Utils::TableModel* model) : m_model(model)
{

}

void ModelToVtkTableConverter::setLabels(const QStringList& labels)
{
    m_labels = labels;
}

QStringList ModelToVtkTableConverter::labels() const
{
    return m_labels;
}

void ModelToVtkTableConverter::run()
{
    if (!m_model) {
        qDebug() << "ModelToVtkTableConverter::run : null model.";
        emit finished();
        return;
    }

    vtkInitOnce(&m_table);
    m_table->Initialize();

    int rows = m_model->rowCount();
    int cols = m_model->columnCount();

    QString columnName;
    for (int col = 0; col < cols ; ++col) {
        columnName = m_labels.size() > col ? m_labels.at(col) : QString("column#%1").arg(col + 1);
        vtkSmartPointer<vtkDoubleArray> arr = vtkDoubleArray::New();
        arr->SetName(columnName.toUtf8().data());
        m_table->AddColumn(arr);
    }
    m_table->SetNumberOfRows(rows); // the order matters , first you add columns then you set the rows of columns

    for (int r = 0; r < rows; ++r)
        for (int c = 0; c < cols; ++c)
            m_table->SetValue(r, c, m_model->data(r, c));

    emit finished();
}

vtkTable* ModelToVtkTableConverter::table() const
{
    return m_table;
}

} // namespace VtkUtils
