#include "LogLocale.h"
#include "core/debug.h"

MODULE_IDENTIFICATION("qlog.core.loglocale");

LogLocale::LogLocale() :
    regexp(QRegularExpression(R"(, tttt|\(t\)|\bt\b)")),
    is24hUsed(!timeFormat(QLocale::ShortFormat).contains("ap", Qt::CaseInsensitive))
{
    FCT_IDENTIFICATION;
}

void LogLocale::changeTime12_24Format(QString &formatString) const
{
    if ( getSettingUse24hformat() )
        formatString.remove("ap", Qt::CaseInsensitive).remove("a", Qt::CaseInsensitive);
    else if ( is24hUsed )
        formatString += " AP";
}

QString LogLocale::formatTimeLongWithoutTZ() const
{
    FCT_IDENTIFICATION;

    QString ret = timeFormat(QLocale::LongFormat).remove(regexp);

    changeTime12_24Format(ret);
    qCDebug(runtime) << "format:" << ret;
    return ret;
}

QString LogLocale::formatTimeShort() const
{
    FCT_IDENTIFICATION;

    QString ret = timeFormat(QLocale::ShortFormat);

    changeTime12_24Format(ret);
    qCDebug(runtime) << "format:" << ret;
    return ret;
}

QString LogLocale::formatTimeLong() const
{
    FCT_IDENTIFICATION;

    QString ret = formatTimeLongWithoutTZ()
#if (QT_VERSION < QT_VERSION_CHECK(6, 5, 0))
                                           .append(" t");
#else
                                           .append(" ttt");
#endif

    qCDebug(runtime) << "format:" << ret;
    return ret;
}

QString LogLocale::formatDateShortWithYYYY() const
{
    FCT_IDENTIFICATION;

    QString ret = dateFormat(QLocale::ShortFormat);

    if ( ret.contains("yy") && !ret.contains("yyyy") )
        ret = ret.replace("yy", "yyyy");

    qCDebug(runtime) << "format:" << ret;
    return ret;
}

QString LogLocale::formatDateTimeShortWithYYYY() const
{
    FCT_IDENTIFICATION;

    QString ret = formatDateShortWithYYYY() + " " +formatTimeShort();

    qCDebug(runtime) << "format:" << ret;
    return ret;
}

bool LogLocale::getSettingUse24hformat() const
{
    return settings.value("use24hformat", is24hUsed).toBool();
}

void LogLocale::setSettingUse24hformat(bool value)
{
    settings.setValue("use24hformat", value);
}
