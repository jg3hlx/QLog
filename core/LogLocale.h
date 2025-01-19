#ifndef QLOG_CORE_LOGLOCALE_H
#define QLOG_CORE_LOGLOCALE_H

#include <QSettings>
#include <QLocale>
#include <QRegularExpression>

class LogLocale : public QLocale
{
public:
    LogLocale();

    QString formatTimeLongWithoutTZ() const;
    QString formatTimeShort() const;
    QString formatTimeLong() const;
    QString formatDateShortWithYYYY() const;
    QString formatDateTimeShortWithYYYY() const;
    bool getSettingUse24hformat() const;
    void setSettingUse24hformat(bool value);

private:
    const QRegularExpression regexp;
    bool is24hUsed;
    QSettings settings;

    void changeTime12_24Format(QString &formatString) const;
};

#endif // QLOG_CORE_LOGLOCALE_H
