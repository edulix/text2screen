#include "sortfilterproxymodel.h"

SortFilterProxyModel::SortFilterProxyModel(QObject *parent) :
    QSortFilterProxyModel(parent)
{
}

bool SortFilterProxyModel::filterAcceptsRow(int source_row,
    const QModelIndex &source_parent) const
{
    bool accepts = QSortFilterProxyModel::filterAcceptsRow(source_row, source_parent);

    qDebug() << "mFilterString = " << mFilterString;
    if (mFilterString.isEmpty()) {
        return accepts;
    }

    QModelIndex index = sourceModel()->index(source_row, 0);
    QString displayText = index.data().toString();

    qDebug() << "filtering " << displayText;

    QRegExp rx(mFilterString);
    rx.setPatternSyntax(QRegExp::Wildcard);
    rx.setCaseSensitivity(Qt::CaseInsensitive);

    accepts = accepts && displayText.contains(rx);

    return accepts;
}
