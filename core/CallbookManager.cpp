#include <QSettings>
#include <QCache>
#include "CallbookManager.h"
#include "core/debug.h"
#include "core/HamQTH.h"
#include "core/QRZ.h"
#include "core/Callsign.h"

MODULE_IDENTIFICATION("qlog.ui.callbookmanager");

CallbookManager::CallbookManager(QObject *parent)
    : QObject{parent},
    primaryCallbookAuthSuccess(false),
    secondaryCallbookAuthSuccess(false)
{
    FCT_IDENTIFICATION;

    initCallbooks();
}

void CallbookManager::queryCallsign(const QString &callsign)
{
    FCT_IDENTIFICATION;

    qCDebug(function_parameters) << callsign;

    if ( queryCache.contains(callsign) )
    {
        emit callsignResult(QMap<QString, QString>(*queryCache.object(callsign)));
        return;
    }

    // create an empty object in cache
    // if there is the second query for the same call immediatelly after
    // the first query, then it returns a result of empty object
    queryCache.insert(callsign, new QMap<QString, QString>());

    if ( !primaryCallbook.isNull() )
    {
        currentQueryCallsign = callsign;
        primaryCallbook->queryCallsign(currentQueryCallsign);
    }
    else
    {
        queryCache.remove(callsign);
        emit callsignNotFound(callsign);
    }
}

bool CallbookManager::isActive()
{
    FCT_IDENTIFICATION;

    bool ret = primaryCallbookAuthSuccess || secondaryCallbookAuthSuccess;
    qCDebug(runtime) << ret;
    return ret;
}

GenericCallbook *CallbookManager::createCallbook(const QString &callbookID)
{
    FCT_IDENTIFICATION;

    GenericCallbook *ret = nullptr;

    if (callbookID == HamQTH::CALLBOOK_NAME )
    {
        ret = new HamQTH(this);
    }
    else if ( callbookID == QRZ::CALLBOOK_NAME )
    {
        ret = new QRZ(this);
    }

    if ( ret )
    {
        connect(ret, &GenericCallbook::callsignResult, this, &CallbookManager::processCallsignResult);
    }

    return ret;
}

void CallbookManager::initCallbooks()
{
    QSettings settings;

    primaryCallbook.clear();
    secondaryCallbook.clear();
    primaryCallbookAuthSuccess = false;
    secondaryCallbookAuthSuccess = false;

    queryCache.clear();
    currentQueryCallsign = QString();

    QString primaryCallbookSelection = settings.value(GenericCallbook::CONFIG_PRIMARY_CALLBOOK_KEY).toString();
    QString secondaryCallbookSelection = settings.value(GenericCallbook::CONFIG_SECONDARY_CALLBOOK_KEY).toString();

    primaryCallbook = createCallbook(primaryCallbookSelection);
    secondaryCallbook = createCallbook(secondaryCallbookSelection);

    if ( !primaryCallbook.isNull() )
    {
        connect(primaryCallbook, &GenericCallbook::callsignNotFound, this, &CallbookManager::primaryCallbookCallsignNotFound);
        connect(primaryCallbook, &GenericCallbook::loginFailed, this, [this]()
        {
            primaryCallbookAuthSuccess = false;
            emit loginFailed(primaryCallbook->getDisplayName());
        });
        connect(primaryCallbook, &GenericCallbook::lookupError, this, [this](const QString &error)
        {
            emit lookupError(primaryCallbook->getDisplayName()
                             + " - " + error
                             + ((!secondaryCallbook.isNull()) ? tr("<p>The secondary callbook will be used</p>") : ""));
            primaryCallbookCallsignNotFound(currentQueryCallsign);
        });
        primaryCallbookAuthSuccess = true;
    }

    if ( !secondaryCallbook.isNull() )
    {
        connect(secondaryCallbook, &GenericCallbook::callsignNotFound, this, &CallbookManager::secondaryCallbookCallsignNotFound);
        connect(secondaryCallbook, &GenericCallbook::loginFailed, this, [this]()
        {
            secondaryCallbookAuthSuccess = false;
            emit loginFailed(secondaryCallbook->getDisplayName());
        });
        connect(secondaryCallbook, &GenericCallbook::lookupError, this, [this](const QString &error)
        {
            emit lookupError(secondaryCallbook->getDisplayName() + " - " + error);
            secondaryCallbookCallsignNotFound(currentQueryCallsign);
        });
        secondaryCallbookAuthSuccess = true;
    }
}

void CallbookManager::abortQuery()
{
    FCT_IDENTIFICATION;

    queryCache.remove(currentQueryCallsign);
    if ( ! primaryCallbook.isNull() ) primaryCallbook->abortQuery();
    if ( ! secondaryCallbook.isNull() ) secondaryCallbook->abortQuery();
}

void CallbookManager::primaryCallbookCallsignNotFound(const QString &notFoundCallsign)
{
    FCT_IDENTIFICATION;

    qCDebug(function_parameters) << notFoundCallsign;

    queryCache.remove(notFoundCallsign);

    if ( notFoundCallsign != currentQueryCallsign ) return ;

    if ( secondaryCallbook.isNull() )
    {
        emit callsignNotFound(notFoundCallsign);
        return;
    }

    qCDebug(runtime) << "Callsign not found - primary - trying the secondary callbook";

    secondaryCallbook->queryCallsign(notFoundCallsign);
}

void CallbookManager::secondaryCallbookCallsignNotFound(const QString &notFoundCallsign)
{
    FCT_IDENTIFICATION;

    qCDebug(function_parameters) << notFoundCallsign;

    queryCache.remove(notFoundCallsign);

    if ( notFoundCallsign != currentQueryCallsign ) return ;

    qCDebug(runtime) << "Callsign not found - secondary ";

    emit callsignNotFound(notFoundCallsign);
}

void CallbookManager::processCallsignResult(const QMap<QString, QString> &data)
{
    FCT_IDENTIFICATION;

    queryCache.insert(data["call"], new QMap<QString, QString>(data));

    // Callbook returned queried callsign
    if ( data["call"] == currentQueryCallsign )
    {
        emit callsignResult(data);
        return;
    }

    Callsign queryCall(currentQueryCallsign);

    if ( ! queryCall.isValid() )
    {
        qCDebug(runtime) << "Query callsign is not valid " << currentQueryCallsign;
        return;
    }

    // If not exists full match record for example for SP/OK1xxx in a callbook
    // then callbooks return a partial result (usually base callsign) OK1xxx. In this case, QLog
    // takes only selected fields from the callbook response.

    if ( queryCall.getBase() == data["call"] )
    {
        qCDebug(runtime) << "Partial match for result - forwarding limited set of information";

        QMap<QString, QString> newdata;

        newdata["call"] = currentQueryCallsign;
        newdata["fname"] = data["fname"];
        newdata["lname"] = data["lname"];
        newdata["lic_year"] = data["lic_year"];
        newdata["qsl_via"] = data["qsl_via"];
        newdata["email"] = data["email"];
        newdata["born"] = data["born"];
        newdata["name"] = data["name"];
        newdata["url"] = data["url"];
        newdata["name_fmt"] = data["name_fmt"];
        newdata["nick"] = data["nick"];
        newdata["image_url"] = data["image_url"];

        queryCache.insert(currentQueryCallsign, new QMap<QString, QString>(newdata));
        emit callsignResult(newdata);
    }
}

QCache<QString, QMap<QString, QString>> CallbookManager::queryCache(100);
