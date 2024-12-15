#include <QUuid>
#include <QUdpSocket>

#include "NetworkNotification.h"
#include "debug.h"
#include "LogParam.h"

MODULE_IDENTIFICATION("qlog.ui.networknotification");

NetworkNotification::NetworkNotification(QObject *parent)
    : QObject(parent)
{
    FCT_IDENTIFICATION;
}

QString NetworkNotification::getNotifQSOAdiAddrs()
{
    FCT_IDENTIFICATION;

    QSettings settings;

    return settings.value(NetworkNotification::CONFIG_NOTIF_QSO_ADI_ADDRS_KEY).toString();
}

void NetworkNotification::saveNotifQSOAdiAddrs(const QString &addresses)
{
    FCT_IDENTIFICATION;

    QSettings settings;

    settings.setValue(NetworkNotification::CONFIG_NOTIF_QSO_ADI_ADDRS_KEY, addresses);
}

QString NetworkNotification::getNotifDXSpotAddrs()
{
    FCT_IDENTIFICATION;

    QSettings settings;

    return settings.value(NetworkNotification::CONFIG_NOTIF_DXSPOT_ADDRS_KEY).toString();
}

void NetworkNotification::saveNotifDXSpotAddrs(const QString &addresses)
{
    QSettings settings;

    settings.setValue(NetworkNotification::CONFIG_NOTIF_DXSPOT_ADDRS_KEY, addresses);
}

QString NetworkNotification::getNotifWSJTXCQSpotAddrs()
{
    FCT_IDENTIFICATION;

    QSettings settings;

    return settings.value(NetworkNotification::CONFIG_NOTIF_WSJTXCQSPOT_ADDRS_KEY).toString();
}

void NetworkNotification::saveNotifWSJTXCQSpotAddrs(const QString &addresses)
{
    FCT_IDENTIFICATION;

    QSettings settings;

    settings.setValue(NetworkNotification::CONFIG_NOTIF_WSJTXCQSPOT_ADDRS_KEY, addresses);
}

QString NetworkNotification::getNotifSpotAlertAddrs()
{
    FCT_IDENTIFICATION;

    QSettings settings;

    return settings.value(NetworkNotification::CONFIG_NOTIF_SPOTALERT_ADDRS_KEY).toString();
}

void NetworkNotification::saveNotifSpotAlertAddrs(const QString &addresses)
{
    FCT_IDENTIFICATION;

    QSettings settings;

    settings.setValue(NetworkNotification::CONFIG_NOTIF_SPOTALERT_ADDRS_KEY, addresses);

}

QString NetworkNotification::getNotifRigStateAddrs()
{
    FCT_IDENTIFICATION;

    QSettings settings;
    return settings.value(NetworkNotification::CONFIG_NOTIF_RIGSTATE_ADDRS_KEY).toString();
}

void NetworkNotification::saveNotifRigStateAddrs(const QString &addresses)
{
    FCT_IDENTIFICATION;

    QSettings settings;
    settings.setValue(NetworkNotification::CONFIG_NOTIF_RIGSTATE_ADDRS_KEY, addresses);
}

void NetworkNotification::QSOInserted(const QSqlRecord &record)
{
    FCT_IDENTIFICATION;

    qCDebug(function_parameters) << "Inserted: " << record;

    HostsPortString destList(getNotifQSOAdiAddrs());

    if ( destList.getAddrList().size() > 0 )
    {
        QSONotificationMsg qsoMsg(record, QSONotificationMsg::QSO_INSERT);
        send(qsoMsg.getJson(), destList);
    }
}

void NetworkNotification::QSOUpdated(const QSqlRecord &record)
{
    FCT_IDENTIFICATION;
    qCDebug(function_parameters) << "Updated: " << record;

    HostsPortString destList(getNotifQSOAdiAddrs());

    if ( destList.getAddrList().size() > 0 )
    {
        QSONotificationMsg qsoMsg(record, QSONotificationMsg::QSO_UPDATE);
        send(qsoMsg.getJson(), destList);
    }
}

void NetworkNotification::QSODeleted(const QSqlRecord &record)
{
    FCT_IDENTIFICATION;
    qCDebug(function_parameters) << "Deleted: " << record;

    HostsPortString destList(getNotifQSOAdiAddrs());

    if ( destList.getAddrList().size() > 0 )
    {
        QSONotificationMsg qsoMsg(record, QSONotificationMsg::QSO_DELETE);
        send(qsoMsg.getJson(), destList);
    }
}

void NetworkNotification::dxSpot(const DxSpot &spot)
{
    FCT_IDENTIFICATION;

    qCDebug(function_parameters) << "DX Spot";

    HostsPortString destList(getNotifDXSpotAddrs());

    if ( destList.getAddrList().size() > 0 )
    {
        DXSpotNotificationMsg dxSpotMsg(spot);
        send(dxSpotMsg.getJson(), destList);
    }
}

void NetworkNotification::wcySpot(const WCYSpot &spot)
{
    FCT_IDENTIFICATION;
    qCDebug(function_parameters) << "WCY Spot";

    HostsPortString destList(getNotifDXSpotAddrs());

    if ( destList.getAddrList().size() > 0 )
    {
        WCYSpotNotificationMsg WCYSpotMsg(spot);
        send(WCYSpotMsg.getJson(), destList);
    }
}

void NetworkNotification::wwvSpot(const WWVSpot &spot)
{
    FCT_IDENTIFICATION;
    qCDebug(function_parameters) << "WWV Spot";

    HostsPortString destList(getNotifDXSpotAddrs());

    if ( destList.getAddrList().size() > 0 )
    {
        WWVSpotNotificationMsg WWVSpotMsg(spot);
        send(WWVSpotMsg.getJson(), destList);
    }
}

void NetworkNotification::toAllSpot(const ToAllSpot &spot)
{
    FCT_IDENTIFICATION;
    qCDebug(function_parameters) << "ToALL Spot";

    HostsPortString destList(getNotifDXSpotAddrs());

    if ( destList.getAddrList().size() > 0 )
    {
        ToAllSpotNotificationMsg ToAllSpotMsg(spot);
        send(ToAllSpotMsg.getJson(), destList);
    }
}

void NetworkNotification::WSJTXCQSpot(const WsjtxEntry &spot)
{
    FCT_IDENTIFICATION;

    qCDebug(function_parameters) << "WSJTX CQ Spot";

    HostsPortString destList(getNotifWSJTXCQSpotAddrs());

    if ( destList.getAddrList().size() > 0 )
    {
        WSJTXCQSpotNotificationMsg dxSpotMsg(spot);
        send(dxSpotMsg.getJson(), destList);
    }
}

void NetworkNotification::spotAlert(const SpotAlert &spot)
{
    FCT_IDENTIFICATION;

    qCDebug(function_parameters) << "Usert Alert";

    HostsPortString destList(getNotifSpotAlertAddrs());

    if ( destList.getAddrList().size() > 0 )
    {
        SpotAlertNotificationMsg spotAlertMsg(spot);
        send(spotAlertMsg.getJson(), destList);
    }
}

void NetworkNotification::rigStatus(const Rig::Status &status)
{
    FCT_IDENTIFICATION;

    qCDebug(function_parameters) << "Rig Status";

    HostsPortString destList(getNotifSpotAlertAddrs());

    if ( destList.getAddrList().size() > 0 )
    {
        RigStatusNotificationMsg rigStatusMsg(status);
        send(rigStatusMsg.getJson(), destList);
    }
}

void NetworkNotification::send(const QByteArray &data, const HostsPortString &dests)
{
    FCT_IDENTIFICATION;

    qCDebug(function_parameters) << QString(data);

    if ( data.size() <= 0 )
    {
        return;
    }

    const QList<HostPortAddress> &addrList = dests.getAddrList();

    for ( const HostPortAddress &addr : addrList )
    {
        QUdpSocket udpSocket;

        qCDebug(runtime) << "Sending to " << addr;
        udpSocket.writeDatagram(data, addr, addr.getPort());
    }
}

QString NetworkNotification::CONFIG_NOTIF_QSO_ADI_ADDRS_KEY = "network/notification/qso/adi_addrs";
QString NetworkNotification::CONFIG_NOTIF_DXSPOT_ADDRS_KEY = "network/notification/dxspot/addrs";
QString NetworkNotification::CONFIG_NOTIF_WSJTXCQSPOT_ADDRS_KEY = "network/notification/wsjtx/cqspot/addrs";
QString NetworkNotification::CONFIG_NOTIF_SPOTALERT_ADDRS_KEY = "network/notification/alerts/spot/addrs";
QString NetworkNotification::CONFIG_NOTIF_RIGSTATE_ADDRS_KEY = "network/notification/rig/state/addrs";

GenericNotificationMsg::GenericNotificationMsg(QObject *parent) :
    QObject(parent)
{
    FCT_IDENTIFICATION;

    msg["appid"] = "QLog";
    msg["logid"] = LogParam::getLogID();
    msg["time"] = QDateTime::currentMSecsSinceEpoch();
}

QSONotificationMsg::QSONotificationMsg(const QSqlRecord &record,
                                       const QSOOperation operation,
                                       QObject *parent) :
    GenericNotificationMsg(parent)
{
    FCT_IDENTIFICATION;

    QString data;
    QTextStream stream(&data, QIODevice::ReadWrite);

    /* currently only ADIF is supported */
    /* ADX and JSON do not support export single record */
    LogFormat* format = LogFormat::open(LogFormat::ADI, stream);

    if (!format) {
        qWarning() << "cannot created ADIF Export Formatter";
        return;
    }

    //format->exportStart();
    format->exportContact(record);
    stream.flush();
    //format->exportEnd();

    QJsonObject qsoData;
    qsoData["operation"] = QSOOperation2String.value(operation,"unknown");
    qsoData["type"] = "adif";
    qsoData["rowid"] = record.value(record.indexOf("id")).toInt();
    qsoData["value"] = data.replace("\n", "");

    msg["msgtype"] = "qso";
    msg["data"] = qsoData;

    delete format;
}

DXSpotNotificationMsg::DXSpotNotificationMsg(const DxSpot &spot, QObject *parent) :
    GenericSpotNotificationMsg(parent)
{
    FCT_IDENTIFICATION;

    QJsonObject spotData;
    spotData["rcvtime"] = spot.dateTime.toString("yyyyMMdd hh:mm:ss");
    spotData["freq"] = qRound(spot.freq * 10000.0) / 10000.0;
    spotData["band"] = spot.band;
    spotData["mode"] = spot.modeGroupString; 
    spotData["comment"] = spot.comment;
    spotData["status"] = DxccStatus2String.value(spot.status, "unknown");

    QJsonObject dxInfo;
    dxInfo["call"] = spot.callsign;
    dxInfo["country"] = spot.dxcc.country;
    dxInfo["pfx"] = spot.dxcc.prefix;
    dxInfo["dxcc"] = spot.dxcc.dxcc;
    dxInfo["cont"] = spot.dxcc.cont;
    dxInfo["cqz"] = spot.dxcc.cqz;
    dxInfo["ituz"] = spot.dxcc.ituz;
    dxInfo["utcoffset"] = spot.dxcc.tz;
    dxInfo["member"] = QJsonArray::fromStringList(spot.memberList2StringList());

    QJsonObject spotterInfo;
    spotterInfo["call"] = spot.spotter;
    spotterInfo["country"] = spot.dxcc_spotter.country;
    spotterInfo["pfx"] = spot.dxcc_spotter.prefix;
    spotterInfo["dxcc"] = spot.dxcc_spotter.dxcc;
    spotterInfo["cont"] = spot.dxcc_spotter.cont;
    spotterInfo["cqz"] = spot.dxcc_spotter.cqz;
    spotterInfo["ituz"] = spot.dxcc_spotter.ituz;
    spotterInfo["utcoffset"] = spot.dxcc_spotter.tz;

    spotData["spotter"] = spotterInfo;
    spotData["dx"] = dxInfo;

    msg["msgtype"] = "dxspot";
    msg["data"] = spotData;
}

WSJTXCQSpotNotificationMsg::WSJTXCQSpotNotificationMsg(const WsjtxEntry &spot, QObject *parent) :
    GenericSpotNotificationMsg(parent)
{
    FCT_IDENTIFICATION;

    QJsonObject spotData;
    spotData["rcvtime"] = spot.receivedTime.toString("yyyyMMdd hh:mm:ss");
    spotData["freq"] = qRound(spot.freq * 10000.0) / 10000.0;
    spotData["band"] = spot.band;
    spotData["mode"] = spot.decodedMode;
    spotData["comment"] = spot.decode.message;
    spotData["status"] = DxccStatus2String.value(spot.status, "unknown");

    QJsonObject dxInfo;
    dxInfo["call"] = spot.callsign;
    dxInfo["country"] = spot.dxcc.country;
    dxInfo["pfx"] = spot.dxcc.prefix;
    dxInfo["dxcc"] = spot.dxcc.dxcc;
    dxInfo["cont"] = spot.dxcc.cont;
    dxInfo["cqz"] = spot.dxcc.cqz;
    dxInfo["ituz"] = spot.dxcc.ituz;
    dxInfo["utcoffset"] = spot.dxcc.tz;
    dxInfo["grid"] = spot.grid;
    dxInfo["member"] = QJsonArray::fromStringList(spot.memberList2StringList());

    spotData["dx"] = dxInfo;

    msg["msgtype"] = "wsjtxcqspot";
    msg["data"] = spotData;
}

GenericSpotNotificationMsg::GenericSpotNotificationMsg(QObject *parent)
    : GenericNotificationMsg(parent)
{
    FCT_IDENTIFICATION;
}

SpotAlertNotificationMsg::SpotAlertNotificationMsg(const SpotAlert &alert, QObject *parent) :
    GenericSpotNotificationMsg(parent)
{
    FCT_IDENTIFICATION;

    QJsonObject spotData;
    spotData["rcvtime"] = alert.spot.dateTime.toString("yyyyMMdd hh:mm:ss");
    spotData["freq"] = qRound(alert.spot.freq * 10000.0) / 10000.0;
    spotData["band"] = alert.spot.band;
    spotData["mode"] = alert.spot.modeGroupString;
    spotData["comment"] = alert.spot.comment;
    spotData["status"] = DxccStatus2String.value(alert.spot.status, "unknown");
    spotData["rules"] = QJsonArray::fromStringList(alert.ruleNameList);

    QJsonObject dxInfo;
    dxInfo["call"] = alert.spot.callsign;
    dxInfo["country"] = alert.spot.dxcc.country;
    dxInfo["pfx"] = alert.spot.dxcc.prefix;
    dxInfo["dxcc"] = alert.spot.dxcc.dxcc;
    dxInfo["cont"] = alert.spot.dxcc.cont;
    dxInfo["cqz"] = alert.spot.dxcc.cqz;
    dxInfo["ituz"] = alert.spot.dxcc.ituz;
    dxInfo["utcoffset"] = alert.spot.dxcc.tz;
    dxInfo["member"] = QJsonArray::fromStringList(alert.spot.memberList2StringList());

    QJsonObject spotterInfo;
    spotterInfo["call"] = alert.spot.spotter;
    spotterInfo["country"] = alert.spot.dxcc_spotter.country;
    spotterInfo["pfx"] = alert.spot.dxcc_spotter.prefix;
    spotterInfo["dxcc"] = alert.spot.dxcc_spotter.dxcc;
    spotterInfo["cont"] = alert.spot.dxcc_spotter.cont;
    spotterInfo["cqz"] = alert.spot.dxcc_spotter.cqz;
    spotterInfo["ituz"] = alert.spot.dxcc_spotter.ituz;
    spotterInfo["utcoffset"] = alert.spot.dxcc_spotter.tz;

    spotData["spotter"] = spotterInfo;
    spotData["dx"] = dxInfo;

    msg["msgtype"] = "spotalert";
    msg["data"] = spotData;

}

WCYSpotNotificationMsg::WCYSpotNotificationMsg(const WCYSpot &spot, QObject *parent) :
    GenericNotificationMsg(parent)
{
    FCT_IDENTIFICATION;

    QJsonObject spotData;
    spotData["rcvtime"] = spot.time.toString("yyyyMMdd hh:mm:ss");
    spotData["K"] = spot.KIndex;
    spotData["expK"] = spot.expK;
    spotData["A"] = spot.AIndex;
    spotData["R"] = spot.RIndex;
    spotData["SFI"] = spot.SFI;
    spotData["SA"] = spot.SA;
    spotData["GMF"] = spot.GMF;
    spotData["Au"] = spot.Au;

    msg["msgtype"] = "wcyspot";
    msg["data"] = spotData;
}

WWVSpotNotificationMsg::WWVSpotNotificationMsg(const WWVSpot &spot, QObject *parent) :
    GenericNotificationMsg(parent)
{
    FCT_IDENTIFICATION;

    QJsonObject spotData;
    spotData["rcvtime"] = spot.time.toString("yyyyMMdd hh:mm:ss");
    spotData["SFI"] = spot.SFI;
    spotData["A"] = spot.AIndex;
    spotData["K"] = spot.KIndex;
    spotData["Info1"] = spot.info1;
    spotData["Info2"] = spot.info2;

    msg["msgtype"] = "wwvspot";
    msg["data"] = spotData;
}

ToAllSpotNotificationMsg::ToAllSpotNotificationMsg(const ToAllSpot &spot, QObject *parent) :
    GenericNotificationMsg(parent)
{
    FCT_IDENTIFICATION;

    QJsonObject spotData;
    spotData["rcvtime"] = spot.time.toString("yyyyMMdd hh:mm:ss");
    spotData["message"] = spot.message;

    QJsonObject spotterInfo;
    spotterInfo["call"] = spot.spotter;
    spotterInfo["country"] = spot.dxcc_spotter.country;
    spotterInfo["pfx"] = spot.dxcc_spotter.prefix;
    spotterInfo["dxcc"] = spot.dxcc_spotter.dxcc;
    spotterInfo["cont"] = spot.dxcc_spotter.cont;
    spotterInfo["cqz"] = spot.dxcc_spotter.cqz;
    spotterInfo["ituz"] = spot.dxcc_spotter.ituz;
    spotterInfo["utcoffset"] = spot.dxcc_spotter.tz;

    spotData["spotter"] = spotterInfo;

    msg["msgtype"] = "toallspot";
    msg["data"] = spotData;
}

RigStatusNotificationMsg::RigStatusNotificationMsg(const Rig::Status &status, QObject *parent) :
    GenericNotificationMsg(parent)
{
    FCT_IDENTIFICATION;

    QJsonObject vfoState;

    auto addIfNoEmpty = [&](const QString &key,
                            const QString &value,
                            bool addCond = true)
    {
        if ( !value.isEmpty() && addCond)
            vfoState[key] = value;
    };

    auto addBoolIfNoEmpty = [&](const QString &key,
                            const qint8 &value,
                            bool addCond = true)
    {
        if ( addCond )
            vfoState[key] = ( value != 0 );
    };

    auto addDoubleIfNoEmpty = [&](const QString &key,
                                const double &value,
                                bool addCond = true)
    {
        if ( value != 0.0 && addCond )
            vfoState[key] = value;
    };

    addIfNoEmpty("vfo", status.vfo);
    addIfNoEmpty("freq",  QString::number(status.freq, 'f', 5), status.freq != 0.0);
    addIfNoEmpty("mode", status.mode);
    addIfNoEmpty("submode", status.submode);
    addIfNoEmpty("rawmode", status.rawmode);
    addBoolIfNoEmpty("ptt", status.ptt, status.ptt != -1);
    addDoubleIfNoEmpty("rit", status.rit);
    addDoubleIfNoEmpty("xit", status.xit);
    addDoubleIfNoEmpty("bandwidth", status.bandwidth);

    QJsonArray vfoStates;
    vfoStates.append(vfoState);

    QJsonObject rigData;
    rigData["profile"] = status.profile;
    rigData["connected"] = status.isConnected;

    if ( !status.vfo.isEmpty() )
    {
        rigData["txvfo"] = status.vfo;
        rigData["rxvfo"] = status.vfo; // split mode is not supported yet
    }

    if ( status.power > 0.0 )
        rigData["txpower"] = status.power;

    if ( vfoState.size() > 0 )
        rigData["vfostates"] = vfoStates;

    if ( status.keySpeed > 0 )
        rigData["keyspeed"] = status.keySpeed;

    msg["msgtype"] = "rigstatus";
    msg["data"] = rigData;
}
