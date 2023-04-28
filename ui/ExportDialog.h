#ifndef EXPORTDIALOG_H
#define EXPORTDIALOG_H

#include <QDialog>
#include <QSqlRecord>
#include <QList>

#include "core/LogLocale.h"

namespace Ui {
class ExportDialog;
}

class ExportDialog : public QDialog {
    Q_OBJECT

public:
    explicit ExportDialog(QWidget *parent = nullptr);
    explicit ExportDialog(const QList<QSqlRecord>&, QWidget *parent = nullptr);
    ~ExportDialog();

public slots:
    void browse();
    void toggleDateRange();
    void toggleMyCallsign();
    void toggleMyGridsquare();
    void runExport();
    void myCallsignChanged(const QString &myCallsign);

private:
    Ui::ExportDialog *ui;
    LogLocale locale;

    const QList<QSqlRecord> qsos4export;
    void setProgress(float);
};

#endif // EXPORTDIALOG_H
