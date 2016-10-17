#include "modeltopointsconverter.h"

#include <QDebug>

namespace Utils
{

ModelToPointsConverter::ModelToPointsConverter(TableModel* model) : m_model(model)
{

}

QList<Point3F> ModelToPointsConverter::points() const
{
    return m_points;
}

void ModelToPointsConverter::run()
{
    if (!m_model) {
        qDebug() << "ModelToPointsConverter::run: null model.";
        emit finished();
        return;
    }

    int rows = m_model->rowCount();
    int cols = m_model->columnCount();

    if (cols < 3) {
        emit finished();
        qDebug() << "ModelToPointsConverter::run: insufficient columns.";
        return;
    }

    for (int i = 0; i < rows; ++i) {
        Point3F pot{m_model->data(i, 0), m_model->data(i, 1), m_model->data(i, 2)};
        m_points.append(pot);
    }

    emit finished();
}

} // namespace Utils
