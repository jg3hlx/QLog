#ifndef SEARCHFILTERPROXYMODEL_H
#define SEARCHFILTERPROXYMODEL_H

#include <QSortFilterProxyModel>

class SearchFilterProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT

public:
    SearchFilterProxyModel(QObject* parent = nullptr);
    void setSearchString(const QString& searchString);

protected:
    bool filterAcceptsRow(int source_row, const QModelIndex& source_parent) const override;

private:
    QString searchString;
};

#endif // SEARCHFILTERPROXYMODEL_H
