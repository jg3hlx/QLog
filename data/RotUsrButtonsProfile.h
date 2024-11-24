#ifndef QLOG_DATA_ROTUSRBUTTONSPROFILE_H
#define QLOG_DATA_ROTUSRBUTTONSPROFILE_H

#include <QString>
#include <QObject>
#include <QDataStream>

#include "data/ProfileManager.h"

#define MAX_ROT_USER_BUTTONS 4

class RotUsrButtonsProfile
{
public:
    RotUsrButtonsProfile()
    {
        shortDescs.resize(MAX_ROT_USER_BUTTONS);
        bearings.resize(MAX_ROT_USER_BUTTONS);
    }

    QString profileName;
    QVector<QString> shortDescs;
    QVector<double> bearings;

    bool operator== (const RotUsrButtonsProfile &profile);
    bool operator!= (const RotUsrButtonsProfile &profile);

private:
    friend QDataStream& operator<<(QDataStream& out, const RotUsrButtonsProfile& v);
    friend QDataStream& operator>>(QDataStream& in, RotUsrButtonsProfile& v);

};

Q_DECLARE_METATYPE(RotUsrButtonsProfile);

class RotUsrButtonsProfilesManager : public ProfileManagerSQL<RotUsrButtonsProfile>
{
    Q_OBJECT

public:

    explicit RotUsrButtonsProfilesManager();
    ~RotUsrButtonsProfilesManager() { };

    static RotUsrButtonsProfilesManager* instance()
    {
        static RotUsrButtonsProfilesManager instance;
        return &instance;
    };
    void save();

};

#endif // QLOG_DATA_ROTUSRBUTTONSPROFILE_H
