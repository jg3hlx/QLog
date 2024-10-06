#include <QStringListModel>
#include <QShortcut>

#include "CWConsoleWidget.h"
#include "ui_CWConsoleWidget.h"
#include "core/debug.h"
#include "data/CWKeyProfile.h"
#include "cwkey/CWKeyer.h"
#include "data/CWShortcutProfile.h"

MODULE_IDENTIFICATION("qlog.ui.cwconsolewidget");

CWConsoleWidget::CWConsoleWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CWConsoleWidget),
    cwKeyOnline(false),
    contact(nullptr),
    sendWord(false)
{
    FCT_IDENTIFICATION;

    ui->setupUi(this);

    QStringListModel* keyModel = new QStringListModel(this);
    ui->cwKeyProfileCombo->setModel(keyModel);


    QStringListModel* shortcutModel = new QStringListModel(this);
    ui->cwShortcutProfileCombo->setModel(shortcutModel);

    refreshKeyProfileCombo();
    refreshShortcutProfileCombo();

    connect(ui->modeSwitch, &SwitchButton::stateChanged, this, &CWConsoleWidget::sendWordSwitched);

    sendWord = getSendWordConfig();
    ui->modeSwitch->setChecked(sendWord);

    cwKeyDisconnected();
}

CWConsoleWidget::~CWConsoleWidget()
{
    FCT_IDENTIFICATION;

    delete ui;
}

void CWConsoleWidget::registerContactWidget(const NewContactWidget * contactWidget)
{
    FCT_IDENTIFICATION;

    contact = contactWidget;
}

void CWConsoleWidget::appendCWEchoText(QString text)
{
    FCT_IDENTIFICATION;

    qCDebug(function_parameters) << text;

    if ( ui->cwEchoConsoleText->isEnabled() )
    {
        ui->cwEchoConsoleText->moveCursor(QTextCursor::End);
        ui->cwEchoConsoleText->insertPlainText(text);
    }
}

void CWConsoleWidget::cwKeyProfileComboChanged(QString profileName)
{
    FCT_IDENTIFICATION;

    qCDebug(function_parameters) << profileName;

    CWKeyProfilesManager::instance()->setCurProfile1(profileName);

    emit cwKeyProfileChanged();
}

void CWConsoleWidget::cwShortcutProfileComboChanged(QString profileName)
{
    FCT_IDENTIFICATION;

    qCDebug(function_parameters) << profileName;

    CWShortcutProfilesManager *shortcutManager =  CWShortcutProfilesManager::instance();
    shortcutManager->setCurProfile1(profileName);

    const CWShortcutProfile &profile = shortcutManager->getCurProfile1();

    ui->macroButton1->setText("F1\n" + profile.shortDescs[0]);
    ui->macroButton1->setShortcut(Qt::Key_F1);
    ui->macroButton2->setText("F2\n" + profile.shortDescs[1]);
    ui->macroButton2->setShortcut(Qt::Key_F2);
    ui->macroButton3->setText("F3\n" + profile.shortDescs[2]);
    ui->macroButton3->setShortcut(Qt::Key_F3);
    ui->macroButton4->setText("F4\n" + profile.shortDescs[3]);
    ui->macroButton4->setShortcut(Qt::Key_F4);
    ui->macroButton5->setText("F5\n" + profile.shortDescs[4]);
    ui->macroButton5->setShortcut(Qt::Key_F5);
    ui->macroButton6->setText("F6\n" + profile.shortDescs[5]);
    ui->macroButton6->setShortcut(Qt::Key_F6);
    ui->macroButton7->setText("F7\n" + profile.shortDescs[6]);
    ui->macroButton7->setShortcut(Qt::Key_F7);

    emit cwShortcutProfileChanged();
}

void CWConsoleWidget::cwShortcutProfileIncrease()
{
    FCT_IDENTIFICATION;

    shortcutComboMove(1);
}

void CWConsoleWidget::cwShortcutProfileDecrease()
{
    FCT_IDENTIFICATION;

    shortcutComboMove(-1);
}

void CWConsoleWidget::shortcutComboMove(int step)
{
    FCT_IDENTIFICATION;

    qCDebug(function_parameters) << step;

    int count = ui->cwShortcutProfileCombo->count();
    int currIndex = ui->cwShortcutProfileCombo->currentIndex();

    if ( count > 0
         && currIndex != -1
         && step < count )
    {
        int nextIndex = currIndex + step;
        nextIndex += (1 - nextIndex / count) * count;
        ui->cwShortcutProfileCombo->setCurrentIndex(nextIndex % count);
    }
}

void CWConsoleWidget::allowMorseSending(bool allow)
{
    FCT_IDENTIFICATION;

    if ( allow )
    {
        ui->cwEchoConsoleText->setEnabled(CWKeyer::instance()->canEchoChar());
        ui->haltButton->setEnabled(CWKeyer::instance()->canStopSending());
        ui->cwKeySpeedSpinBox->setEnabled(CWKeyer::instance()->canSetSpeed());
    }
    else
    {
        ui->cwEchoConsoleText->setEnabled(allow);
        ui->haltButton->setEnabled(allow);
        ui->cwKeySpeedSpinBox->setEnabled(allow);
    }
    ui->clearButton->setEnabled(allow);
    ui->cwConsoleText->setEnabled(allow);
    ui->cwSendEdit->setEnabled(allow);
    ui->cwShortcutProfileCombo->setEnabled(allow);
    ui->macroButton1->setEnabled(allow);
    ui->macroButton2->setEnabled(allow);
    ui->macroButton3->setEnabled(allow);
    ui->macroButton4->setEnabled(allow);
    ui->macroButton5->setEnabled(allow);
    ui->macroButton6->setEnabled(allow);
    ui->macroButton7->setEnabled(allow);
    ui->modeSwitch->setEnabled(allow);
}

void CWConsoleWidget::saveSendWordConfig(bool state)
{
    FCT_IDENTIFICATION;

    QSettings settings;

    settings.setValue("cwconsole/sendWord", state);
}

bool CWConsoleWidget::getSendWordConfig()
{
    FCT_IDENTIFICATION;

    QSettings settings;

    return settings.value("cwconsole/sendWord", false).toBool();
}

void CWConsoleWidget::refreshKeyProfileCombo()
{
    FCT_IDENTIFICATION;

    ui->cwKeyProfileCombo->blockSignals(true);

    CWKeyProfilesManager *cwKeyManager =  CWKeyProfilesManager::instance();

    QStringList currProfiles = cwKeyManager->profileNameList();
    QStringListModel* model = dynamic_cast<QStringListModel*>(ui->cwKeyProfileCombo->model());

    model->setStringList(currProfiles);

    if ( cwKeyManager->getCurProfile1().profileName.isEmpty()
         && currProfiles.count() > 0 )
    {
        /* changing profile from empty to something */
        ui->cwKeyProfileCombo->setCurrentText(currProfiles.first());
    }
    else
    {
        /* no profile change, just refresh the combo and preserve current profile */
        ui->cwKeyProfileCombo->setCurrentText(cwKeyManager->getCurProfile1().profileName);
    }

    cwKeyProfileComboChanged(ui->cwKeyProfileCombo->currentText());

    ui->cwKeyProfileCombo->blockSignals(false);
}

void CWConsoleWidget::refreshShortcutProfileCombo()
{
    FCT_IDENTIFICATION;

    ui->cwShortcutProfileCombo->blockSignals(true);

    CWShortcutProfilesManager *shortcutManager =  CWShortcutProfilesManager::instance();

    QStringList currProfiles = shortcutManager->profileNameList();
    QStringListModel* model = dynamic_cast<QStringListModel*>(ui->cwShortcutProfileCombo->model());

    model->setStringList(currProfiles);

    if ( shortcutManager->getCurProfile1().profileName.isEmpty()
         && currProfiles.count() > 0 )
    {
        /* changing profile from empty to something */
        ui->cwShortcutProfileCombo->setCurrentText(currProfiles.first());
    }
    else
    {
        /* no profile change, just refresh the combo and preserve current profile */
        ui->cwShortcutProfileCombo->setCurrentText(shortcutManager->getCurProfile1().profileName);
    }

    cwShortcutProfileComboChanged(ui->cwShortcutProfileCombo->currentText());

    ui->cwShortcutProfileCombo->blockSignals(false);
}

void CWConsoleWidget::reloadSettings()
{
    FCT_IDENTIFICATION;

    refreshKeyProfileCombo();
    refreshShortcutProfileCombo();
}

void CWConsoleWidget::clearConsoles()
{
    FCT_IDENTIFICATION;

    ui->cwConsoleText->clear();
    ui->cwEchoConsoleText->clear();

}

void CWConsoleWidget::cwKeyConnected(QString profile)
{
    FCT_IDENTIFICATION;
    ui->cwKeyProfileCombo->setStyleSheet("QComboBox {color: green}");

    if ( profile != ui->cwKeyProfileCombo->currentText() )
    {
        ui->cwKeyProfileCombo->blockSignals(true);
        ui->cwKeyProfileCombo->setCurrentText(profile);
        ui->cwKeyProfileCombo->blockSignals(false);
    }
    allowMorseSending(true);
    ui->cwKeySpeedSpinBox->setValue(CWKeyProfilesManager::instance()->getCurProfile1().defaultSpeed);
    cwKeyOnline = true;

    ui->cwSendEdit->setPlaceholderText(QString());
}

void CWConsoleWidget::cwKeyDisconnected()
{
    FCT_IDENTIFICATION;
    ui->cwKeyProfileCombo->setStyleSheet("QComboBox {color: red}");

    allowMorseSending(false);

    clearConsoles();

    cwKeyOnline = false;
}

void CWConsoleWidget::cwKeySpeedChanged(int newWPM)
{
    FCT_IDENTIFICATION;

    qCDebug(function_parameters) << newWPM;

    CWKeyer::instance()->setSpeed(newWPM);
    Rig::instance()->syncKeySpeed(newWPM);
}

void CWConsoleWidget::cwKeySpeedIncrease()
{
    FCT_IDENTIFICATION;

    if ( !CWKeyer::instance()->canSetSpeed() ) return;
    ui->cwKeySpeedSpinBox->setValue(ui->cwKeySpeedSpinBox->value() + 2);
}

void CWConsoleWidget::cwKeySpeedDecrease()
{
    FCT_IDENTIFICATION;

    if ( !CWKeyer::instance()->canSetSpeed() ) return;
    ui->cwKeySpeedSpinBox->setValue(ui->cwKeySpeedSpinBox->value() - 2);
}

void CWConsoleWidget::cwSendButtonPressed(bool insertNewLine)
{
    FCT_IDENTIFICATION;

    if ( ui->cwSendEdit->text().isEmpty() )
    {
        return;
    }

    sendCWText(ui->cwSendEdit->text(), insertNewLine);
    ui->cwSendEdit->clear();
}

void CWConsoleWidget::rigDisconnectHandler()
{
    FCT_IDENTIFICATION;

    if ( CWKeyer::instance()->rigMustConnected() )
    {
        allowMorseSending(false);
        if ( cwKeyOnline )
        {
            ui->cwSendEdit->setPlaceholderText(tr("Rig must be connected"));
        }
    }
}

void CWConsoleWidget::rigConnectHandler()
{
    FCT_IDENTIFICATION;

    if ( cwKeyOnline )
    {
        // if MorseOverCat Key
        if (CWKeyer::instance()->rigMustConnected()
            && Rig::instance()->isMorseOverCatSupported() )
        {
            allowMorseSending(true);
            CWKeyer::instance()->setSpeed(ui->cwKeySpeedSpinBox->value());

            ui->cwSendEdit->setPlaceholderText(QString());
        }
        else
        {
            Rig::instance()->syncKeySpeed(ui->cwKeySpeedSpinBox->value());
        }
    }
}

void CWConsoleWidget::cwKeyMacroF1()
{
    FCT_IDENTIFICATION;

    sendCWText(CWShortcutProfilesManager::instance()->getCurProfile1().macros[0]);
}

void CWConsoleWidget::cwKeyMacroF2()
{
    FCT_IDENTIFICATION;

    sendCWText(CWShortcutProfilesManager::instance()->getCurProfile1().macros[1]);
}

void CWConsoleWidget::cwKeyMacroF3()
{
    FCT_IDENTIFICATION;

    sendCWText(CWShortcutProfilesManager::instance()->getCurProfile1().macros[2]);
}

void CWConsoleWidget::cwKeyMacroF4()
{
    FCT_IDENTIFICATION;

    sendCWText(CWShortcutProfilesManager::instance()->getCurProfile1().macros[3]);
}

void CWConsoleWidget::cwKeyMacroF5()
{
    FCT_IDENTIFICATION;

    sendCWText(CWShortcutProfilesManager::instance()->getCurProfile1().macros[4]);
}

void CWConsoleWidget::cwKeyMacroF6()
{
    FCT_IDENTIFICATION;

    sendCWText(CWShortcutProfilesManager::instance()->getCurProfile1().macros[5]);
}

void CWConsoleWidget::cwKeyMacroF7()
{
    FCT_IDENTIFICATION;

    sendCWText(CWShortcutProfilesManager::instance()->getCurProfile1().macros[6]);
}

void CWConsoleWidget::haltButtonPressed()
{
    FCT_IDENTIFICATION;

    CWKeyer::instance()->imediatellyStop();
}

void CWConsoleWidget::sendWordSwitched(int mode)
{
    FCT_IDENTIFICATION;

    sendWord = (mode == Qt::Checked) ? true: false;

    ui->sendModeLabel->setText(sendWord ? tr("Word") : tr("Whole"));
    saveSendWordConfig(sendWord);
}

void CWConsoleWidget::cwTextChanged(QString text)
{
    FCT_IDENTIFICATION;

    qCDebug(function_parameters) << text << text.endsWith(" ") << text.size() << sendWord;

    if ( sendWord && text.size() > 0 && text.endsWith(" ") )
    {
        cwSendButtonPressed(false);
    }
}

void CWConsoleWidget::setWPM(qint32 wpm)
{
    FCT_IDENTIFICATION;

    ui->cwKeySpeedSpinBox->blockSignals(true);
    ui->cwKeySpeedSpinBox->setValue(wpm);
    Rig::instance()->syncKeySpeed(wpm);
    ui->cwKeySpeedSpinBox->blockSignals(false);
}

void CWConsoleWidget::sendCWText(const QString &text, bool insertNewLine)
{
    FCT_IDENTIFICATION;

    QString expandedText(text.toUpper());

    expandMacros(expandedText);

    qCDebug(runtime) << "CW text" << expandedText;
    QString newLine = (insertNewLine ? QString("\n")
                                     : QString());
    CWKeyer::instance()->sendText(expandedText + QString(" ") + newLine); //insert extra space do divide words in echo console

    newLine.replace("\n", QString::fromUtf8("\u23ce\n")); // UTF8 Return char
    ui->cwConsoleText->moveCursor(QTextCursor::End);
    ui->cwConsoleText->insertPlainText(expandedText + newLine);
}

void CWConsoleWidget::expandMacros(QString &text)
{
    FCT_IDENTIFICATION;

    static QRegularExpression callRE("<DXCALL>");
    static QRegularExpression nameRE("<NAME>");
    static QRegularExpression rstRE("<RST>");
    static QRegularExpression rstnRE("<RSTN>");
    static QRegularExpression greetingRE("<GREETING>");

    static QRegularExpression myCallRE("<MYCALL>");
    static QRegularExpression myNameRE("<MYNAME>");
    static QRegularExpression myQTHRE("<MYQTH>");
    static QRegularExpression myLocatorRE("<MYLOCATOR>");
    static QRegularExpression myGridRE("<MYGRID>");
    static QRegularExpression mySIGRE("<MYSIG>");
    static QRegularExpression mySIGInfoRE("<MYSIGINFO>");
    static QRegularExpression myIOTARE("<MYIOTA>");
    static QRegularExpression mySOTARE("<MYSOTA>");
    static QRegularExpression myWWFTRE("<MYWWFT>");
    static QRegularExpression myVUCCRE("<MYVUCC>");
    static QRegularExpression myPWRRE("<MYPWR>");

    if ( contact )
    {
        text.replace(callRE, contact->getCallsign().toUpper());
        text.replace(nameRE, contact->getName().toUpper());
        text.replace(rstRE, contact->getRST().toUpper());
        text.replace(rstnRE, contact->getRST().replace('9', 'N'));
        text.replace(greetingRE, contact->getGreeting().toUpper());

        text.replace(myCallRE, contact->getMyCallsign().toUpper());
        text.replace(myNameRE, contact->getMyName().toUpper());
        text.replace(myQTHRE, contact->getMyQTH().toUpper());
        text.replace(myLocatorRE, contact->getMyLocator().toUpper());
        text.replace(myGridRE, contact->getMyLocator().toUpper());
        text.replace(mySIGRE, contact->getMySIG().toUpper());
        text.replace(mySIGInfoRE, contact->getMySIGInfo().toUpper());
        text.replace(myIOTARE, contact->getMyIOTA().toUpper());
        text.replace(mySOTARE, contact->getMySOTA().toUpper());
        text.replace(myWWFTRE, contact->getMyWWFT().toUpper());
        text.replace(myVUCCRE, contact->getMyVUCC().toUpper());
        text.replace(myPWRRE, contact->getMyPWR().toUpper());
    }
}
