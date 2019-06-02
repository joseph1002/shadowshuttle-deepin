//
// Created by wallestar on 5/29/19.
//

#include "GfwListUpdater.h"
#include "model/Configuration.h"

const QString GfwListUpdater::GFWLIST_URL = "https://raw.githubusercontent.com/gfwlist/gfwlist/master/gfwlist.txt";

bool GfwListUpdater::mergeAndWritePACFile(QString gfwListResult) {
    return true;
}

QString GfwListUpdater::mergePACFile(QString gfwListResult) {
    return "";
}

void GfwListUpdater::updatePACFromGFWList(const Configuration& configuration) {

}

QList<QString> GfwListUpdater::parseBase64ToValidList(QString response) {
    return QList<QString>();
}

QList<QString> GfwListUpdater::parseToValidList(QString content) {
    return QList<QString>();
}
