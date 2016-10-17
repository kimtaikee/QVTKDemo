#ifndef TABLEMODEL_H
#define TABLEMODEL_H

#include <QAbstractTableModel>

#include "utils_global.h"

namespace Utils
{

class UTILS_EXPORT TableModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit TableModel(int column, int row, QObject* parent = nullptr);
    virtual ~TableModel();

    void random(int min = -5, int max = 5);
    void resize(int column, int row);

    void clear();

    int randomMin();
    int randomMax();

    void setHorizontalHeaderData(const QVariantList& data);
    QVariantList horizontalHeaderData() const;

    void setVerticalHeaderData(const QVariantList& data);
    QVariantList verticalHeaderData() const;

    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    qreal data(int row, int col) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);
    bool setData(int row, int column, const QVariant& value);
    Qt::ItemFlags flags(const QModelIndex &index) const;

signals:
    void rowsChanged(int oldRows, int newRows);
    void columnsChanged(int oldCols, int newCols);

private:
    QList<QVector<qreal> * > m_data;
    QVariantList m_horHeaderData;
    QVariantList m_verHeaderData;
    int m_cols = 3;
    int m_rows = 30;
    int m_randomMin = -1;
    int m_randomMax = 1;
};

} // namespace Utils
#endif // TABLEMODEL_H
