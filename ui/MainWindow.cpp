#include <QMessageBox>
#include <QLabel>
#include <QColor>
#include <QSpacerItem>
#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "ui/SettingsDialog.h"
#include "ui/ImportDialog.h"
#include "ui/ExportDialog.h"
#include "ui/LotwDialog.h"
#include "core/Fldigi.h"
#include "rig/Rig.h"
#include "rotator/Rotator.h"
#include "cwkey/CWKeyer.h"
#include "core/Wsjtx.h"
#include "core/debug.h"
#include "ui/NewContactWidget.h"
#include "ui/QSOFilterDialog.h"
#include "ui/Eqsldialog.h"
#include "ui/ClublogDialog.h"
#include "ui/QrzDialog.h"
#include "ui/AwardsDialog.h"
#include "core/Lotw.h"
#include "core/Eqsl.h"
#include "core/QRZ.h"
#include "core/PropConditions.h"
#include "data/MainLayoutProfile.h"
#include "ui/EditActivitiesDialog.h"
#include "core/HRDLog.h"
#include "ui/HRDLogDialog.h"
#include "ui/ProfileImageWidget.h"
#include "core/LogParam.h"
#include "core/QSOFilterManager.h"
#include "data/Data.h"
#include "data/ActivityProfile.h"
#include "data/AntProfile.h"
#include "data/RigProfile.h"
#include "data/RotProfile.h"

MODULE_IDENTIFICATION("qlog.ui.mainwindow");

MainWindow::MainWindow(QWidget* parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    stats(new StatisticsWidget),
    clublogRT(new ClubLog(this))
{
    FCT_IDENTIFICATION;

    ui->setupUi(this);

    restoreContestMenuSeqnoType();
    restoreContestMenuDupeType();
    restoreContestMenuLinkExchange();

    darkLightModeSwith = new SwitchButton("", ui->statusBar);
    darkIconLabel = new QLabel("<html><img src=':/icons/light-dark-24px.svg'></html>",ui->statusBar);

    /* Dark Mode is supported only in case of Fusion Style */
    if ( QApplication::style()->objectName().compare("fusion",
                                                     Qt::CaseSensitivity::CaseInsensitive) != 0)
    {
        isFusionStyle = false;
        darkLightModeSwith->setEnabled(false);
        darkIconLabel->setEnabled(false);
        darkLightModeSwith->setToolTip(tr("Not enabled for non-Fusion style"));
        darkModeToggle(Qt::Unchecked);

    }
    else
    {
        isFusionStyle = true;
    }

    /* the block below is present because the main window
     * becomes large after the instalation
     */
    ui->wsjtxDockWidget->hide();
    ui->rotatorDockWidget->hide();
    ui->bandmapDockWidget->hide();
    ui->mapDockWidget->hide();
    ui->dxDockWidget->hide();
    ui->rigDockWidget->hide();
    ui->cwConsoleDockWidget->hide();
    ui->chatDockWidget->hide();
    ui->profileImageDockWidget->hide();
    ui->alertDockWidget->hide();

    ui->cwconsoleWidget->registerContactWidget(ui->newContactWidget);
    ui->rotatorWidget->registerContactWidget(ui->newContactWidget);
    ui->onlineMapWidget->registerContactWidget(ui->newContactWidget);
    ui->chatWidget->registerContactWidget(ui->newContactWidget);

    setLayoutGeometry();

    const StationProfile &profile = StationProfilesManager::instance()->getCurProfile1();

    activityButton = new QPushButton("", ui->statusBar);
    activityButton->setFlat(true);
    activityButton->setFocusPolicy(Qt::NoFocus);
    QMenu *activityMenu = new QMenu(activityButton);
    activityButton->setMenu(activityMenu);

    conditionsLabel = new QLabel("", ui->statusBar);
    conditionsLabel->setIndent(20);
    conditionsLabel->setToolTip(QString("<img src='%1'>").arg(PropConditions::solarSummaryFile()));
    profileLabel = new QLabel("<b>" + profile.profileName + ":</b>", ui->statusBar);
    profileLabel->setIndent(10);
    callsignLabel = new QLabel(stationCallsignStatus(profile), ui->statusBar);
    locatorLabel = new QLabel(profile.locator.toLower(), ui->statusBar);
    contestLabel = new QLabel(ui->statusBar);
    contestLabel->setIndent(20);
    alertButton = new QPushButton("0", ui->statusBar);
    alertButton->setIcon(QIcon(":/icons/alert.svg"));
    alertButton->setFlat(true);
    alertButton->setFocusPolicy(Qt::NoFocus);
    QMenu *menuAlert = new QMenu(this);
    menuAlert->addAction(ui->actionShowAlerts);
    menuAlert->addAction(ui->actionClearAlerts);
    menuAlert->addSeparator();
    menuAlert->addAction(ui->actionEditAlertRules);
    menuAlert->addAction(ui->actionBeepSettingAlert);
    ui->actionBeepSettingAlert->setChecked(settings.value("alertbeep", false).toBool());
    alertButton->setMenu(menuAlert);

    alertTextButton = new QPushButton(" ", ui->statusBar);
    alertTextButton->setFlat(true);
    alertTextButton->setFocusPolicy(Qt::NoFocus);
    alertTextButton->setToolTip(tr("Press to tune the alert"));

    ui->toolBar->hide();
    ui->statusBar->addWidget(activityButton);
    ui->statusBar->addWidget(profileLabel);
    ui->statusBar->addWidget(callsignLabel);
    ui->statusBar->addWidget(locatorLabel);
    ui->statusBar->addWidget(contestLabel);
    ui->statusBar->addWidget(conditionsLabel);

    ui->statusBar->addPermanentWidget(alertTextButton);
    ui->statusBar->addPermanentWidget(alertButton);
    ui->statusBar->addPermanentWidget(darkLightModeSwith);
    ui->statusBar->addPermanentWidget(darkIconLabel);

    setContestMode(LogParam::getParam("contest/contestid", QString()).toString());

    connect(seqGroup, &QActionGroup::triggered, this, &MainWindow::saveContestMenuSeqnoType);
    connect(dupeGroup, &QActionGroup::triggered, this, &MainWindow::saveContestMenuDupeType);
    connect(linkExchangeGroup, &QActionGroup::triggered, this, &MainWindow::saveContestMenuLinkExchangeType);
    
    connect(ActivityProfilesManager::instance(), &ActivityProfilesManager::changeFinished,
            this, &MainWindow::handleActivityChange);
    connect(ActivityProfilesManager::instance(), &ActivityProfilesManager::changeFinished,
            ui->newContactWidget, &NewContactWidget::setValuesFromActivity);

    connect(AntProfilesManager::instance(), &AntProfilesManager::profileChanged,
            ui->newContactWidget, &NewContactWidget::refreshAntProfileCombo);
    connect(AntProfilesManager::instance(), &AntProfilesManager::profileChanged,
            ui->onlineMapWidget, &OnlineMapWidget::flyToMyQTH);

    connect(RotProfilesManager::instance(), &RotProfilesManager::profileChanged,
            ui->rotatorWidget, &RotatorWidget::refreshRotProfileCombo);

    connect(RigProfilesManager::instance(), &RigProfilesManager::profileChanged,
            ui->newContactWidget, &NewContactWidget::refreshRigProfileCombo);
    connect(RigProfilesManager::instance(), &RigProfilesManager::profileChanged,
            ui->rigWidget, &RigWidget::refreshRigProfileCombo);

    connect(MainLayoutProfilesManager::instance(), &MainLayoutProfilesManager::profileChanged,
            this, &MainWindow::setSimplyLayoutGeometry);
    connect(MainLayoutProfilesManager::instance(), &MainLayoutProfilesManager::profileChanged,
            ui->newContactWidget, &NewContactWidget::setupCustomUi);

    connect(StationProfilesManager::instance(), &StationProfilesManager::profileChanged,
            this, &MainWindow::stationProfileChanged);
    connect(StationProfilesManager::instance(), &StationProfilesManager::profileChanged,
            ui->newContactWidget, &NewContactWidget::refreshStationProfileCombo);
    connect(StationProfilesManager::instance(), &StationProfilesManager::profileChanged,
            ui->rotatorWidget, &RotatorWidget::redrawMap);
    connect(StationProfilesManager::instance(), &StationProfilesManager::profileChanged,
            ui->onlineMapWidget, &OnlineMapWidget::flyToMyQTH);
    connect(StationProfilesManager::instance(), &StationProfilesManager::profileChanged,
            ui->chatWidget, &ChatWidget::reloadStationProfile);
    connect(StationProfilesManager::instance(), &StationProfilesManager::profileChanged,
            ui->clockWidget, &ClockWidget::updateSun);

    connect(this, &MainWindow::themeChanged, ui->bandmapWidget, &BandmapWidget::update);
    connect(this, &MainWindow::themeChanged, ui->clockWidget, &ClockWidget::updateClock);
    connect(this, &MainWindow::themeChanged, ui->onlineMapWidget, &OnlineMapWidget::changeTheme);
    connect(this, &MainWindow::themeChanged, ui->rotatorWidget, &RotatorWidget::redrawMap);
    connect(this, &MainWindow::themeChanged, stats, &StatisticsWidget::changeTheme);

    connect(darkLightModeSwith, &SwitchButton::stateChanged, this, &MainWindow::darkModeToggle);
    darkLightModeSwith->setChecked(settings.value("darkmode", false).toBool());

    connect(Rig::instance(), &Rig::rigErrorPresent, this, &MainWindow::rigErrorHandler);
    connect(Rig::instance(), &Rig::rigCWKeyOpenRequest, this, &MainWindow::cwKeyerConnectProfile);
    connect(Rig::instance(), &Rig::rigCWKeyCloseRequest, this, &MainWindow::cwKeyerDisconnectProfile);
    connect(Rig::instance(), &Rig::frequencyChanged, ui->onlineMapWidget, &OnlineMapWidget::setIBPBand);
    connect(Rig::instance(), &Rig::frequencyChanged, ui->bandmapWidget , &BandmapWidget::updateTunedFrequency);
    connect(Rig::instance(), &Rig::frequencyChanged, ui->newContactWidget, &NewContactWidget::changeFrequency);
    connect(Rig::instance(), &Rig::frequencyChanged, ui->rigWidget, &RigWidget::updateFrequency);
    connect(Rig::instance(), &Rig::modeChanged, ui->bandmapWidget, &BandmapWidget::updateMode);
    connect(Rig::instance(), &Rig::modeChanged, ui->newContactWidget, &NewContactWidget::changeModefromRig);
    connect(Rig::instance(), &Rig::modeChanged, ui->rigWidget, &RigWidget::updateMode);
    connect(Rig::instance(), &Rig::powerChanged, ui->newContactWidget, &NewContactWidget::changePower);
    connect(Rig::instance(), &Rig::powerChanged, ui->rigWidget, &RigWidget::updatePWR);
    connect(Rig::instance(), &Rig::rigConnected, ui->newContactWidget, &NewContactWidget::rigConnected);
    connect(Rig::instance(), &Rig::rigConnected, ui->rigWidget, &RigWidget::rigConnected);
    connect(Rig::instance(), &Rig::rigConnected, ui->cwconsoleWidget, &CWConsoleWidget::rigConnectHandler);
    connect(Rig::instance(), &Rig::rigDisconnected, ui->cwconsoleWidget, &CWConsoleWidget::rigDisconnectHandler);
    connect(Rig::instance(), &Rig::rigDisconnected, ui->newContactWidget, &NewContactWidget::rigDisconnected);
    connect(Rig::instance(), &Rig::rigDisconnected, ui->rigWidget, &RigWidget::rigDisconnected);
    connect(Rig::instance(), &Rig::vfoChanged, ui->rigWidget, &RigWidget::updateVFO);
    connect(Rig::instance(), &Rig::xitChanged, ui->rigWidget, &RigWidget::updateXIT);
    connect(Rig::instance(), &Rig::ritChanged, ui->rigWidget, &RigWidget::updateRIT);
    connect(Rig::instance(), &Rig::pttChanged, ui->rigWidget, &RigWidget::updatePTT);

    connect(Rotator::instance(), &Rotator::rotErrorPresent, this, &MainWindow::rotErrorHandler);
    connect(Rotator::instance(), &Rotator::positionChanged, ui->onlineMapWidget, &OnlineMapWidget::antPositionChanged);
    connect(Rotator::instance(), &Rotator::rotConnected, ui->onlineMapWidget, &OnlineMapWidget::rotConnected);
    connect(Rotator::instance(), &Rotator::rotDisconnected, ui->onlineMapWidget, &OnlineMapWidget::rotDisconnected);
    connect(Rotator::instance(), &Rotator::positionChanged, ui->rotatorWidget, &RotatorWidget::positionChanged);
    connect(Rotator::instance(), &Rotator::rotConnected, ui->rotatorWidget, &RotatorWidget::rotConnected);
    connect(Rotator::instance(), &Rotator::rotDisconnected, ui->rotatorWidget, &RotatorWidget::rotDisconnected);

    connect(CWKeyer::instance(), &CWKeyer::cwKeyerError, this, &MainWindow::cwKeyerErrorHandler);
    connect(CWKeyer::instance(), &CWKeyer::cwKeyWPMChanged, ui->cwconsoleWidget, &CWConsoleWidget::setWPM);
    connect(CWKeyer::instance(), &CWKeyer::cwKeyEchoText, ui->cwconsoleWidget, &CWConsoleWidget::appendCWEchoText);
    connect(CWKeyer::instance(), &CWKeyer::cwKeyConnected, ui->cwconsoleWidget, &CWConsoleWidget::cwKeyConnected);
    connect(CWKeyer::instance(), &CWKeyer::cwKeyDisconnected, ui->cwconsoleWidget, &CWConsoleWidget::cwKeyDisconnected);

    Fldigi* fldigi = new Fldigi(this);
    connect(fldigi, &Fldigi::addContact, ui->newContactWidget, &NewContactWidget::saveExternalContact);

    wsjtx = new Wsjtx(this);
    connect(wsjtx, &Wsjtx::statusReceived, ui->wsjtxWidget, &WsjtxWidget::statusReceived);
    connect(wsjtx, &Wsjtx::decodeReceived, ui->wsjtxWidget, &WsjtxWidget::decodeReceived);
    connect(wsjtx, &Wsjtx::addContact, ui->newContactWidget, &NewContactWidget::saveExternalContact);
    connect(ui->wsjtxWidget, &WsjtxWidget::CQSpot, &networknotification, &NetworkNotification::WSJTXCQSpot);
    connect(ui->wsjtxWidget, &WsjtxWidget::CQSpot, &alertEvaluator, &AlertEvaluator::WSJTXCQSpot);
    connect(ui->wsjtxWidget, &WsjtxWidget::filteredCQSpot, ui->onlineMapWidget, &OnlineMapWidget::drawWSJTXSpot);
    connect(ui->wsjtxWidget, &WsjtxWidget::spotsCleared, ui->onlineMapWidget, &OnlineMapWidget::clearWSJTXSpots);
    connect(ui->wsjtxWidget, &WsjtxWidget::reply, wsjtx, &Wsjtx::startReply);
    connect(ui->wsjtxWidget, &WsjtxWidget::frequencyChanged, ui->newContactWidget, &NewContactWidget::changeFrequency);
    connect(ui->wsjtxWidget, &WsjtxWidget::frequencyChanged, ui->onlineMapWidget, &OnlineMapWidget::setIBPBand);
    connect(ui->wsjtxWidget, &WsjtxWidget::frequencyChanged, ui->bandmapWidget , &BandmapWidget::updateTunedFrequency);
    connect(ui->wsjtxWidget, &WsjtxWidget::modeChanged, ui->newContactWidget, &NewContactWidget::changeModefromRig);

    connect(this, &MainWindow::settingsChanged, wsjtx, &Wsjtx::reloadSetting);
    connect(this, &MainWindow::settingsChanged, ui->rotatorWidget, &RotatorWidget::reloadSettings);
    connect(this, &MainWindow::settingsChanged, ui->rigWidget, &RigWidget::reloadSettings);
    connect(this, &MainWindow::settingsChanged, ui->cwconsoleWidget, &CWConsoleWidget::reloadSettings);
    connect(this, &MainWindow::settingsChanged, ui->rotatorWidget, &RotatorWidget::redrawMap);
    connect(this, &MainWindow::settingsChanged, ui->onlineMapWidget, &OnlineMapWidget::flyToMyQTH);
    connect(this, &MainWindow::settingsChanged, ui->logbookWidget, &LogbookWidget::reloadSetting);
    connect(this, &MainWindow::settingsChanged, ui->dxWidget, &DxWidget::reloadSetting);
    connect(this, &MainWindow::altBackslash, Rig::instance(), &Rig::setPTT);
    connect(this, &MainWindow::manualMode, ui->newContactWidget, &NewContactWidget::setManualMode);
    connect(this, &MainWindow::contestStopped, ui->newContactWidget, &NewContactWidget::stopContest);
    connect(this, &MainWindow::contestStopped, ui->bandmapWidget, &BandmapWidget::resetDupe);
    connect(this, &MainWindow::contestStopped, ui->alertsWidget, &AlertWidget::resetDupe);
    connect(this, &MainWindow::contestStopped, ui->chatWidget, &ChatWidget::resetDupe);

    connect(this, &MainWindow::dupeTypeChanged, ui->bandmapWidget, &BandmapWidget::recalculateDupe);
    connect(this, &MainWindow::dupeTypeChanged, ui->alertsWidget, &AlertWidget::recalculateDupe);
    connect(this, &MainWindow::dupeTypeChanged, ui->chatWidget, &ChatWidget::recalculateDupe);
    connect(this, &MainWindow::dupeTypeChanged, ui->newContactWidget, &NewContactWidget::refreshCallsignsColors);

    connect(ui->rigWidget, &RigWidget::rigProfileChanged, this, &MainWindow::rigConnect);

    connect(ui->rotatorWidget, &RotatorWidget::rotProfileChanged, this, &MainWindow::rotConnect);

    connect(ui->logbookWidget, &LogbookWidget::deletedEntities, Data::instance(), &Data::invalidateSetOfDXCCStatusCache); // must be the first delete signal
    connect(ui->logbookWidget, &LogbookWidget::logbookUpdated, stats, &StatisticsWidget::refreshWidget);
    connect(ui->logbookWidget, &LogbookWidget::contactUpdated, &networknotification, &NetworkNotification::QSOUpdated);
    connect(ui->logbookWidget, &LogbookWidget::clublogContactUpdated, clublogRT, &ClubLog::updateQSOImmediately);
    connect(ui->logbookWidget, &LogbookWidget::contactDeleted, &networknotification, &NetworkNotification::QSODeleted);
    connect(ui->logbookWidget, &LogbookWidget::contactDeleted, ui->bandmapWidget, &BandmapWidget::updateSpotsDupeWhenQSODeleted);
    connect(ui->logbookWidget, &LogbookWidget::deletedEntities, ui->bandmapWidget, &BandmapWidget::updateSpotsDxccStatusWhenQSODeleted);
    connect(ui->logbookWidget, &LogbookWidget::contactDeleted, ui->alertsWidget, &AlertWidget::updateSpotsDupeWhenQSODeleted);
    connect(ui->logbookWidget, &LogbookWidget::deletedEntities, ui->alertsWidget, &AlertWidget::updateSpotsDxccStatusWhenQSODeleted);
    connect(ui->logbookWidget, &LogbookWidget::contactDeleted, ui->chatWidget, &ChatWidget::updateSpotsDupeWhenQSODeleted);
    connect(ui->logbookWidget, &LogbookWidget::deletedEntities, ui->chatWidget, &ChatWidget::updateSpotsDxccStatusWhenQSODeleted);
    connect(ui->logbookWidget, &LogbookWidget::deletedEntities, ui->newContactWidget, &NewContactWidget::refreshCallsignsColors);
    connect(ui->logbookWidget, &LogbookWidget::clublogContactDeleted, clublogRT, &ClubLog::deleteQSOImmediately);
    connect(ui->logbookWidget, &LogbookWidget::sendDXSpotContactReq, ui->dxWidget, &DxWidget::prepareQSOSpot);

    connect(ui->newContactWidget, &NewContactWidget::contactAdded, Data::instance(), &Data::invalidateDXCCStatusCache); // must be the first delete signal
    connect(ui->newContactWidget, &NewContactWidget::contactAdded, ui->logbookWidget, &LogbookWidget::updateTable);
    connect(ui->newContactWidget, &NewContactWidget::contactAdded, &networknotification, &NetworkNotification::QSOInserted);
    connect(ui->newContactWidget, &NewContactWidget::contactAdded, ui->bandmapWidget, &BandmapWidget::updateSpotsStatusWhenQSOAdded);
    connect(ui->newContactWidget, &NewContactWidget::contactAdded, ui->alertsWidget, &AlertWidget::updateSpotsStatusWhenQSOAdded);
    connect(ui->newContactWidget, &NewContactWidget::contactAdded, ui->chatWidget, &ChatWidget::updateSpotsStatusWhenQSOAdded);
    connect(ui->newContactWidget, &NewContactWidget::contactAdded, ui->wsjtxWidget, &WsjtxWidget::updateSpotsStatusWhenQSOAdded);
    connect(ui->newContactWidget, &NewContactWidget::contactAdded, ui->dxWidget, &DxWidget::setLastQSO);
    connect(ui->newContactWidget, &NewContactWidget::contactAdded, clublogRT, &ClubLog::insertQSOImmediately);
    connect(ui->newContactWidget, &NewContactWidget::contestStarted, this, &MainWindow::startContest);
    connect(ui->newContactWidget, &NewContactWidget::newTarget, ui->mapWidget, &MapWidget::setTarget);
    connect(ui->newContactWidget, &NewContactWidget::newTarget, ui->onlineMapWidget, &OnlineMapWidget::setTarget);
    connect(ui->newContactWidget, &NewContactWidget::filterCallsign, ui->logbookWidget, &LogbookWidget::filterCallsign);
    connect(ui->newContactWidget, &NewContactWidget::userFrequencyChanged, ui->bandmapWidget, &BandmapWidget::updateTunedFrequency);
    connect(ui->newContactWidget, &NewContactWidget::userFrequencyChanged, ui->onlineMapWidget, &OnlineMapWidget::setIBPBand);
    connect(ui->newContactWidget, &NewContactWidget::userModeChanged, ui->bandmapWidget, &BandmapWidget::updateMode);
    connect(ui->newContactWidget, &NewContactWidget::markQSO, ui->bandmapWidget, &BandmapWidget::addSpot);
    connect(ui->newContactWidget, &NewContactWidget::callboolImageUrl, ui->profileImageWidget, &ProfileImageWidget::loadImageFromUrl);
    connect(ui->newContactWidget, &NewContactWidget::rigProfileChanged, this, &MainWindow::rigConnect);

    connect(ui->dxWidget, &DxWidget::newFilteredSpot, ui->bandmapWidget, &BandmapWidget::addSpot);
    connect(ui->dxWidget, &DxWidget::newFilteredSpot, Rig::instance(), &Rig::sendDXSpot);
    connect(ui->dxWidget, &DxWidget::newSpot, &networknotification, &NetworkNotification::dxSpot);
    connect(ui->dxWidget, &DxWidget::newSpot, &alertEvaluator, &AlertEvaluator::dxSpot);
    connect(ui->dxWidget, &DxWidget::newWCYSpot, &networknotification, &NetworkNotification::wcySpot);
    connect(ui->dxWidget, &DxWidget::newWWVSpot, &networknotification, &NetworkNotification::wwvSpot);
    connect(ui->dxWidget, &DxWidget::newToAllSpot, &networknotification, &NetworkNotification::toAllSpot);
    connect(ui->dxWidget, &DxWidget::tuneDx, ui->newContactWidget, &NewContactWidget::tuneDx);

    connect(&alertEvaluator, &AlertEvaluator::spotAlert, this, &MainWindow::processSpotAlert);
    connect(&alertEvaluator, &AlertEvaluator::spotAlert, &networknotification, &NetworkNotification::spotAlert);

    connect(ui->bandmapWidget, &BandmapWidget::tuneDx, ui->newContactWidget, &NewContactWidget::tuneDx);
    connect(ui->bandmapWidget, &BandmapWidget::nearestSpotFound, ui->newContactWidget, &NewContactWidget::setNearestSpot);

    connect(ui->wsjtxWidget, &WsjtxWidget::callsignSelected, ui->newContactWidget, &NewContactWidget::prepareWSJTXQSO);

    connect(ui->chatWidget, &ChatWidget::prepareQSOInfo, ui->newContactWidget, &NewContactWidget::fillCallsignGrid);
    connect(ui->chatWidget, &ChatWidget::userListUpdated, ui->onlineMapWidget, &OnlineMapWidget::drawChatUsers);
    connect(ui->chatWidget, &ChatWidget::beamingRequested, ui->rotatorWidget, &RotatorWidget::setBearing);

    connect(ui->onlineMapWidget, &OnlineMapWidget::chatCallsignPressed, ui->chatWidget, &ChatWidget::setChatCallsign);
    connect(ui->onlineMapWidget, &OnlineMapWidget::wsjtxCallsignPressed, ui->wsjtxWidget, &WsjtxWidget::callsignClicked);

    connect(ui->alertsWidget, &AlertWidget::rulesChanged, &alertEvaluator, &AlertEvaluator::loadRules);
    connect(ui->alertsWidget, &AlertWidget::alertsCleared, this, &MainWindow::clearAlertEvent);
    connect(ui->alertsWidget, &AlertWidget::tuneDx, ui->newContactWidget, &NewContactWidget::tuneDx);
    connect(ui->alertsWidget, &AlertWidget::tuneWsjtx, wsjtx, &Wsjtx::startReply);

    conditions = new PropConditions();

    connect(conditions, &PropConditions::conditionsUpdated, this, &MainWindow::conditionsUpdated);
    connect(conditions, &PropConditions::auroraMapUpdated, ui->onlineMapWidget, &OnlineMapWidget::auroraDataUpdate);
    connect(conditions, &PropConditions::mufMapUpdated, ui->onlineMapWidget, &OnlineMapWidget::mufDataUpdate);

    ui->onlineMapWidget->assignPropConditions(conditions);
    ui->newContactWidget->assignPropConditions(conditions);

    connect(clublogRT, &ClubLog::uploadError, this, [this](const QString &msg)
    {
        qCInfo(runtime) << "Clublog RT Upload Error: " << msg;
        QMessageBox::warning(this, tr("Clublog Immediately Upload Error"), msg);
    });

    connect(clublogRT, &ClubLog::QSOUploaded, ui->logbookWidget, &LogbookWidget::updateTable);

    if ( StationProfilesManager::instance()->profileNameList().isEmpty() )
        showSettings();
    else
        MembershipQE::instance()->updateLists();

    /********************/
    /* GLOBAL SHORTCUTs */
    /********************/
    // Menu actions are defined in the MainWindow.ui file
    // The rest of global shortcuts are defined below
    connect(ui->actionSearchCallsign, &QAction::triggered, ui->logbookWidget, &LogbookWidget::focusSearchCallsign);
    connect(ui->actionAddBandmapMark, &QAction::triggered, ui->newContactWidget, &NewContactWidget::markContact);
    connect(ui->actionUseNearestCallsign, &QAction::triggered, ui->newContactWidget, &NewContactWidget::useNearestCallsign);
    connect(ui->actionBandSwitchUp, &QAction::triggered, ui->rigWidget, &RigWidget::bandUp);
    connect(ui->actionBandSwitchDown, &QAction::triggered, ui->rigWidget, &RigWidget::bandDown);
    connect(ui->actionCWSpeedUp, &QAction::triggered, ui->cwconsoleWidget, &CWConsoleWidget::cwKeySpeedIncrease);
    connect(ui->actionCWSpeedDown, &QAction::triggered, ui->cwconsoleWidget, &CWConsoleWidget::cwKeySpeedDecrease);
    connect(ui->actionCWProfileUp, &QAction::triggered, ui->cwconsoleWidget, &CWConsoleWidget::cwShortcutProfileIncrease);
    connect(ui->actionCWProfileDown, &QAction::triggered, ui->cwconsoleWidget, &CWConsoleWidget::cwShortcutProfileDecrease);

    // PTT Off is solved in the Event Handler because it is not possible to handle the Push/Release event for the shortcut.
    connect(ui->actionPTTOn, &QAction::triggered, this, &MainWindow::shortcutALTBackslash);

    // Register aditional action (global shortcuts)
    addAction(ui->actionSearchCallsign);
    addAction(ui->actionAddBandmapMark);
    addAction(ui->actionBandSwitchUp);
    addAction(ui->actionBandSwitchDown);
    addAction(ui->actionUseNearestCallsign);
    addAction(ui->actionCWSpeedUp);
    addAction(ui->actionCWSpeedDown);
    addAction(ui->actionCWProfileUp);
    addAction(ui->actionCWProfileDown);
    addAction(ui->actionPTTOn);
    addAction(ui->actionNewContact);
    addAction(ui->actionSaveContact);

    restoreUserDefinedShortcuts();

    //restoreEquipmentConnOptions();
    //restoreConnectionStates();

    setupActivitiesMenu();
}

void MainWindow::closeEvent(QCloseEvent* event)
{
    FCT_IDENTIFICATION;

    // save the window geometry
    settings.setValue("geometry", saveGeometry());
    settings.setValue("windowState", saveState());

    if ( stats )
    {
        stats->close();
        stats->deleteLater();
        stats = nullptr;
    }

    QMainWindow::closeEvent(event);
}

/* It has to be controlled via global scope because keyReleaseEvent handles
 * only events from focused widget */
void MainWindow::keyReleaseEvent(QKeyEvent *event)
{
    FCT_IDENTIFICATION;

    // shortcut has no Release signal therefore, it is necessary to handle it here.
    const QKeySequence &pttShortcut = ui->actionPTTOn->shortcut();

    // QKeySequence can contain many combinations. check them
    for ( int pttShortcutIndex = 0; pttShortcutIndex < pttShortcut.count(); pttShortcutIndex++ )
    {
        int key = pttShortcut[pttShortcutIndex] & ~Qt::KeyboardModifierMask;
        Qt::KeyboardModifiers modifiers = Qt::KeyboardModifiers(pttShortcut[pttShortcutIndex] & Qt::KeyboardModifierMask);

        if ( event->key() == key
             && event->modifiers() == modifiers
             && !event->isAutoRepeat() )
        {
            emit altBackslash(false);
        }
    }

    QMainWindow::keyReleaseEvent(event);
}

QList<QAction *> MainWindow::getUserDefinedShortcutActionList()
{
    QList<QAction *> ret;
    const QList<QAction*> &actions = findChildren<QAction*>();

    for ( QAction* action : actions)
    {
        if ( !action->shortcut().toString(QKeySequence::NativeText).isEmpty() )
            qCDebug(runtime) << action->text() << action->shortcut().toString(QKeySequence::NativeText);

        if ( action->property("changeableshortcut").toBool() )
        {
            qCDebug(runtime) << "User-Defined shortcut" << action->shortcut().toString(QKeySequence::NativeText);
            ret << action;
        }
    }
    return ret;
}

QStringList MainWindow::getBuiltInStaticShortcutList() const
{
    FCT_IDENTIFICATION;

    QStringList ret;

    const QList<QShortcut*> allShortcuts = findChildren<QShortcut*>();
    for ( QShortcut* shortcut : allShortcuts)
    {
        if ( !shortcut->key().toString().isEmpty() )
        {
            qCDebug(runtime) << "Built-In nonchangeble shortcut"
                             << shortcut->key().toString(QKeySequence::NativeText);
            ret << shortcut->key().toString(QKeySequence::NativeText);
        }
    }

    const QList<QPushButton*> allButtons = findChildren<QPushButton*>();
    for ( QPushButton* button : allButtons)
    {
        if ( !button->shortcut().toString().isEmpty())
        {
            qCDebug(runtime) << "Built-In nonchangable shortcut - buttons"
                    << button->shortcut().toString(QKeySequence::NativeText);
            ret << button->shortcut().toString(QKeySequence::NativeText);
        }
    }

    return ret;
}

void MainWindow::rigConnect()
{
    FCT_IDENTIFICATION;

    //saveEquipmentConnOptions();

    if ( ui->actionConnectRig->isChecked() )
        Rig::instance()->open();
    else
        Rig::instance()->close();
}

void MainWindow::rigErrorHandler(const QString &error, const QString &errorDetail)
{
    FCT_IDENTIFICATION;

    QMessageBox::warning(nullptr, QMessageBox::tr("QLog Warning"),
                         QMessageBox::tr("<b>Rig Error:</b> ") + error
                                         + "<p>" + tr("<b>Error Detail:</b> ") + errorDetail + "</p>");
    ui->actionConnectRig->setChecked(false);
}

void MainWindow::rotErrorHandler(const QString &error, const QString &errorDetail)
{
    FCT_IDENTIFICATION;

    QMessageBox::warning(nullptr, QMessageBox::tr("QLog Warning"),
                         QMessageBox::tr("<b>Rotator Error:</b> ") + error
                                         + "<p>" + tr("<b>Error Detail:</b> ") + errorDetail + "</p>");
    ui->actionConnectRotator->setChecked(false);
}

void MainWindow::cwKeyerErrorHandler(const QString &error, const QString &errorDetail)
{
    FCT_IDENTIFICATION;

    QMessageBox::warning(nullptr, QMessageBox::tr("QLog Warning"),
                         QMessageBox::tr("<b>CW Keyer Error:</b> ") + error
                                         + "<p>" + tr("<b>Error Detail:</b> ") + errorDetail + "</p>");
    ui->actionConnectCWKeyer->setChecked(false);
}

void MainWindow::stationProfileChanged()
{
    FCT_IDENTIFICATION;

    const StationProfile &profile = StationProfilesManager::instance()->getCurProfile1();

    qCDebug(runtime) << profile.callsign << " " << profile.locator << " " << profile.operatorName;

    profileLabel->setText("<b>" + profile.profileName + ":</b>");
    callsignLabel->setText(stationCallsignStatus(profile));
    locatorLabel->setText(profile.locator.toLower());
}

QString MainWindow::stationCallsignStatus(const StationProfile &profile) const
{
    FCT_IDENTIFICATION;

    if ( profile.operatorCallsign.isEmpty() || profile.callsign == profile.operatorCallsign )
        return profile.callsign.toLower();

    return profile.callsign.toLower() + " [" + tr("op: ") + profile.operatorCallsign.toLower() + "]";
}

void MainWindow::darkModeToggle(int mode)
{
    FCT_IDENTIFICATION;

    qCDebug(function_parameters) << mode;

    bool darkMode = (mode == Qt::Checked) ? true: false;
    settings.setValue("darkmode", darkMode);

    if ( mode == Qt::Checked)
    {
        setDarkMode();
    }
    else
    {
        setLightMode();
    }

    QFile style(":/res/stylesheet.css");
    style.open(QFile::ReadOnly | QIODevice::Text);
    qApp->setStyleSheet(style.readAll());
    style.close();

    emit themeChanged(darkMode);

}

void MainWindow::processSpotAlert(SpotAlert alert)
{
    FCT_IDENTIFICATION;

    ui->alertsWidget->addAlert(alert);
    alertButton->setText(QString::number(ui->alertsWidget->alertCount()));
    alertTextButton->setText(alert.ruleName.join(", ") + ": " + alert.callsign + ", " + alert.band + ", " + alert.modeGroupString);
    alertTextButton->disconnect();

    connect(alertTextButton, &QPushButton::clicked, this, [this, alert]()
    {
        if ( alert.source == SpotAlert::WSJTXCQSPOT )
            this->wsjtx->startReply(alert.wsjtxDecode);
        else
            ui->newContactWidget->tuneDx(alert.callsign,
                                         alert.freq,
                                         alert.bandPlanMode);
    });

    if ( ui->actionBeepSettingAlert->isChecked() )
    {
        QApplication::beep();
    }
}

void MainWindow::clearAlertEvent()
{
    FCT_IDENTIFICATION;

    int newCount = ui->alertsWidget->alertCount();

    alertButton->setText(QString::number(newCount));

    if ( newCount == 0 )
    {
        alertTextButton->setText(" ");
        alertTextButton->disconnect();
    }
}

void MainWindow::beepSettingAlerts()
{
    FCT_IDENTIFICATION;

    settings.setValue("alertbeep", ui->actionBeepSettingAlert->isChecked());

    if ( ui->actionBeepSettingAlert->isChecked() )
    {
        QApplication::beep();
    }
}

void MainWindow::shortcutALTBackslash()
{
    FCT_IDENTIFICATION;

    emit altBackslash(true);
}

void MainWindow::setManualContact(bool isChecked)
{
    FCT_IDENTIFICATION;

    emit manualMode(isChecked);
}

void MainWindow::showEditLayout()
{
    FCT_IDENTIFICATION;

    EditActivitiesDialog dialog(this);
    dialog.exec();
    setupActivitiesMenu();
}

void MainWindow::setLayoutGeometry()
{
    FCT_IDENTIFICATION;

    // restore the window geometry and state
    const MainLayoutProfile &layoutProfile = MainLayoutProfilesManager::instance()->getCurProfile1();

    if ( layoutProfile.mainGeometry != QByteArray()
         || layoutProfile.mainState != QByteArray() )
    {
        restoreGeometry(layoutProfile.mainGeometry);
#if (QT_VERSION < QT_VERSION_CHECK(6, 3, 0))
        // workaround for QTBUG-46620
        if ( isMaximized() )
        {
            const QList<QScreen *> &screens = QGuiApplication::screens();
            setGeometry( screens[0]->availableGeometry() );
        }
#endif
        restoreState(layoutProfile.mainState);
        darkLightModeSwith->setChecked(layoutProfile.darkMode);
    }
    else
    {
        restoreGeometry(settings.value("geometry").toByteArray());
#if (QT_VERSION < QT_VERSION_CHECK(6, 3, 0))
        // workaround for QTBUG-46620
        if ( isMaximized() )
        {
            const QList<QScreen *> &screens = QGuiApplication::screens();
            setGeometry( screens[0]->availableGeometry() );
        }
#endif
        restoreState(settings.value("windowState").toByteArray());
        // leave dark mode as is
    }
}

void MainWindow::setSimplyLayoutGeometry()
{
    //this method is a nextstep of the workaround for QTBUG-46620.
    // In the repeated setLayout, it is necessary to call only this.

    FCT_IDENTIFICATION;

    const MainLayoutProfile &layoutProfile = MainLayoutProfilesManager::instance()->getCurProfile1();
    if ( layoutProfile.mainGeometry != QByteArray()
        || layoutProfile.mainState != QByteArray() )
    {
        restoreGeometry(layoutProfile.mainGeometry);
        restoreState(layoutProfile.mainState);
        darkLightModeSwith->setChecked(isFusionStyle && layoutProfile.darkMode);
    }
}

void MainWindow::saveProfileLayoutGeometry()
{
    FCT_IDENTIFICATION;

    MainLayoutProfile layoutProfile = MainLayoutProfilesManager::instance()->getCurProfile1();

    if ( layoutProfile != MainLayoutProfile() )
    {
        layoutProfile.mainGeometry = saveGeometry();
        layoutProfile.mainState = saveState();
        layoutProfile.darkMode = darkLightModeSwith->isChecked();
        layoutProfile.tabsexpanded =  ui->newContactWidget->getTabCollapseState();
        MainLayoutProfilesManager::instance()->addProfile(layoutProfile.profileName, layoutProfile);
        MainLayoutProfilesManager::instance()->save();
    }
}

void MainWindow::setEquipmentKeepOptions(bool)
{
    FCT_IDENTIFICATION;

    // this is obsolete, use activities instead.
    // Left only because of possible problems and for quick activation of the function.
    //saveEquipmentConnOptions();
}



void MainWindow::setDarkMode()
{
    FCT_IDENTIFICATION;

    QPalette darkPalette;
    QColor darkColor = QColor(45,45,45);
    QColor disabledColor = QColor(127,127,127);
    darkPalette.setColor(QPalette::Window, darkColor);
    darkPalette.setColor(QPalette::WindowText, Qt::white);
    darkPalette.setColor(QPalette::Base, QColor(18,18,18));
    darkPalette.setColor(QPalette::AlternateBase, darkColor);
    darkPalette.setColor(QPalette::Text, Qt::white);
    darkPalette.setColor(QPalette::Disabled, QPalette::Text, disabledColor);
    darkPalette.setColor(QPalette::Button, darkColor);
    darkPalette.setColor(QPalette::ButtonText, Qt::white);
    darkPalette.setColor(QPalette::Disabled, QPalette::ButtonText, disabledColor);
    darkPalette.setColor(QPalette::BrightText, Qt::red);
    darkPalette.setColor(QPalette::Link, QColor(42, 130, 218));
    darkPalette.setColor(QPalette::Highlight, QColor(42, 130, 218));
    darkPalette.setColor(QPalette::HighlightedText, Qt::black);
    darkPalette.setColor(QPalette::Disabled, QPalette::HighlightedText, disabledColor);

    qApp->setPalette(darkPalette);
}

void MainWindow::setLightMode()
{
    FCT_IDENTIFICATION;

    qApp->setPalette(this->style()->standardPalette());
}

void MainWindow::setupActivitiesMenu()
{
    FCT_IDENTIFICATION;

    QMenu *actionMenu = activityButton->menu();

    actionMenu->clear();

    const QString &currActivityProfile = ActivityProfilesManager::instance()->getCurProfile1().profileName;

    // The first position will be always the Classic Profile
    QAction *classicLayoutAction = new QAction(tr("Classic"), actionMenu);
    classicLayoutAction->setCheckable(true);
    if ( currActivityProfile == QString() )
    {
        classicLayoutAction->setChecked(true);
        ui->actionSaveGeometry->setEnabled(false);
        setSimplyLayoutGeometry();
        activityButton->setText(classicLayoutAction->text());
    }
    connect(classicLayoutAction, &QAction::triggered, this, [this, classicLayoutAction]()
    {
        //save empty profile
        // Classic Action is only about Layout
        MainLayoutProfilesManager::instance()->setCurProfile1("");
        ActivityProfilesManager::instance()->setCurProfile1("");
        ui->actionSaveGeometry->setEnabled(false);
        activityButton->setText(classicLayoutAction->text());
    } );

    actionMenu->addAction(classicLayoutAction);

    QActionGroup *activitiMenuGroup = new QActionGroup(classicLayoutAction);
    activitiMenuGroup->addAction(classicLayoutAction);

    actionMenu->addSeparator();

    // The rest of positions will be the Custom Activity Profiles
    const QStringList &activityProfileNames = ActivityProfilesManager::instance()->profileNameList();

    for ( const QString &profileName : activityProfileNames )
    {
        QAction *activityAction = new QAction(profileName, actionMenu);
        activityAction->setCheckable(true);

        if ( currActivityProfile == profileName )
        {
            activityAction->setChecked(true);
            ui->actionSaveGeometry->setEnabled(true);
            ActivityProfilesManager::instance()->setAllProfiles();
            activityButton->setText(activityAction->text());
        }

        connect(activityAction, &QAction::triggered, this, [this, profileName, activityAction]()
        {
            ActivityProfilesManager::instance()->setCurProfile1(profileName);
            ui->actionSaveGeometry->setEnabled(true);
            activityButton->setText(activityAction->text());
        } );
        actionMenu->addAction(activityAction);
        activitiMenuGroup->addAction(activityAction);
    }

    actionMenu->addSeparator();
    actionMenu->addAction(ui->actionSaveGeometry);
    actionMenu->addSeparator();
    actionMenu->addAction(ui->actionActivitiesEdit);
}

void MainWindow::saveEquipmentConnOptions()
{
    FCT_IDENTIFICATION;

    // this is obsolete, use activities instead.
    // Left only because of possible problems and for quick activation of the function.
#if 0
    settings.setValue("equipment/keepoptions", ui->actionEquipmentKeepOptions->isChecked());

    if ( ui->actionEquipmentKeepOptions->isChecked() )
    {
        settings.setValue("equipment/rigconnected", ui->actionConnectRig->isChecked());
        settings.setValue("equipment/rotconnected", ui->actionConnectRotator->isChecked());
        settings.setValue("equipment/cwkeyconnected", ui->actionConnectCWKeyer->isChecked());
    }
#endif

}

void MainWindow::restoreConnectionStates()
{
    FCT_IDENTIFICATION;

    // this is obsolete, use activities instead.
    // Left only because of possible problems and for quick activation of the function.
#if 0
    if ( ui->actionEquipmentKeepOptions->isChecked() )
    {
        if ( settings.value("equipment/rigconnected", false).toBool() )
        {
            QTimer::singleShot(2000, this, [this]()
            {
                if ( !ui->actionConnectRig->isChecked() )
                    ui->actionConnectRig->setChecked(true);
            });
        }

        if ( settings.value("equipment/rotconnected", false).toBool() )
        {
            QTimer::singleShot(2500, this, [this]()
            {
                if ( !ui->actionConnectRotator->isChecked() )
                    ui->actionConnectRotator->setChecked(true);
            });
        }

        if ( settings.value("equipment/cwkeyconnected", false).toBool() )
        {
            QTimer::singleShot(3000, this, [this]()
            {
                if ( !ui->actionConnectCWKeyer->isChecked() )
                    ui->actionConnectCWKeyer->setChecked(true);
            });
        }
    }
#endif

}

void MainWindow::restoreEquipmentConnOptions()
{
    FCT_IDENTIFICATION;

    // this is obsolete, use activities instead.
    // Left only because of possible problems and for quick activation of the function.
#if 0
    ui->actionEquipmentKeepOptions->blockSignals(true);
    ui->actionEquipmentKeepOptions->setChecked(settings.value("equipment/keepoptions", false).toBool());
    ui->actionEquipmentKeepOptions->blockSignals(false);
#endif

}

void MainWindow::restoreUserDefinedShortcuts()
{
    FCT_IDENTIFICATION;

    const QHash<QString, QVariant> &state = settings.value("shortcuts").toHash();

    if ( state.count() > 0)
    {
        const QList<QAction*> actions = getUserDefinedShortcutActionList();
        for ( QAction *action : actions )
        {
            const QVariant &value = state.value(action->objectName());
            const QString &current = action->shortcut().toString(QKeySequence::NativeText);
            qCDebug(runtime) << "Object "<< action->objectName()
                             << "current shortcut" << current
                             << "requested" << value.toString();

            if ( value != QVariant() &&  current != value.toString())
            {
                action->setShortcut(value.toString());
                qCDebug(runtime) << "Changed";
            }
        }
    }
}

void MainWindow::saveUserDefinedShortcuts()
{
    FCT_IDENTIFICATION;

    QHash<QString, QVariant> state;
    const QList<QAction*> actions = getUserDefinedShortcutActionList();
    for ( const QAction *action : actions )
    {
        const QString & newShortcut = action->shortcut().toString(QKeySequence::NativeText);

        qCDebug(runtime) << "Object" << action->objectName()
                         << "has a shortcut" << newShortcut;
        state[action->objectName()] = newShortcut;
    }
    settings.setValue("shortcuts", state);
}

void MainWindow::saveContestMenuSeqnoType(QAction *action)
{
    FCT_IDENTIFICATION;

    LogParam::setParam("contest/seqnotype", action->data());
    // this function is called only if contest is not active
    // therefore it is not needed to somehow recalculate seq
}

void MainWindow::restoreContestMenuSeqnoType()
{
    FCT_IDENTIFICATION;

    ui->actionSeqSingle->setData(Data::SeqType::SINGLE);
    ui->actionSeqPerBand->setData(Data::SeqType::PER_BAND);
    seqGroup = new QActionGroup(ui->menuSequence);
    seqGroup->addAction(ui->actionSeqSingle);
    seqGroup->addAction(ui->actionSeqPerBand);

    int seqnoType = LogParam::getParam("contest/seqnotype", Data::SeqType::SINGLE).toInt();

    const QList<QAction *> seqActions = seqGroup->actions();
    for ( QAction *action : seqActions)
    {
        if ( action->data().toInt() == seqnoType )
        {
            action->setChecked(true);
            break;
        }
    }
}

void MainWindow::saveContestMenuDupeType(QAction *action)
{
    FCT_IDENTIFICATION;

    LogParam::setParam("contest/dupetype", action->data());
    emit dupeTypeChanged();
}

void MainWindow::saveContestMenuLinkExchangeType(QAction *action)
{
    FCT_IDENTIFICATION;

    LogParam::setParam("contest/linkexchangetype", action->data());
    ui->newContactWidget->changeSRXStringLink(action->data().toInt());
}

void MainWindow::restoreContestMenuDupeType()
{
    FCT_IDENTIFICATION;

    dupeGroup = new QActionGroup(ui->menuDupeCheck);
    ui->actionDupeAllBands->setData(Data::DupeType::ALL_BANDS);
    ui->actionDupeEachBand->setData(Data::DupeType::EACH_BAND);
    ui->actionDupeEachBandMode->setData(Data::DupeType::EACH_BAND_MODE);
    ui->actionDupeNoCheck->setData(Data::DupeType::NO_CHECK);
    dupeGroup->addAction(ui->actionDupeAllBands);
    dupeGroup->addAction(ui->actionDupeEachBand);
    dupeGroup->addAction(ui->actionDupeEachBandMode);
    dupeGroup->addAction(ui->actionDupeNoCheck);

    int dupeType = LogParam::getParam("contest/dupetype", Data::DupeType::ALL_BANDS).toInt();

    const QList<QAction *> seqActions = dupeGroup->actions();
    for ( QAction *action : seqActions)
    {
        if ( action->data().toInt() == dupeType )
        {
            action->setChecked(true);
            break;
        }
    }
}

void MainWindow::restoreContestMenuLinkExchange()
{
    FCT_IDENTIFICATION;

    linkExchangeGroup = new QActionGroup(ui->menuLinkExchange);

    int linkExchangeType = LogParam::getParam("contest/linkexchangetype", LogbookModel::COLUMN_INVALID).toInt();

    ui->actionLinkExchangeNone->setData(LogbookModel::COLUMN_INVALID);
    linkExchangeGroup->addAction(ui->actionLinkExchangeNone);
    ui->actionLinkExchangeNone->setChecked(linkExchangeType == LogbookModel::COLUMN_INVALID);

    QList<QAction*> actions;

    auto addActionToMenu = [&] (const LogbookModel::ColumnID columnID)
    {
        QAction *newAction = new QAction(ui->menuLinkExchange);
        newAction->setCheckable(true);
        newAction->setText(LogbookModel::getFieldNameTranslation(columnID));
        newAction->setData(columnID);
        actions.append(newAction);
    };

    addActionToMenu(LogbookModel::COLUMN_AGE);
    addActionToMenu(LogbookModel::COLUMN_CQZ);
    addActionToMenu(LogbookModel::COLUMN_ITUZ);
    addActionToMenu(LogbookModel::COLUMN_GRID);
    addActionToMenu(LogbookModel::COLUMN_NAME_INTL);
    addActionToMenu(LogbookModel::COLUMN_QTH_INTL);
    addActionToMenu(LogbookModel::COLUMN_RX_PWR);
    addActionToMenu(LogbookModel::COLUMN_STATE);

    std::sort(actions.begin(), actions.end(), [](QAction *a, QAction *b)
    {
        return a->text().localeAwareCompare(b->text()) < 0;
    });

    for (QAction *action : actions)
    {
        ui->menuLinkExchange->addAction(action);
        linkExchangeGroup->addAction(action);

        if ( action->data().toInt() == linkExchangeType )
            action->setChecked(true);
    }

    ui->newContactWidget->changeSRXStringLink(linkExchangeType);
}

void MainWindow::startContest(const QString contestID, const QDateTime)
{
    FCT_IDENTIFICATION;

    // Contest's start signal is sent from NewContact
    const QSOFilter &contestFilter = QSOFilter::createFromNowContestFilter(contestID);
    QSOFilterManager::instance()->save(contestFilter);
    ui->logbookWidget->refreshUserFilter();
    ui->logbookWidget->setUserFilter(contestFilter.filterName);
    LogParam::setParam("contest/filter", contestFilter.filterName);
    setContestMode(contestID);
}

void MainWindow::stopContest()
{
    FCT_IDENTIFICATION;

    const QString &contestFilterName = LogParam::getParam("contest/filter").toString();

    if ( !contestFilterName.isEmpty() )
    {
        QMessageBox::StandardButton reply;

        reply = QMessageBox::question(this, tr("Contest"),
                                      tr("Do you want to remove the Contest filter %1?").arg(contestFilterName),
                                      QMessageBox::Yes|QMessageBox::No);

        if ( reply == QMessageBox::Yes )
        {
            QSOFilterManager::instance()->remove(contestFilterName);
            ui->logbookWidget->refreshUserFilter();
        }
        else
        {
            QSOFilter contestFilter = QSOFilterManager::instance()->getFilter(contestFilterName);
            contestFilter.addRule(QSOFilter::createToDateRule(QDateTime::currentDateTimeUtc()));
            QSOFilterManager::instance()->save(contestFilter);
        }
    }
    LogParam::setParam("contest/filter", QString());
    setContestMode(QString());

    emit contestStopped();
}

void MainWindow::setContestMode(const QString &contestID)
{
    FCT_IDENTIFICATION;

    bool isActive = !contestID.isEmpty();
    ui->actionContestStop->setEnabled(isActive);
    if ( seqGroup )
        seqGroup->setEnabled(!isActive);

    contestLabel->setVisible(isActive);
    contestLabel->setText((isActive) ? "<b>" + tr("Contest: ") + "</b>" + contestID : QString());
}

void MainWindow::handleActivityChange(const QString name)
{
    FCT_IDENTIFICATION;

    qCDebug(function_parameters) << name;

    const ActivityProfile &profile = ActivityProfilesManager::instance()->getProfile(name);

    const QVariant &valueRig = profile.getProfileParam(ActivityProfile::ProfileType::RIG_PROFILE,
                                                       ActivityProfile::ProfileParamType::CONNECT);

    if ( !valueRig.isNull()
        && RigProfilesManager::instance()->getCurProfile1().profileName == profile.profiles[ActivityProfile::ProfileType::RIG_PROFILE].name )
    {
        if ( ui->actionConnectRig->isChecked() && valueRig.toBool() )
            rigConnect();
        else
            ui->actionConnectRig->setChecked(valueRig.toBool()); // rigConnect is called when the signal is processed
    }

    const QVariant &valueRot = profile.getProfileParam(ActivityProfile::ProfileType::ROT_PROFILE,
                                                       ActivityProfile::ProfileParamType::CONNECT);

    if ( !valueRig.isNull()
          && RotProfilesManager::instance()->getCurProfile1().profileName == profile.profiles[ActivityProfile::ProfileType::ROT_PROFILE].name )
    {
        if ( ui->actionConnectRotator->isChecked() && valueRig.toBool() )
            rotConnect();
        else
            ui->actionConnectRotator->setChecked(valueRot.toBool()); // rotConnect is called when the signal is processed
    }
}

void MainWindow::rotConnect()
{
    FCT_IDENTIFICATION;

    //saveEquipmentConnOptions();

    if ( ui->actionConnectRotator->isChecked() )
        Rotator::instance()->open();
    else
        Rotator::instance()->close();
}

void MainWindow::cwKeyerConnect()
{
    FCT_IDENTIFICATION;

    //saveEquipmentConnOptions();;

    if ( ui->actionConnectCWKeyer->isChecked() )
    {
        CWKeyer::instance()->open();
    }
    else
    {
        CWKeyer::instance()->close();
    }
}

void MainWindow::cwKeyerConnectProfile(QString requestedProfile)
{
    FCT_IDENTIFICATION;

    // it is a hack, maybe it will be improved in the future
    if ( requestedProfile == EMPTY_PROFILE_NAME || requestedProfile.isEmpty() )
    {
        return;
    }

    CWKeyProfile testingProfile = CWKeyProfilesManager::instance()->getProfile(requestedProfile);

    if ( testingProfile == CWKeyProfile() ) //if requested profile does not exists then no change
    {
        qWarning() << "Rig request unknown CW Key Profile";
        return;
    }
    ui->actionConnectCWKeyer->setChecked(true);
    CWKeyProfilesManager::instance()->setCurProfile1(requestedProfile);

    cwKeyerConnect();
}

void MainWindow::cwKeyerDisconnectProfile(QString requestedProfile)
{
    FCT_IDENTIFICATION;

    // it is a hack, maybe it will be improved in the future
    if ( requestedProfile == EMPTY_PROFILE_NAME || requestedProfile.isEmpty() )
    {
        return;
    }

    CWKeyProfile testingProfile = CWKeyProfilesManager::instance()->getProfile(requestedProfile);

    if ( testingProfile == CWKeyProfile() ) //if requested profile does not exists then no change
    {
        qWarning() << "Rig requests an unknown CW Key Profile";
        return;
    }

    /* checking whether the user has changed the assigned key during the work. If so, leave it connected. */
    if ( testingProfile !=  CWKeyProfilesManager::instance()->getCurProfile1() )
    {
        return;
    }
    ui->actionConnectCWKeyer->setChecked(false);

    cwKeyerConnect();
}

void MainWindow::showSettings()
{
    FCT_IDENTIFICATION;

    SettingsDialog sw(this);

    if ( sw.exec() == QDialog::Accepted )
    {
        rigConnect();
        rotConnect();
        stationProfileChanged();

        MembershipQE::instance()->updateLists();
        saveUserDefinedShortcuts();
        emit settingsChanged();
    }
    else
        restoreUserDefinedShortcuts();
}

void MainWindow::showStatistics()
{
    FCT_IDENTIFICATION;

    if ( stats )
    {
       stats->show();
    }
}

void MainWindow::importLog() {
    FCT_IDENTIFICATION;

    ImportDialog dialog;
    dialog.exec();
    ui->logbookWidget->updateTable();
}

void MainWindow::exportLog() {
    FCT_IDENTIFICATION;

    ExportDialog dialog;
    dialog.exec();
    ui->logbookWidget->updateTable();
}

void MainWindow::showLotw()
{
    FCT_IDENTIFICATION;

    if ( ! Lotw::getUsername().isEmpty() )
    {
        LotwDialog dialog;
        dialog.exec();
        ui->logbookWidget->updateTable();
    }
    else
    {
        QMessageBox::warning(this, tr("QLog Warning"), tr("LoTW is not configured properly.<p> Please, use <b>Settings</b> dialog to configure it.</p>"));
    }
}

void MainWindow::showeQSL()
{
    FCT_IDENTIFICATION;

    if ( ! EQSL::getUsername().isEmpty() )
    {
        EqslDialog dialog;
        dialog.exec();
        ui->logbookWidget->updateTable();
    }
    else
    {
        QMessageBox::warning(this, tr("QLog Warning"), tr("eQSL is not configured properly.<p> Please, use <b>Settings</b> dialog to configure it.</p>"));
    }
}

void MainWindow::showClublog()
{
    FCT_IDENTIFICATION;

    if ( ! ClubLog::getEmail().isEmpty() )
    {
        ClublogDialog dialog;
        dialog.exec();
        ui->logbookWidget->updateTable();
    }
    else
    {
        QMessageBox::warning(this, tr("QLog Warning"), tr("Clublog is not configured properly.<p> Please, use <b>Settings</b> dialog to configure it.</p>"));
    }
}

void MainWindow::showHRDLog()
{
    FCT_IDENTIFICATION;

    if ( ! HRDLog::getRegisteredCallsign().isEmpty() )
    {
        HRDLogDialog dialog;
        dialog.exec();
        ui->logbookWidget->updateTable();
    }
    else
    {
        QMessageBox::warning(this, tr("QLog Warning"), tr("HRDLog is not configured properly.<p> Please, use <b>Settings</b> dialog to configure it.</p>"));
    }
}

void MainWindow::showQRZ()
{
    FCT_IDENTIFICATION;

    QString logbookAPIKey = QRZ::getLogbookAPIKey();

    if ( !logbookAPIKey.isEmpty() )
    {
        QRZDialog dialog;
        dialog.exec();
        ui->logbookWidget->updateTable();
    }
    else
    {
        QMessageBox::warning(this, tr("QLog Warning"), tr("QRZ.com is not configured properly.<p> Please, use <b>Settings</b> dialog to configure it.</p>"));
    }

}

void MainWindow::showAwards()
{
    FCT_IDENTIFICATION;

    AwardsDialog dialog;
    connect(&dialog, &AwardsDialog::AwardConditionSelected,
            ui->logbookWidget, &LogbookWidget::filterCountryBand);
    connect(&dialog, &AwardsDialog::finished,
            ui->logbookWidget, &LogbookWidget::restoreFilters);
    dialog.exec();
}

void MainWindow::showAbout() {
    FCT_IDENTIFICATION;

    QString aboutText = tr("<h1>QLog %1</h1>"
                           "<p>&copy; 2019 Thomas Gatzweiler DL2IC<br/>"
                           "&copy; 2021-2024 Ladislav Foldyna OK1MLG</p>"
                           "<p>Based on Qt %2<br/>"
                           "%3<br/>"
                           "%4<br/>"
                           "%5</p>"
                           "<p>Icon by <a href='http://www.iconshock.com'>Icon Shock</a><br />"
                           "Satellite images by <a href='http://www.nasa.gov'>NASA</a><br />"
                           "ZoneDetect by <a href='https://github.com/BertoldVdb/ZoneDetect'>Bertold Van den Bergh</a><br />"
                           "TimeZone Database by <a href='https://github.com/evansiroky/timezone-boundary-builder'>Evan Siroky</a>");


    QString version = QCoreApplication::applicationVersion();
    QString hamlibVersion =
#if defined(Q_OS_WIN)
            QString(rig_version());
#else
            QString(hamlib_version);
#endif

    QString OSName = QString("%1 %2 (%3)").arg(QSysInfo::prettyProductName(), QSysInfo::currentCpuArchitecture(), QGuiApplication::platformName() );
    aboutText = aboutText.arg(version)
                         .arg(qVersion())
                         .arg(hamlibVersion)
                         .arg(QSslSocket::sslLibraryVersionString())
                         .arg(OSName);


    QMessageBox::about(this, tr("About"), aboutText);
}

void MainWindow::showWikiHelp()
{
    FCT_IDENTIFICATION;

    QDesktopServices::openUrl(QString("https://github.com/foldynl/QLog/wiki"));
}

void MainWindow::showMailingList()
{
    FCT_IDENTIFICATION;

    QDesktopServices::openUrl(QString("https://groups.io/g/qlog"));
}

void MainWindow::showReportBug()
{
    FCT_IDENTIFICATION;
    QDesktopServices::openUrl(QString("https://github.com/foldynl/QLog/blob/master/CONTRIBUTING.md#reporting-bugs"));
}

void MainWindow::showAlerts()
{
    FCT_IDENTIFICATION;

    ui->alertDockWidget->show();
}

void MainWindow::clearAlerts()
{
    FCT_IDENTIFICATION;

    ui->alertsWidget->clearAllAlerts();
}

void MainWindow::conditionsUpdated() {
    FCT_IDENTIFICATION;

    QString kcolor, fluxcolor, acolor;

    QString k_index_string, flux_string, a_index_string;

    k_index_string = flux_string = a_index_string = tr("N/A");

    /* https://3fs.net.au/making-sense-of-solar-indices/ */
    if ( conditions->isKIndexValid() )
    {
        double k_index = conditions->getKIndex();

        if (k_index < 3.5) {
            kcolor = "green";
        }
        else if (k_index < 4.5) {
            kcolor = "orange";
        }
        else {
            kcolor = "red";
        }

        k_index_string = QString::number(k_index, 'g', 2);
    }

    if ( conditions->isFluxValid() )
    {
        if ( conditions->getFlux() < 100 )
        {
            fluxcolor = "red";
        }
        else if ( conditions->getFlux() < 200 )
        {
            fluxcolor = "orange";
        }
        else
        {
            fluxcolor = "green";
        }

        flux_string = QString::number(conditions->getFlux());

    }

    if ( conditions->isAIndexValid() )
    {
        if ( conditions->getAIndex() < 27 )
        {
            acolor = "green";
        }
        else if ( conditions->getAIndex() < 48 )
        {
            acolor = "orange";
        }
        else
        {
            acolor = "red";
        }

        a_index_string = QString::number(conditions->getAIndex());
    }

    conditionsLabel->setTextFormat(Qt::RichText);
    conditionsLabel->setText(QString("SFI <b style='color: %1'>%2</b> A <b style='color: %3'>%4</b> K <b style='color: %5'>%6</b>").arg(
                                 fluxcolor, flux_string, acolor, a_index_string, kcolor, k_index_string ));
}

void MainWindow::QSOFilterSetting()
{
    FCT_IDENTIFICATION;

    QSOFilterDialog dialog(this);
    dialog.exec();
    ui->logbookWidget->refreshUserFilter();
}

void MainWindow::alertRuleSetting()
{
    FCT_IDENTIFICATION;
    ui->alertsWidget->showEditRules();
}

MainWindow::~MainWindow()
{
    FCT_IDENTIFICATION;

    //saveEquipmentConnOptions();

    CWKeyer::instance()->close();
    QThread::msleep(500);

    Rig::instance()->stopTimer();
    Rotator::instance()->stopTimer();
    CWKeyer::instance()->stopTimer();

    conditions->deleteLater();
    conditionsLabel->deleteLater();
    profileLabel->deleteLater();
    callsignLabel->deleteLater();
    locatorLabel->deleteLater();
    QSqlDatabase::database().close();
    clublogRT->deleteLater();
    if ( wsjtx )
        wsjtx->deleteLater();

    seqGroup->deleteLater();
    dupeGroup->deleteLater();
    delete ui;
}
