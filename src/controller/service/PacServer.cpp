//
// Created by wallestar on 5/29/19.
//

#include "PacServer.h"
#include "model/Configuration.h"
#include "common/utils.h"
//#include "JlCompress.h"

const QString PacServer::PAC_FILE = "pac.txt";
const QString PacServer::USER_RULE_FILE = "user-rule.txt";
const QString PacServer::USER_ABP_FILE = "abp.txt";

QString PacServer::touchPacFile() {

}

QString PacServer::getPacUrl(const Configuration& configuration) {
    QString pacUrl;
    if (configuration.isUseOnlinePac()) {
        pacUrl = configuration.getPacUrl();
    } else {
        QString pac_file = QDir(Utils::configPath()).filePath("pac.txt");
        QFile file(pac_file);
        if (!file.exists()) {
            // create pac.txt from resources

        }
        pacUrl = "file://" + pac_file;
    }
    qDebug() << "pacUrl:" << pacUrl;
    return pacUrl;
}
