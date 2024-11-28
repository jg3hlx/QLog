#ifndef QLOG_DATA_WSJTXENTRY_H
#define QLOG_DATA_WSJTXENTRY_H

#include "core/Wsjtx.h"
#include "data/DxSpot.h"

class WsjtxEntry : public DxSpot
{

public:

    WsjtxDecode decode;
    QString grid;
    double distance;
    QDateTime receivedTime;
    QString decodedMode;

    WsjtxEntry() : DxSpot(), distance(0.0){};

    WsjtxEntry(const DxSpot &other) :
        DxSpot(other) {};

    WsjtxEntry(const WsjtxEntry &other) :
        DxSpot(other),
        decode(other.decode),
        grid(other.grid),
        distance(other.distance),
        receivedTime(other.receivedTime),
        decodedMode(other.decodedMode)
    {
        bandPlanMode = ((decodedMode == "FT8")  ? BandPlan::BAND_MODE_FT8
                                               : BandPlan::BAND_MODE_DIGITAL );
        modeGroupString = BandPlan::bandMode2BandModeGroupString(bandPlanMode);
    } ;

    WsjtxEntry& operator=(const WsjtxEntry &newSpot)
    {
        if ( this == &newSpot )
            return *this;

        DxSpot::operator=(newSpot);

        decode = newSpot.decode;
        grid = newSpot.grid;
        distance = newSpot.distance;
        receivedTime = newSpot.receivedTime;
        decodedMode = newSpot.decodedMode;

        return *this;
    };

    operator QString() const
    {
        return QString("WsjtxEntry")
        + "Country: " + dxcc.dxcc
        + "CQZ" + dxcc.cqz
        + "ITUZ" + dxcc.ituz
        + "Status: " + status
        + "Band: " + band
        + "ModeGroup: " + ((decodedMode == "FT8") ? BandPlan::MODE_GROUP_STRING_FT8
                                                  : BandPlan::MODE_GROUP_STRING_DIGITAL )
        + "spotter Country: " + dxcc_spotter.dxcc
        + "Continent: " + dxcc.cont
        + "Spotter Continent: " + dxcc_spotter.cont
        + "Callsign: " + callsign
        + "Message: " + decode.message
        + "DX Member: " + memberList2StringList().join(", ");
    };
};

#endif // QLOG_DATA_WSJTXENTRY_H
