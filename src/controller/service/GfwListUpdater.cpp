//
// Created by wallestar on 5/29/19.
//

#include "GfwListUpdater.h"
#include "model/Configuration.h"
#include "PacServer.h"
#include "common/utils.h"
#include "controller/ShadowsocksController.h"

const QString GfwListUpdater::GFWLIST_URL = "https://raw.githubusercontent.com/gfwlist/gfwlist/master/gfwlist.txt";

GfwListUpdater::GfwListUpdater(QObject *parent)
        : QObject(parent) {
}

void GfwListUpdater::httpDownload() {
    gfwContent.clear();
    url = GFWLIST_URL;

    // schedule the request
    httpRequestAborted = false;
    startRequest(url);
}

void GfwListUpdater::startRequest(QUrl url) {
    reply = qnam.get(QNetworkRequest(url));
    connect(reply, &QNetworkReply::finished,
            this, &GfwListUpdater::httpFinished);
    connect(reply, &QNetworkReply::readyRead,
            this, &GfwListUpdater::httpReadyRead);
    connect(reply, &QNetworkReply::downloadProgress,
            this, &GfwListUpdater::updateDataReadProgress);
}

void GfwListUpdater::httpFinished() {
    if (httpRequestAborted) {
        reply->deleteLater();
        reply = nullptr;
        return;
    }

    QVariant redirectionTarget = reply->attribute(QNetworkRequest::RedirectionTargetAttribute);
    if (reply->error()) {
        qDebug() << QString("Download failed: %1.").arg(reply->errorString());
    } else if (!redirectionTarget.isNull()) {
        QUrl newUrl = url.resolved(redirectionTarget.toUrl());
        url = newUrl;
        reply->deleteLater();
        reply = nullptr;
        startRequest(url);
        return;
    } else {
        if (gfwContent.size() > 0) {
            mergeAndWritePacFile(gfwContent);
            gfwContent.clear();
        }
    }

    reply->deleteLater();
    reply = nullptr;
}

void GfwListUpdater::httpReadyRead() {
    // this slot gets called every time the QNetworkReply has new data.
    // We read all of its new data and write it into the file.
    // That way we use less RAM than when reading it at the finished()
    // signal of the QNetworkReply
    const QByteArray bytes = reply->readAll();
    gfwContent.append(bytes);
}


void GfwListUpdater::updateDataReadProgress(qint64 bytesRead, qint64 totalBytes) {
    if (httpRequestAborted)
        return;
}

bool GfwListUpdater::mergeAndWritePacFile(const QByteArray &raw) {
    QString abpContent = mergePacFile(raw);
    ShadowsocksController::Instance().touchPacFile(abpContent);
    return true;
}

QString GfwListUpdater::mergePacFile(const QByteArray &raw) {
    QDebug debug = qDebug();
    debug.noquote();
    QByteArray array = QByteArray::fromBase64(raw);

    QString gfwJson = getRulesJson(array);

    QString zipFilePath = Utils::getQrcDataPath(PacServer::USER_ABP_FILE_ZIP);
    QString abpContent = Utils::uncompressFile(zipFilePath, PacServer::USER_ABP_FILE);
    abpContent.replace("__RULES__", gfwJson);

    QString userRule = getUserRules();
    abpContent.replace("__USERRULES__", getRulesJson(userRule.toUtf8()));

    return abpContent;
}

void GfwListUpdater::updatePacFromGFWList(const Configuration &configuration) {
    httpDownload();
}

QString GfwListUpdater::getUserRules() {
    QString userRulePath = ShadowsocksController::Instance().touchUserRuleFile();
    QFile userRuleFile(userRulePath);
    if (!userRuleFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return "";
    }

    return userRuleFile.readAll();
}

QString GfwListUpdater::getRulesJson(const QByteArray &array) {
    QTextStream in(array);

    QJsonArray jsonArray;
    while (!in.atEnd()) {
        QString line = in.readLine();
        if (!(line.startsWith("!") || line.startsWith("[") || line.trimmed().isEmpty())) {
            QJsonValue jsValue(line);
            jsonArray.append(jsValue);
        }
    }

    QJsonDocument doc(jsonArray);
    return doc.toJson(QJsonDocument::Indented);
}
