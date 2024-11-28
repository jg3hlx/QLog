#ifndef QLOG_DATA_DXSPOT_H
#define QLOG_DATA_DXSPOT_H

#include <QtCore>
#include "Dxcc.h"
#include "core/MembershipQE.h"
#include "data/BandPlan.h"

class DxSpot
{

public:

    QDateTime dateTime;
    QString callsign;
    QList<ClubInfo> callsign_member;
    double freq;
    QString band;
    QString modeGroupString;
    BandPlan::BandPlanMode bandPlanMode;
    QString spotter;
    QString comment;
    DxccEntity dxcc;
    DxccEntity dxcc_spotter;
    DxccStatus status;
    qulonglong dupeCount = 0;

    DxSpot() : freq(0.0),
        bandPlanMode(BandPlan::BAND_MODE_UNKNOWN),
        status(DxccStatus::UnknownStatus)
        {};

    DxSpot(const DxSpot &newSpot):
        dateTime(newSpot.dateTime),
        callsign(newSpot.callsign),
        callsign_member(newSpot.callsign_member),
        freq(newSpot.freq),
        band(newSpot.band),
        modeGroupString(newSpot.modeGroupString),
        bandPlanMode(newSpot.bandPlanMode),
        spotter(newSpot.spotter),
        comment(newSpot.comment),
        dxcc(newSpot.dxcc),
        dxcc_spotter(newSpot.dxcc_spotter),
        status(newSpot.status),
        dupeCount(newSpot.dupeCount) {};

    DxSpot& operator=(const DxSpot &newSpot)
    {
        if ( this == &newSpot )
            return *this;

        dateTime = newSpot.dateTime;
        callsign = newSpot.callsign;
        callsign_member = newSpot.callsign_member;
        freq = newSpot.freq;
        band = newSpot.band;
        modeGroupString = newSpot.modeGroupString;
        bandPlanMode = newSpot.bandPlanMode;
        spotter = newSpot.spotter;
        comment = newSpot.comment;
        dxcc = newSpot.dxcc;
        dxcc_spotter = newSpot.dxcc_spotter;
        status = newSpot.status;
        dupeCount = newSpot.dupeCount;

        return *this;
    }

    QStringList memberList2StringList() const
    {
        QStringList ret;
        for ( const ClubInfo &member : static_cast<const QList<ClubInfo>&>(callsign_member) )
            ret << member.getClubInfo();
        return ret;
    };

    QSet<QString> memberList2Set() const
    {
        QSet<QString> ret;

        for ( const ClubInfo &member : static_cast<const QList<ClubInfo>&>(callsign_member) )
            ret << member.getClubInfo();
        return ret;
    }

    operator QString() const
    {
        return QString("DxSpot")
               + "Country: " + dxcc.dxcc
               + "CQZ" + dxcc.cqz
               + "ITUZ" + dxcc.ituz
               + "Status: " + status
               + "ModeGroup: " + modeGroupString
               + "Band: " + band
               + "spotter Country: " + dxcc_spotter.dxcc
               + "Continent: " + dxcc.cont
               + "Spotter Continent: " + dxcc_spotter.cont
               + "Callsign: " + callsign
               + "Message: " + comment
               + "DX Member: " + memberList2StringList().join(", ");
    }
};

Q_DECLARE_METATYPE(DxSpot);

#endif // QLOG_DATA_DXSPOT_H
