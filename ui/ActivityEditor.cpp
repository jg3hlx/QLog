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

    /*********************/
    /* QSO Row A Buttons */
    /*********************/
    connect(ui->qsoRowADownButton, &QPushButton::clicked, this, [this]()
    {
        QModelIndexList modelList = ui->qsoRowAFieldsListView->selectionModel()->selectedRows();

        if ( !modelList.isEmpty() )
            qsoRowAFieldsModel->moveDown(modelList.at(0));
    });

    connect(ui->qsoRowAUpButton, &QPushButton::clicked, this, [this]()
    {
        QModelIndexList modelList = ui->qsoRowAFieldsListView->selectionModel()->selectedRows();

        if ( !modelList.isEmpty() )
            qsoRowAFieldsModel->moveUp(modelList.at(0));
    });

    connect(ui->moveToQSORowAButton, &QPushButton::clicked, this, [this]()
    {
        moveField(availableFieldsModel,
                  qsoRowAFieldsModel,
                  ui->availableFieldsListView->selectionModel()->selectedIndexes());
    });

    connect(ui->removeFromQSORowAButton, &QPushButton::clicked, this, [this]()
    {
        moveField(qsoRowAFieldsModel,
                  availableFieldsModel,
                  ui->qsoRowAFieldsListView->selectionModel()->selectedRows());
    });
    /*********************/
    /* QSO Row B Buttons */
    /*********************/
    connect(ui->qsoRowBDownButton, &QPushButton::clicked, this, [this]()
    {
        QModelIndexList modelList = ui->qsoRowBFieldsListView->selectionModel()->selectedRows();

        if ( !modelList.isEmpty() )
            qsoRowBFieldsModel->moveDown(modelList.at(0));
    });

    connect(ui->qsoRowBUpButton, &QPushButton::clicked, this, [this]()
    {
        QModelIndexList modelList = ui->qsoRowBFieldsListView->selectionModel()->selectedRows();

        if ( !modelList.isEmpty() )
            qsoRowBFieldsModel->moveUp(modelList.at(0));
    });

    connect(ui->moveToQSORowBButton, &QPushButton::clicked, this, [this]()
    {
        moveField(availableFieldsModel,
                  qsoRowBFieldsModel,
                  ui->availableFieldsListView->selectionModel()->selectedIndexes());
    });

    connect(ui->removeFromQSORowBButton, &QPushButton::clicked, this, [this]()
    {
        moveField(qsoRowBFieldsModel,
                  availableFieldsModel,
                  ui->qsoRowBFieldsListView->selectionModel()->selectedRows());
    });
}

void ActivityEditor::connectDetailColsButtons()
{
    FCT_IDENTIFICATION;

    /****************************/
    /* QSO Detail Col A Buttons */
    /****************************/
    connect(ui->detailColADownButton, &QPushButton::clicked, this, [this]()
    {
        QModelIndexList modelList = ui->detailColAFieldsListView->selectionModel()->selectedRows();

        if ( !modelList.isEmpty() )
            detailColAFieldsModel->moveDown(modelList.at(0));
    });

    connect(ui->detailColAUpButton, &QPushButton::clicked, this, [this]()
    {
        QModelIndexList modelList = ui->detailColAFieldsListView->selectionModel()->selectedRows();

        if ( !modelList.isEmpty() )
            detailColAFieldsModel->moveUp(modelList.at(0));
    });

    connect(ui->moveToDetailColAButton, &QPushButton::clicked, this, [this]()
    {
        moveField(availableFieldsModel,
                  detailColAFieldsModel,
                  ui->availableFieldsListView->selectionModel()->selectedIndexes());
    });

    connect(ui->removeFromDetailColAButton, &QPushButton::clicked, this, [this]()
    {
        moveField(detailColAFieldsModel,
                  availableFieldsModel,
                  ui->detailColAFieldsListView->selectionModel()->selectedRows());
    });

    /****************************/
    /* QSO Detail Col B Buttons */
    /****************************/
    connect(ui->detailColBDownButton, &QPushButton::clicked, this, [this]()
    {
        QModelIndexList modelList = ui->detailColBFieldsListView->selectionModel()->selectedRows();

        if ( !modelList.isEmpty() )
            detailColBFieldsModel->moveDown(modelList.at(0));
    });

    connect(ui->detailColBUpButton, &QPushButton::clicked, this, [this]()
    {
        QModelIndexList modelList = ui->detailColBFieldsListView->selectionModel()->selectedRows();

        if ( !modelList.isEmpty() )
            detailColBFieldsModel->moveUp(modelList.at(0));
    });

    connect(ui->moveToDetailColBButton, &QPushButton::clicked, this, [this]()
    {
        moveField(availableFieldsModel,
                  detailColBFieldsModel,
                  ui->availableFieldsListView->selectionModel()->selectedIndexes());
    });

    connect(ui->removeFromDetailColBButton, &QPushButton::clicked, this, [this]()
    {
        moveField(detailColBFieldsModel,
                  availableFieldsModel,
                  ui->detailColBFieldsListView->selectionModel()->selectedRows());
    });

    /****************************/
    /* QSO Detail Col C Buttons */
    /****************************/
    connect(ui->detailColCDownButton, &QPushButton::clicked, this, [this]()
    {
        QModelIndexList modelList = ui->detailColCFieldsListView->selectionModel()->selectedRows();

        if ( !modelList.isEmpty() )
            detailColCFieldsModel->moveDown(modelList.at(0));
    });

    connect(ui->detailColCUpButton, &QPushButton::clicked, this, [this]()
    {
        QModelIndexList modelList = ui->detailColCFieldsListView->selectionModel()->selectedRows();

        if ( !modelList.isEmpty() )
            detailColCFieldsModel->moveUp(modelList.at(0));
    });

    connect(ui->moveToDetailColCButton, &QPushButton::clicked, this, [this]()
    {
        moveField(availableFieldsModel,
                  detailColCFieldsModel,
                  ui->availableFieldsListView->selectionModel()->selectedIndexes());
    });

    connect(ui->removeFromDetailColCButton, &QPushButton::clicked, this, [this]()
    {
        moveField(detailColCFieldsModel,
                  availableFieldsModel,
                  ui->detailColCFieldsListView->selectionModel()->selectedRows());
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

    for ( int fieldIndex : static_cast<const QList<int>&>(profile.rowA) )
    {
        QString fieldName = dynamicWidgets->getFieldLabelName4Index(fieldIndex);
        qsoRowAFieldsModel->append(fieldName);
        availableFieldsModel->deleteItem(fieldName);
    }

    for ( int fieldIndex : static_cast<const QList<int>&>(profile.rowB) )
    {
        QString fieldName = dynamicWidgets->getFieldLabelName4Index(fieldIndex);
        qsoRowBFieldsModel->append(fieldName);
        availableFieldsModel->deleteItem(fieldName);
    }

    for ( int fieldIndex : static_cast<const QList<int>&>(profile.detailColA) )
    {
        QString fieldName = dynamicWidgets->getFieldLabelName4Index(fieldIndex);
        detailColAFieldsModel->append(fieldName);
        availableFieldsModel->deleteItem(fieldName);
    }

    for ( int fieldIndex : static_cast<const QList<int>&>(profile.detailColB) )
    {
        QString fieldName = dynamicWidgets->getFieldLabelName4Index(fieldIndex);
        detailColBFieldsModel->append(fieldName);
        availableFieldsModel->deleteItem(fieldName);
    }

    for ( int fieldIndex : static_cast<const QList<int>&>(profile.detailColC) )
    {
        QString fieldName = dynamicWidgets->getFieldLabelName4Index(fieldIndex);
        detailColCFieldsModel->append(fieldName);
        availableFieldsModel->deleteItem(fieldName);
    }

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
