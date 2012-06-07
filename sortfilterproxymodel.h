#ifndef SORTFILTERPROXYMODEL_H
#define SORTFILTERPROXYMODEL_H

#include <QSortFilterProxyModel>
#include <QDebug>

class SortFilterProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT
    Q_PROPERTY(QString filterString READ filterString WRITE setFilterString NOTIFY filterStringChanged)
public:
    explicit SortFilterProxyModel(QObject *parent = 0);
    QString filterString() const { return mFilterString; }

public Q_SLOTS:
    void setFilterString(const QString &filterString) { mFilterString = filterString; qDebug() << "emit filterStringChanged();" << filterString; emit filterStringChanged(); reset(); }
Q_SIGNALS:
    void filterStringChanged();

protected:
    virtual bool filterAcceptsRow(int source_row, const QModelIndex & source_parent) const;

protected:
    QString mFilterString;
};

#endif // SORTFILTERPROXYMODEL_H
