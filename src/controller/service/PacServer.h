//
// Created by wallestar on 5/29/19.
//

#ifndef PACSERVER_H
#define PACSERVER_H

#include <QtCore>

class Configuration;

class PacServer {
public:
    PacServer() {};
    QString touchPacFile();
    QString getPacUrl(const Configuration& configuration);
    QString getPacAddress(QString host, QString port);

private:
    const static QString PAC_FILE;
    const static QString PAC_FILE_ZIP;
    const static QString USER_RULE_FILE;
    const static QString USER_ABP_FILE;
    const static QString USER_ABP_FILE_ZIP;
};


#endif //PACSERVER_H
