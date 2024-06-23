#ifndef QLOG_MODELS_ALERTTABLEMODEL_H
#define QLOG_MODELS_ALERTTABLEMODEL_H

#include <QAbstractTableModel>
#include <data/SpotAlert.h>
#include <QMutex>
#include "core/LogLocale.h"

class AlertTableModel : public QAbstractTableModel
{
    Q_OBJECT


public:
    struct AlertTableRecord
    {
        QStringList ruleName;
        long long counter;
        SpotAlert alert;

        bool operator==(const AlertTableRecord &) const;
        explicit AlertTableRecord(const SpotAlert&);
    };

    AlertTableModel(QObject* parent = nullptr) : QAbstractTableModel(parent){};
    int rowCount(const QModelIndex& parent = QModelIndex()) const;
    int columnCount(const QModelIndex& parent = QModelIndex()) const;
    QVariant data(const QModelIndex& index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
    void addAlert(SpotAlert entry);
    void clear();
    const AlertTableRecord getTableRecord(const QModelIndex& index);
    void aging(const int clear_interval_sec);

private:
    LogLocale locale;
    QList<AlertTableRecord> alertList;
    QMutex alertListMutex;
};

#endif // QLOG_MODELS_ALERTTABLEMODEL_H
