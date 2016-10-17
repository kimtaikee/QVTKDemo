#include "pointstomodelconverter.h"
#include "tablemodel.h"

#include <QDebug>

namespace Utils
{

PointsToModelConverter::PointsToModelConverter(const QList<Point3F>& points, TableModel* model) :
    m_points(points), m_model(model)
{

}

void PointsToModelConverter::run()
{
    if (!m_model) {
        qDebug() << "PointsToModelConverter::run : null model.";
        emit finished();
        return;
    }

    m_model->resize(3, m_points.size());
    m_model->blockSignals(true);

    for (int i = 0; i < m_points.size(); ++i) {
        m_model->setData(i, 0, m_points.at(i).x);
        m_model->setData(i, 1, m_points.at(i).y);
        m_model->setData(i, 2, m_points.at(i).z);
    }

    m_model->blockSignals(false);

    emit finished();
}

} // namespace Utils
