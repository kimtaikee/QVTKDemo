#include "tablemodel.h"

#include "utils.h"

#include <QDebug>

namespace Utils
{

TableModel::TableModel(int column, int row, QObject* parent) : QAbstractTableModel(parent), m_cols(column), m_rows(row)
{
    m_cols = column;
    m_rows = row;

    // m_data
    for (int i = 0; i < m_rows; i++) {
        QVector<qreal>* dataVec = new QVector<qreal>(m_cols);
        for (int k = 0; k < dataVec->size(); k++) {
            if (k % 2 == 0)
                dataVec->replace(k, i * 50 + qrand() % 20);
            else
                dataVec->replace(k, qrand() % 100);
        }
        m_data.append(dataVec);
    }
}

TableModel::~TableModel()
{
    clear();
}

void TableModel::random(int min, int max)
{
    m_randomMin = min;
    m_randomMax = max;

    for (int r = 0; r < m_rows; ++r) {
        for (int c = 0; c < m_cols; ++c) {
            auto vecPtr = m_data[r];
            vecPtr->replace(c, Utils::random(m_randomMin, m_randomMax));
        }
    }
    emit layoutChanged();
}

void TableModel::resize(int column, int row)
{
    if (m_cols == column && m_rows == row) {
//        qDebug() << "TableModel::resize: same cols & rows.";
        return;
    }

    if (m_cols != column)
        emit columnsChanged(m_cols, column);

    if (m_rows != row)
        emit rowsChanged(m_rows, row);

    qDeleteAll(m_data);
    m_data.clear();

    m_cols = column;
    m_rows = row;

    // m_data
    for (int i = 0; i < m_rows; i++) {
        QVector<qreal>* dataVec = new QVector<qreal>(m_cols);
        for (int k = 0; k < dataVec->size(); k++)
            dataVec->replace(k, qreal());
        m_data.append(dataVec);
    }
}

void TableModel::clear()
{
    qDeleteAll(m_data);
    m_data.clear();

    m_rows = 0;
    m_cols = 0;

    emit layoutChanged();
}

int TableModel::randomMin()
{
    return m_randomMin;
}

int TableModel::randomMax()
{
    return m_randomMax;
}

void TableModel::setHorizontalHeaderData(const QVariantList& data)
{
    if (m_horHeaderData != data) {
        m_horHeaderData = data;
        emit layoutChanged();
    }
}

QVariantList TableModel::horizontalHeaderData() const
{
    return m_horHeaderData;
}

void TableModel::setVerticalHeaderData(const QVariantList& data)
{
    if (m_verHeaderData != data) {
        m_verHeaderData = data;
        emit layoutChanged();
    }
}

QVariantList TableModel::verticalHeaderData() const
{
    return m_verHeaderData;
}

int TableModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return m_rows;
}

int TableModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return m_cols;
}

QVariant TableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
        return QVariant();

    if (orientation == Qt::Horizontal) {
        if (!m_horHeaderData.isEmpty() && m_horHeaderData.size() > section)
            return m_horHeaderData.at(section);
        else
            return QString("%1").arg(section + 1);
    }

    if (orientation == Qt::Vertical) {
        if (!m_verHeaderData.isEmpty() && m_verHeaderData.size() > section)
            return m_verHeaderData.at(section);
        else
            return QString("%1").arg(section + 1);
    }
}

QVariant TableModel::data(const QModelIndex &index, int role) const
{
    if (role == Qt::DisplayRole) {
        return m_data[index.row()]->at(index.column());
    } else if (role == Qt::EditRole) {
        return m_data[index.row()]->at(index.column());
    }

    return QVariant();
}

qreal TableModel::data(int row, int col) const
{
    return m_data[row]->at(col);
}

bool TableModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (index.isValid() && role == Qt::EditRole) {
        m_data[index.row()]->replace(index.column(), value.toDouble());
        emit dataChanged(index, index);
        return true;
    }
    return false;
}

bool TableModel::setData(int row, int column, const QVariant& value)
{
    QModelIndex index = this->createIndex(row, column);
    return setData(index, value);
}

Qt::ItemFlags TableModel::flags(const QModelIndex &index) const
{
    return QAbstractItemModel::flags(index) | Qt::ItemIsEditable;
}

} // namespace Utils
