#include <QMessageBox>

#include "ActivityEditor.h"
#include "ui_ActivityEditor.h"
#include "core/debug.h"
#include "ui/NewContactWidget.h"
#include "ui/MainWindow.h"

MODULE_IDENTIFICATION("qlog.ui.mainlayouteditor");

ActivityEditor::ActivityEditor(const QString &activityName,
                                   QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ActivityEditor),
    availableFieldsModel(new StringListModel(this)),
    qsoRowAFieldsModel(new StringListModel(this)),
    qsoRowBFieldsModel(new StringListModel(this)),
    detailColAFieldsModel(new StringListModel(this)),
    detailColBFieldsModel(new StringListModel(this)),
    detailColCFieldsModel(new StringListModel(this)),
    dynamicWidgets(new NewContactDynamicWidgets(false, this))
{
    FCT_IDENTIFICATION;

    ui->setupUi(this);

#if (QT_VERSION < QT_VERSION_CHECK(5, 15, 0))
    // disabled for QT 5.12 (ubuntu 20.04) due to issue in QT
    // moveRow does not work
    ui->qsoRowADownButton->setVisible(false);
    ui->qsoRowAUpButton->setVisible(false);
    ui->qsoRowBDownButton->setVisible(false);
    ui->qsoRowBUpButton->setVisible(false);
    ui->detailColADownButton->setVisible(false);
    ui->detailColAUpButton->setVisible(false);
    ui->detailColBDownButton->setVisible(false);
    ui->detailColBUpButton->setVisible(false);
    ui->detailColCDownButton->setVisible(false);
    ui->detailColCUpButton->setVisible(false);
#endif

    availableFieldsModel->setStringList(dynamicWidgets->getAllFieldLabelNames());
    availableFieldsModel->sort(0);

    ui->availableFieldsListView->setModel(availableFieldsModel);
    ui->qsoRowAFieldsListView->setModel(qsoRowAFieldsModel);
    ui->qsoRowBFieldsListView->setModel(qsoRowBFieldsModel);
    ui->detailColAFieldsListView->setModel(detailColAFieldsModel);
    ui->detailColBFieldsListView->setModel(detailColBFieldsModel);
    ui->detailColCFieldsListView->setModel(detailColCFieldsModel);

    connectQSORowButtons();
    connectDetailColsButtons();

    if ( ! activityName.isEmpty() )
    {
        MainLayoutProfile profile = MainLayoutProfilesManager::instance()->getProfile(activityName);

        ui->activityNameEdit->setEnabled(false);
        ui->activityNameEdit->setText(profile.profileName);

        fillWidgets(profile);
    }
    else
        fillWidgets(MainLayoutProfile::getClassicLayout());
}

ActivityEditor::~ActivityEditor()
{
    delete ui;
    delete dynamicWidgets;
}

void ActivityEditor::save()
{
    FCT_IDENTIFICATION;

    if ( ui->activityNameEdit->text().isEmpty() )
    {
        ui->activityNameEdit->setPlaceholderText(tr("Must not be empty"));
        return;
    }

    if ( activityNameExists(ui->activityNameEdit->text()) )
    {
        QMessageBox::warning(nullptr, QMessageBox::tr("QLog Info"),
                              QMessageBox::tr("Activity name is already exists."));
        return;
    }

    MainLayoutProfile profile;

    profile.profileName = ui->activityNameEdit->text();
    profile.rowA = getFieldIndexes(qsoRowAFieldsModel);
    profile.rowB = getFieldIndexes(qsoRowBFieldsModel);
    profile.detailColA = getFieldIndexes(detailColAFieldsModel);
    profile.detailColB = getFieldIndexes(detailColBFieldsModel);
    profile.detailColC = getFieldIndexes(detailColCFieldsModel);
    profile.mainGeometry = mainGeometry;
    profile.mainState = mainState;
    profile.darkMode = darkMode;
    MainLayoutProfilesManager::instance()->addProfile(profile.profileName, profile);
    MainLayoutProfilesManager::instance()->save();

    accept();
}

void ActivityEditor::profileNameChanged(const QString &profileName)
{
    FCT_IDENTIFICATION;

    QPalette p;
    p.setColor(QPalette::Text, ( activityNameExists(profileName) ) ? Qt::red
                                                                   : qApp->palette().text().color());
    ui->activityNameEdit->setPalette(p);
}

void ActivityEditor::clearMainLayoutClick()
{
    FCT_IDENTIFICATION;

    mainGeometry = QByteArray();
    mainState = QByteArray();
    darkMode = false;
    ui->mainLayoutStateLabel->setText(statusUnSavedText);
    ui->mainLayoutClearButton->setEnabled(false);
}

void ActivityEditor::moveField(StringListModel *source,
                               StringListModel *destination,
                               const QModelIndexList &sourceIndexList)
{
    FCT_IDENTIFICATION;

    QModelIndexList selectedIndexes = sourceIndexList;

    if ( selectedIndexes.isEmpty() )
            return;

    std::sort(selectedIndexes.begin(),
              selectedIndexes.end(),
              [](const QModelIndex &a, const QModelIndex &b)
    {
        return a.row() > b.row();
    });

    for ( const QModelIndex &index : sourceIndexList )
        destination->append(source->data(index).toString());

    /* Delete the sorted index list becuase without sorting
     * it deletes wrong records
     */
    for ( const QModelIndex &index : selectedIndexes )
        source->deleteItem(index);
}

void ActivityEditor::connectQSORowButtons()
{
    FCT_IDENTIFICATION;

    // Connect buttons for QSO Row A
    connectMoveButtons(ui->qsoRowADownButton, ui->qsoRowAUpButton,
                       ui->qsoRowAFieldsListView, qsoRowAFieldsModel);
    connectFieldButtons(ui->moveToQSORowAButton, ui->removeFromQSORowAButton,
                        qsoRowAFieldsModel, ui->qsoRowAFieldsListView);

    // Connect buttons for QSO Row B
    connectMoveButtons(ui->qsoRowBDownButton, ui->qsoRowBUpButton,
                       ui->qsoRowBFieldsListView, qsoRowBFieldsModel);
    connectFieldButtons(ui->moveToQSORowBButton, ui->removeFromQSORowBButton,
                        qsoRowBFieldsModel, ui->qsoRowBFieldsListView);
}

void ActivityEditor::connectDetailColsButtons()
{
    FCT_IDENTIFICATION;

    connectMoveButtons(ui->detailColADownButton, ui->detailColAUpButton,
                       ui->detailColAFieldsListView, detailColAFieldsModel);
    connectFieldButtons(ui->moveToDetailColAButton, ui->removeFromDetailColAButton,
                        detailColAFieldsModel, ui->detailColAFieldsListView);

    connectMoveButtons(ui->detailColBDownButton, ui->detailColBUpButton,
                       ui->detailColBFieldsListView, detailColBFieldsModel);
    connectFieldButtons(ui->moveToDetailColBButton, ui->removeFromDetailColBButton,
                        detailColBFieldsModel, ui->detailColBFieldsListView);

    connectMoveButtons(ui->detailColCDownButton, ui->detailColCUpButton,
                       ui->detailColCFieldsListView, detailColCFieldsModel);
    connectFieldButtons(ui->moveToDetailColCButton, ui->removeFromDetailColCButton,
                        detailColCFieldsModel, ui->detailColCFieldsListView);
}

void ActivityEditor::connectMoveButtons(QPushButton *downButton, QPushButton *upButton,
                                        QListView *listView, StringListModel *model)
{
    FCT_IDENTIFICATION;

    connect(downButton, &QPushButton::clicked, this, [listView, model]()
    {
        const QModelIndexList &modelList = listView->selectionModel()->selectedRows();
        if ( !modelList.isEmpty() )
            model->moveDown(modelList.at(0));
    });

    connect(upButton, &QPushButton::clicked, this, [listView, model]()
    {
        const QModelIndexList &modelList = listView->selectionModel()->selectedRows();
        if ( !modelList.isEmpty() )
            model->moveUp(modelList.at(0));
    });
}

void ActivityEditor::connectFieldButtons(QPushButton *moveToButton, QPushButton *removeButton,
                                         StringListModel *targetModel, QListView *targetListView)
{
    FCT_IDENTIFICATION;

    connect(moveToButton, &QPushButton::clicked, this, [this, targetModel]()
    {
         moveField(availableFieldsModel, targetModel,
         ui->availableFieldsListView->selectionModel()->selectedIndexes());
    });

    connect(removeButton, &QPushButton::clicked, this, [this, targetModel, targetListView]()
    {
         moveField(targetModel, availableFieldsModel,
         targetListView->selectionModel()->selectedRows());
    });
}

QList<int> ActivityEditor::getFieldIndexes(StringListModel *model)
{
    FCT_IDENTIFICATION;

    const QStringList &list = model->stringList();
    QList<int> ret;

    for ( const QString &fieldName : list )
    {
        int index = dynamicWidgets->getIndex4FieldLabelName(fieldName);
        if ( index >= 0 )
            ret << index;
    }

    return ret;
}

void ActivityEditor::fillWidgets(const MainLayoutProfile &profile)
{
    FCT_IDENTIFICATION;

    auto processFields = [this](const QList<int>& fieldIndices, StringListModel* targetModel)
    {
        for (int fieldIndex : fieldIndices)
        {
            const QString &fieldName = dynamicWidgets->getFieldLabelName4Index(fieldIndex);
            targetModel->append(fieldName);
            availableFieldsModel->deleteItem(fieldName);
        }
    };

    // Process each row and detail column using the helper
    processFields(static_cast<const QList<int>&>(profile.rowA), qsoRowAFieldsModel);
    processFields(static_cast<const QList<int>&>(profile.rowB), qsoRowBFieldsModel);
    processFields(static_cast<const QList<int>&>(profile.detailColA), detailColAFieldsModel);
    processFields(static_cast<const QList<int>&>(profile.detailColB), detailColBFieldsModel);
    processFields(static_cast<const QList<int>&>(profile.detailColC), detailColCFieldsModel);

    mainGeometry = profile.mainGeometry;
    mainState = profile.mainState;
    darkMode = profile.darkMode;

    if ( mainGeometry == QByteArray()
         && mainState == QByteArray() )
    {
        ui->mainLayoutStateLabel->setText(statusUnSavedText);
        ui->mainLayoutClearButton->setEnabled(false);
    }
}

bool ActivityEditor::activityNameExists(const QString &activityName)
{
    FCT_IDENTIFICATION;

    qCDebug(function_parameters) << activityName;

    return  ui->activityNameEdit->isEnabled()
            && MainLayoutProfilesManager::instance()->profileNameList().contains(activityName);
}
