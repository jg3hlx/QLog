#include <QDebug>
#include <QSettings>
#include <cstring>
#include <hamlib/rig.h>
#include "Rig.h"

#ifndef HAMLIB_FILPATHLEN
#define HAMLIB_FILPATHLEN FILPATHLEN
#endif

static QString modeToString(rmode_t mode) {
    switch (mode) {
    case RIG_MODE_AM: return "AM";
    case RIG_MODE_CW: return "CW";
    case RIG_MODE_USB: return "USB";
    case RIG_MODE_LSB: return "LSB";
    case RIG_MODE_FM: return "FM";
    default: return "";
    }
}

static enum serial_handshake_e stringToFlowControl(const QString in_flowcontrol)
{
    QString flowcontrol = in_flowcontrol.toLower();

    if ( flowcontrol == "software" ) return RIG_HANDSHAKE_XONXOFF;
    if ( flowcontrol == "hardware" ) return RIG_HANDSHAKE_HARDWARE;
    return RIG_HANDSHAKE_NONE;
}

static enum serial_parity_e stringToParity(const QString in_parity)
{
    QString parity = in_parity.toLower();

    if ( parity == "even" ) return RIG_PARITY_EVEN;
    if ( parity == "odd" ) return RIG_PARITY_ODD;
    if ( parity == "mark" ) return RIG_PARITY_MARK;
    if ( parity == "space" ) return RIG_PARITY_SPACE;

    return RIG_PARITY_NONE;
}

Rig* Rig::instance() {
    static Rig instance;
    return &instance;
}

void Rig::start() {
    QTimer* timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(update()));
    timer->start(500);
}

void Rig::update() {
    if (!rig) return;

    if (!rigLock.tryLock(200)) return;

    freq_t vfo_freq;

    if (rig_get_freq(rig, RIG_VFO_CURR, &vfo_freq) == RIG_OK) {
        int new_freq = static_cast<int>(vfo_freq);

        if (new_freq != freq_rx) {
            freq_rx = new_freq;
            emit frequencyChanged(freq_rx/1e6);
        }
    }


    rmode_t modeId;
    pbwidth_t pbwidth;

    if (rig_get_mode(rig, RIG_VFO_CURR, &modeId, &pbwidth) == RIG_OK) {
        QString new_mode = modeToString(modeId);
        if (new_mode != mode_rx)  {
            mode_rx = new_mode;
            emit modeChanged(mode_rx);
        }
    }

    value_t rigPowerLevel;
    unsigned int rigPower;
    rig_get_level(rig, RIG_VFO_CURR, RIG_LEVEL_RFPOWER, &rigPowerLevel);
    rig_power2mW(rig, &rigPower, rigPowerLevel.f, freq_rx, modeId);

    if (rigPower != power) {
        power = rigPower;
        emit powerChanged(power/1000.0);
    }

    rigLock.unlock();
}

void Rig::open() {
    QSettings settings;
    int model = settings.value("hamlib/rig/model").toInt();
    int baudrate = settings.value("hamlib/rig/baudrate").toInt();
    int databits = settings.value("hamlib/rig/databits").toInt();
    float stopbits = settings.value("hamlib/rig/stopbits").toFloat();
    QString flowControl = settings.value("hamlib/rig/stopbits").toString();
    QString parity = settings.value("hamlib/rig/parity").toString();
    QByteArray portStr = settings.value("hamlib/rig/port").toByteArray();
    QString hostname = settings.value("hamlib/rig/hostname").toString();
    int netport = settings.value("hamlib/rig/netport").toInt();

    const char* port = portStr.constData();

    qDebug() << portStr;
    rigLock.lock();

    // if rig is active then close it
    //close(); // do not call close here because rig is already locked by mutex
    if (rig)
    {
        rig_close(rig);
        rig_cleanup(rig);
    }

    rig = rig_init(model);

    if (!rig)
    {
        // initialization failed
        return;
    }

    if (rig->caps->port_type == RIG_PORT_NETWORK
        || rig->caps->port_type == RIG_PORT_UDP_NETWORK )
    {
        //handling Network Radio
        strncpy(rig->state.rigport.pathname, hostname.toLocal8Bit().constData(), HAMLIB_FILPATHLEN - 1);
        //port is hardcoded in hamlib - not necessary to set it.
        (void)netport;

    }
    else
    {
        //handling Serial Port Radio
        strncpy(rig->state.rigport.pathname, port, HAMLIB_FILPATHLEN - 1);
        rig->state.rigport.parm.serial.rate = baudrate;
        rig->state.rigport.parm.serial.data_bits = databits;
        rig->state.rigport.parm.serial.stop_bits = stopbits;
        rig->state.rigport.parm.serial.handshake = stringToFlowControl(flowControl);
        rig->state.rigport.parm.serial.parity = stringToParity(parity);
    }

    int status = rig_open(rig);

    rigLock.unlock();

    if (status != RIG_OK) {
        qWarning() << "rig connection error";
    }
    else {
        qDebug() << "connected to rig";
    }
}

void Rig::close()
{
    rigLock.lock();
    if (rig)
    {
        rig_close(rig);
        rig_cleanup(rig);
        rig = nullptr;
    }
    rigLock.unlock();
}

void Rig::setFrequency(double newFreq) {
    if (!rig) return;
    return;

    rigLock.lock();
    freq_rx = static_cast<int>(newFreq*1e6);
    rig_set_freq(rig, RIG_VFO_CURR, freq_rx);
    rigLock.unlock();
}

void Rig::setMode(QString newMode) {
    if (!rig) return;
    return;

    rigLock.lock();
    mode_rx = newMode;
    if (newMode == "CW") {
        rig_set_mode(rig, RIG_VFO_CURR, RIG_MODE_CW, RIG_PASSBAND_NORMAL);
    }
    else if (newMode == "SSB") {
        if (freq_rx < 10) {
            rig_set_mode(rig, RIG_VFO_CURR, RIG_MODE_LSB, RIG_PASSBAND_NORMAL);
        }
        else {
            rig_set_mode(rig, RIG_VFO_CURR, RIG_MODE_USB, RIG_PASSBAND_NORMAL);
        }
    }
    else if (newMode == "AM") {
        rig_set_mode(rig, RIG_VFO_CURR, RIG_MODE_AM, RIG_PASSBAND_NORMAL);
    }
    else if (newMode == "FM") {
        rig_set_mode(rig, RIG_VFO_CURR, RIG_MODE_FM, RIG_PASSBAND_NORMAL);
    }

    rigLock.unlock();
}

void Rig::setPower(double newPower) {
    if (!rig) return;
    power = (int)(newPower*1000);
}
