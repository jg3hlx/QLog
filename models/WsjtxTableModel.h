#ifndef QLOG_MODELS_WSJTXTABLEMODEL_H
#define QLOG_MODELS_WSJTXTABLEMODEL_H

#include <QAbstractTableModel>
#include "data/WsjtxEntry.h"

class WsjtxTableModel : public QAbstractTableModel {
    Q_OBJECT

public:
    WsjtxTableModel(QObject* parent = nullptr) : QAbstractTableModel(parent) {spotPeriod = 120;}
    int rowCount(const QModelIndex& parent = QModelIndex()) const;
    int columnCount(const QModelIndex& parent = QModelIndex()) const;
    QVariant data(const QModelIndex& index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
    void addOrReplaceEntry(WsjtxEntry entry);
    void spotAging();
    bool callsignExists(const WsjtxEntry &);
    const WsjtxEntry getEntry(const QString &callsign) const;
    const WsjtxEntry getEntry(QModelIndex idx) const;

    void setCurrentSpotPeriod(float);
    void clear();

private:
    QList<WsjtxEntry> wsjtxData;
    float spotPeriod;
};

#endif // QLOG_MODELS_WSJTXTABLEMODEL_H
