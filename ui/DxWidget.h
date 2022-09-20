#ifndef DXWIDGET_H
#define DXWIDGET_H

#include <QWidget>
#include <QtNetwork>
#include <QSortFilterProxyModel>
#include <QRegularExpression>
#include <QSqlRecord>

#include "data/Data.h"
#include "data/DxSpot.h"
#include "ui/SwitchButton.h"

namespace Ui {
class DxWidget;
}

class DxTableModel : public QAbstractTableModel {
    Q_OBJECT

public:
    DxTableModel(QObject* parent = 0) : QAbstractTableModel(parent) {}

    int rowCount(const QModelIndex& parent = QModelIndex()) const;
    int columnCount(const QModelIndex& parent = QModelIndex()) const;
    QVariant data(const QModelIndex& index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
    void addEntry(DxSpot entry);
    QString getCallsign(const QModelIndex& index);
    double getFrequency(const QModelIndex& index);
    void clear();

private:
    QList<DxSpot> dxData;
};

class DeleteHighlightedDXServerWhenDelPressedEventFilter : public QObject
{
     Q_OBJECT
protected:
    bool eventFilter(QObject *obj, QEvent *event);
};

class DxWidget : public QWidget {
    Q_OBJECT

public:
    explicit DxWidget(QWidget *parent = 0);
    ~DxWidget();

public slots:
    void toggleConnect();
    void receive();
    void send();
    void connected();
    void socketError(QAbstractSocket::SocketError);
    void viewModeChanged(int);
    void entryDoubleClicked(QModelIndex);
    void actionFilter();
    void adjusteServerSelectSize(QString);
    void serverSelectChanged(int);
    void setLastQSO(QSqlRecord);

    void actionCommandSpotQSO();
    void actionCommandShowHFStats();
    void actionCommandShowVHFStats();
    void actionCommandShowWCY();
    void actionCommandShowWWV();

signals:
    void tuneDx(QString, double);
    void newSpot(DxSpot);
    void newFilteredSpot(DxSpot);

private:
    DxTableModel* dxTableModel;
    QTcpSocket* socket;
    Ui::DxWidget *ui;
    QRegularExpression moderegexp;
    QRegularExpression contregexp;
    QRegularExpression spottercontregexp;
    QRegularExpression bandregexp;
    QSqlRecord lastQSO;

    void connectCluster();
    void disconnectCluster();
    void saveDXCServers();
    QString modeFilterRegExp();
    QString contFilterRegExp();
    QString spotterContFilterRegExp();
    QString bandFilterRegExp();
    void sendCommand(const QString&,
                     bool switchToConsole = false);


    QStringList getDXCServerList(void);
};

#endif // DXWIDGET_H
