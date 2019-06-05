//
// Created by wallestar on 5/29/19.
//

#include "PacServer.h"
#include "model/Configuration.h"
#include "common/utils.h"
#include "common/constant.h"

const QString PacServer::PAC_FILE = "proxy.pac.txt";
const QString PacServer::PAC_FILE_ZIP = "proxy.pac.txt.zip";
const QString PacServer::USER_RULE_FILE = "user-rule.txt";
const QString PacServer::USER_ABP_FILE = "abp.js";
const QString PacServer::USER_ABP_FILE_ZIP = "abp.js.zip";

QString PacServer::touchPacFile(int port) {
    QString configPath = Utils::configPath();
    QString pacFile = QDir(configPath).filePath(PAC_FILE);
    QFile file(pacFile);
    if (!file.exists()) {
        QString zipFilePath = Utils::getQrcDataPath(PAC_FILE_ZIP);
        QString fileContent = Utils::uncompressFile(zipFilePath, PAC_FILE);

        QString proxy = getPacAddress(Constant::LOCALHOST, port);
        fileContent.replace("__PROXY__", proxy);
//        qDebug() << fileContent;

        if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            return "";
        }
        file.write(fileContent.toUtf8());
        file.flush();
        file.close();
    }
    return pacFile;
}

QString PacServer::touchPacFile(int oldPort, int port) {
    QString configPath = Utils::configPath();
    QString pacFile = QDir(configPath).filePath(PAC_FILE);
    QFile file(pacFile);
    if (!file.exists()) {
        return touchPacFile(port);
    }

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return "";
    }

    QString fileContent = file.readAll();
    fileContent.replace(getPacAddress(Constant::LOCALHOST, oldPort), getPacAddress(Constant::LOCALHOST, port));
    file.close();

    if (!file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate)) {
        return "";
    }

    file.write(fileContent.toUtf8());
    file.flush();
    file.close();

    return pacFile;
}

QString PacServer::touchPacFile(int port, QString fileContent) {
    QString proxy = getPacAddress(Constant::LOCALHOST, port);
    fileContent.replace("__PROXY__", proxy);
//    qDebug() << fileContent;

    QString configPath = Utils::configPath();
    QString pacFile = QDir(configPath).filePath(PAC_FILE);
    QFile file(pacFile);

    if (!file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate)) {
        return "";
    }
    file.write(fileContent.toUtf8());
    file.flush();
    file.close();

    return pacFile;
}

QString PacServer::getPacUrl(const Configuration& configuration) {
    QString pacUrl;
    if (configuration.isUseOnlinePac()) {
        pacUrl = configuration.getPacUrl();
    } else {
        QString pacFile = touchPacFile(configuration.getLocalPort());
        pacUrl = "file://" + pacFile;
    }
//    qDebug() << "pacUrl:" << pacUrl;
    return pacUrl;
}

QString PacServer::getPacAddress(QString host, int port) {
    // SOCKS5 127.0.0.1:1080
    return QString("SOCKS5 %1:%2").arg(host, QString::number(port));
}
