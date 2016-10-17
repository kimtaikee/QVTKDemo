#include "modeltovectorsconverter.h"
#include "tablemodel.h"

#include <QDebug>

namespace Utils
{

ModelToVectorsConverter::ModelToVectorsConverter(TableModel* model) : m_model(model)
{

}

void ModelToVectorsConverter::run()
{
    if (!m_model) {
        qDebug() << "ModelToVectorsConverter::run: null model.";
        emit finished();
        return;
    }

    int rows = m_model->rowCount();
    int cols = m_model->columnCount();

    if (cols < 4) {
        emit finished();
        qDebug() << "ModelToVectorsConverter::run: insufficient columns.";
        return;
    }

    for (int i = 0; i < rows; ++i) {
        Vector4F vec{m_model->data(i, 0), m_model->data(i, 1), m_model->data(i, 2), m_model->data(i, 3)};
        m_vectors.append(vec);
    }

    emit finished();

}

QList<Vector4F> ModelToVectorsConverter::vectors() const
{
    return m_vectors;
}

} // namespace Utils
