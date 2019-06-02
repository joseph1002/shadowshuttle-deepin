//
// Created by wallestar on 5/29/19.
//

#ifndef GFWLISTUPDATER_H
#define GFWLISTUPDATER_H
#include <QString>
#include <QList>

class Configuration;
class GfwListUpdater {
public:
    GfwListUpdater() {}
    static bool mergeAndWritePACFile(QString gfwListResult);
    static QString mergePACFile(QString gfwListResult);
    void updatePACFromGFWList(const Configuration& configuration);
    static QList<QString> parseBase64ToValidList(QString response);
    static QList<QString> parseToValidList(QString content);
private:
    const static QString GFWLIST_URL;
};


#endif //GFWLISTUPDATER_H
