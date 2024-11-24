#ifndef QLOG_MODELS_LOGBOOKMODEL_H
#define QLOG_MODELS_LOGBOOKMODEL_H

#include <QObject>
#include <QSqlTableModel>

class LogbookModel : public QSqlTableModel
{
    Q_OBJECT

public:
    explicit LogbookModel(QObject* parent = nullptr, QSqlDatabase db = QSqlDatabase());

    QVariant data(const QModelIndex &index, int role) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role) override;
    void updateExternalServicesUploadStatus( const QModelIndex &index, int role, bool &updateResult );
    void updateUploadToModified( const QModelIndex &index, int role, int column, bool &updateResult );
    enum ColumnID
    {
        COLUMN_INVALID = -1,
        COLUMN_ID = 0,
        COLUMN_TIME_ON = 1,
        COLUMN_TIME_OFF = 2,
        COLUMN_CALL = 3,
        COLUMN_RST_SENT = 4,
        COLUMN_RST_RCVD = 5,
        COLUMN_FREQUENCY = 6,
        COLUMN_BAND = 7,
        COLUMN_MODE = 8,
        COLUMN_SUBMODE = 9,
        COLUMN_NAME = 10,
        COLUMN_QTH = 11,
        COLUMN_GRID = 12,
        COLUMN_DXCC = 13,
        COLUMN_COUNTRY = 14,
        COLUMN_CONTINENT = 15,
        COLUMN_CQZ = 16,
        COLUMN_ITUZ = 17,
        COLUMN_PREFIX = 18,
        COLUMN_STATE = 19,
        COLUMN_COUNTY = 20,
        COLUMN_IOTA = 21,
        COLUMN_QSL_RCVD = 22,
        COLUMN_QSL_RCVD_DATE = 23,
        COLUMN_QSL_SENT = 24,
        COLUMN_QSL_SENT_DATE = 25,
        COLUMN_LOTW_RCVD = 26,
        COLUMN_LOTW_RCVD_DATE = 27,
        COLUMN_LOTW_SENT = 28,
        COLUMN_LOTW_SENT_DATE = 29,
        COLUMN_TX_POWER = 30,
        COLUMN_FIELDS = 31,
        COLUMN_ADDRESS = 32,
        COLUMN_ADDRESS_INTL = 33,
        COLUMN_AGE = 34,
        COLUMN_A_INDEX = 35,
        COLUMN_ANT_AZ = 36,
        COLUMN_ANT_EL = 37,
        COLUMN_ANT_PATH = 38,
        COLUMN_ARRL_SECT = 39,
        COLUMN_AWARD_SUBMITTED = 40,
        COLUMN_AWARD_GRANTED = 41,
        COLUMN_BAND_RX = 42,
        COLUMN_CHECK = 43,
        COLUMN_CLASS = 44,
        COLUMN_CLUBLOG_QSO_UPLOAD_DATE = 45,
        COLUMN_CLUBLOG_QSO_UPLOAD_STATUS = 46,
        COLUMN_COMMENT = 47,
        COLUMN_COMMENT_INTL = 48,
        COLUMN_CONTACTED_OP = 49,
        COLUMN_CONTEST_ID = 50,
        COLUMN_COUNTRY_INTL = 51,
        COLUMN_CREDIT_SUBMITTED = 52,
        COLUMN_CREDIT_GRANTED = 53,
        COLUMN_DARC_DOK = 54,
        COLUMN_DISTANCE = 55,
        COLUMN_EMAIL = 56,
        COLUMN_EQ_CALL = 57,
        COLUMN_EQSL_QSLRDATE = 58,
        COLUMN_EQSL_QSLSDATE = 59,
        COLUMN_EQSL_QSL_RCVD = 60,
        COLUMN_EQSL_QSL_SENT = 61,
        COLUMN_FISTS = 62,
        COLUMN_FISTS_CC = 63,
        COLUMN_FORCE_INIT = 64,
        COLUMN_FREQ_RX = 65,
        COLUMN_GUEST_OP = 66,
        COLUMN_HRDLOG_QSO_UPLOAD_DATE = 67,
        COLUMN_HRDLOG_QSO_UPLOAD_STATUS = 68,
        COLUMN_IOTA_ISLAND_ID = 69,
        COLUMN_K_INDEX = 70,
        COLUMN_LAT = 71,
        COLUMN_LON = 72,
        COLUMN_MAX_BURSTS = 73,
        COLUMN_MS_SHOWER = 74,
        COLUMN_MY_ANTENNA = 75,
        COLUMN_MY_ANTENNA_INTL = 76,
        COLUMN_MY_CITY = 77,
        COLUMN_MY_CITY_INTL = 78,
        COLUMN_MY_CNTY = 79,
        COLUMN_MY_COUNTRY = 80,
        COLUMN_MY_COUNTRY_INTL = 81,
        COLUMN_MY_CQ_ZONE = 82,
        COLUMN_MY_DXCC = 83,
        COLUMN_MY_FISTS = 84,
        COLUMN_MY_GRIDSQUARE = 85,
        COLUMN_MY_IOTA = 86,
        COLUMN_MY_IOTA_ISLAND_ID = 87,
        COLUMN_MY_ITU_ZONE = 88,
        COLUMN_MY_LAT = 89,
        COLUMN_MY_LON = 90,
        COLUMN_MY_NAME = 91,
        COLUMN_MY_NAME_INTL = 92,
        COLUMN_MY_POSTAL_CODE = 93,
        COLUMN_MY_POSTAL_CODE_INTL = 94,
        COLUMN_MY_RIG = 95,
        COLUMN_MY_RIG_INTL = 96,
        COLUMN_MY_SIG = 97,
        COLUMN_MY_SIG_INTL = 98,
        COLUMN_MY_SIG_INFO = 99,
        COLUMN_MY_SIG_INFO_INTL = 100,
        COLUMN_MY_SOTA_REF = 101,
        COLUMN_MY_STATE = 102,
        COLUMN_MY_STREET = 103,
        COLUMN_MY_STREET_INTL = 104,
        COLUMN_MY_USACA_COUNTIES = 105,
        COLUMN_MY_VUCC_GRIDS = 106,
        COLUMN_NAME_INTL = 107,
        COLUMN_NOTES = 108,
        COLUMN_NOTES_INTL = 109,
        COLUMN_NR_BURSTS = 110,
        COLUMN_NR_PINGS = 111,
        COLUMN_OPERATOR = 112,
        COLUMN_OWNER_CALLSIGN = 113,
        COLUMN_PRECEDENCE = 114,
        COLUMN_PROP_MODE = 115,
        COLUMN_PUBLIC_KEY = 116,
        COLUMN_QRZCOM_QSO_UPLOAD_DATE = 117,
        COLUMN_QRZCOM_QSO_UPLOAD_STATUS = 118,
        COLUMN_QSLMSG = 119,
        COLUMN_QSLMSG_INTL = 120,
        COLUMN_QSL_RCVD_VIA = 121,
        COLUMN_QSL_SENT_VIA = 122,
        COLUMN_QSL_VIA = 123,
        COLUMN_QSO_COMPLETE = 124,
        COLUMN_QSO_RANDOM = 125,
        COLUMN_QTH_INTL = 126,
        COLUMN_REGION = 127,
        COLUMN_RIG = 128,
        COLUMN_RIG_INTL = 129,
        COLUMN_RX_PWR = 130,
        COLUMN_SAT_MODE = 131,
        COLUMN_SAT_NAME = 132,
        COLUMN_SFI = 133,
        COLUMN_SIG = 134,
        COLUMN_SIG_INTL = 135,
        COLUMN_SIG_INFO = 136,
        COLUMN_SIG_INFO_INTL = 137,
        COLUMN_SILENT_KEY = 138,
        COLUMN_SKCC = 139,
        COLUMN_SOTA_REF = 140,
        COLUMN_SRX = 141,
        COLUMN_SRX_STRING = 142,
        COLUMN_STATION_CALLSIGN = 143,
        COLUMN_STX = 144,
        COLUMN_STX_STRING = 145,
        COLUMN_SWL = 146,
        COLUMN_TEN_TEN = 147,
        COLUMN_UKSMG = 148,
        COLUMN_USACA_COUNTIES = 149,
        COLUMN_VE_PROV = 150,
        COLUMN_VUCC_GRIDS = 151,
        COLUMN_WEB = 152,
        COLUMN_MY_ARRL_SECT = 153,
        COLUMN_MY_WWFF_REF = 154,
        COLUMN_WWFF_REF = 155,
        COLUMN_ALTITUDE = 156,
        COLUMN_GRID_EXT = 157,
        COLUMN_HAMLOGEU_QSO_UPLOAD_DATE = 158,
        COLUMN_HAMLOGEU_QSO_UPLOAD_STATUS = 159,
        COLUMN_HAMQTH_QSO_UPLOAD_DATE = 160,
        COLUMN_HAMQTH_QSO_UPLOAD_STATUS = 161,
        COLUMN_MY_ALTITUDE = 162,
        COLUMN_MY_GRIDSQUARE_EXT = 163,
        COLUMN_MY_POTA_REF = 164,
        COLUMN_POTA_REF = 165,
        COLUMN_LAST_ELEMENT = 166
    };

private:
    static QHash<LogbookModel::ColumnID, const char *> fieldNameTranslationMap;

public:
    static QString getFieldNameTranslation(const LogbookModel::ColumnID key)
    {
        const char *value = fieldNameTranslationMap.value(key);
        return value ? tr(value) : QString();
    }

    static const QStringList getAllFieldNamesTranslation()
    {
        QStringList ret;
        for (auto it = fieldNameTranslationMap.begin(); it != fieldNameTranslationMap.end(); ++it)
            ret.append(getFieldNameTranslation(it.key()));

        return ret;
    }
};

#endif // QLOG_MODELS_LOGBOOKMODEL_H
