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
    static bool MergeAndWritePACFile(QString gfwListResult);
    static QString MergePACFile(QString gfwListResult);
    void UpdatePACFromGFWList(const Configuration& configuration);
    static QList<QString> ParseBase64ToValidList(QString response);
    static QList<QString> ParseToValidList(QString content);
private:
    const QString GFWLIST_URL = "https://raw.githubusercontent.com/gfwlist/gfwlist/master/gfwlist.txt";
};


#endif //GFWLISTUPDATER_H
