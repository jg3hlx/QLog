#ifndef QSOFILTERMANAGER_H
#define QSOFILTERMANAGER_H

#include <QObject>
#include <QSqlQuery>

struct QSOFilterRule
{
    int tableFieldIndex;
    int operatorID;
    QString value;
};

class QSOFilter
{
public:
    QString filterName;
    int machingType;
    QList<QSOFilterRule> rules;

    void addRule(const QSOFilterRule &rule)
    {
        rules.append(rule);
    }
};

class QSOFilterManager : public QObject
{
    Q_OBJECT
public:

    static QSOFilterManager * instance()
    {
        static QSOFilterManager instance;
        return &instance;
    }

    bool save(const QSOFilter &filter);
    QStringList getFilterList() const;
    QSOFilter getFilter(const QString &filterName) const;
    QString getWhereClause(const QString &filterName) const;

private:
    QSOFilterManager(QObject *parent = nullptr);

    bool replaceFilter(const QString &filterName, const int matchingType);
    bool insertFilterRule(const QString &filterName, const QSOFilterRule &rule);
    bool deleteFilterRules(const QString &filterName);

    bool stmtsReady;
    QSqlQuery insertRuleStmt;
    QSqlQuery insertFilterStmt;
    QSqlQuery deleteFilterStmt;
};

#endif // QSOFILTERMANAGER_H
