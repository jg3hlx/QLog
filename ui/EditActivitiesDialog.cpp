#include "EditActivitiesDialog.h"
#include "ui_EditActivitiesDialog.h"
#include "core/debug.h"
#include "ui/EditActivitiesDialog.h"
#include "ui/MainLayoutEditor.h"
#include "data/MainLayoutProfile.h"

MODULE_IDENTIFICATION("qlog.ui.EditLayoutDialog");

EditActivitiesDialog::EditActivitiesDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::EditActivitiesDialog)
{
    FCT_IDENTIFICATION;

    ui->setupUi(this);
    loadProfiles();
}

EditActivitiesDialog::~EditActivitiesDialog()
{
    FCT_IDENTIFICATION;
    delete ui;
}

void EditActivitiesDialog::loadProfiles()
{
    FCT_IDENTIFICATION;

    ui->listView->setModel(new QStringListModel(MainLayoutProfilesManager::instance()->profileNameList(), this));
}

void EditActivitiesDialog::addButton()
{
    FCT_IDENTIFICATION;

    MainLayoutEditor dialog(QString(), this);
    dialog.exec();
    loadProfiles();
}

void EditActivitiesDialog::removeButton()
{
    FCT_IDENTIFICATION;

    MainLayoutProfilesManager::instance()->removeProfile(ui->listView->currentIndex().data().toString());
    MainLayoutProfilesManager::instance()->save();
    loadProfiles();
}

void EditActivitiesDialog::editEvent(const QModelIndex &idx)
{
    FCT_IDENTIFICATION;

    MainLayoutEditor dialog(ui->listView->model()->data(idx).toString(), this);
    dialog.exec();
}

void EditActivitiesDialog::editButton()
{
    FCT_IDENTIFICATION;

    const QModelIndexList &selected = ui->listView->selectionModel()->selectedIndexes();
    if (!selected.isEmpty())
        editEvent(selected.first());
}
