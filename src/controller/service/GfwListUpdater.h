//
// Created by wallestar on 5/29/19.
//

#ifndef GFWLISTUPDATER_H
#define GFWLISTUPDATER_H
#include <QString>
#include <QList>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QFile>

class Configuration;
class GfwListUpdater: public QObject {
Q_OBJECT
public:
    GfwListUpdater(QObject *parent = nullptr);
    static bool mergeAndWritePacFile(const QByteArray& raw);
    static QString mergePacFile(const QByteArray& raw);
    void updatePacFromGFWList(const Configuration& configuration);
private:
    void httpDownload();
    void startRequest(QUrl url);
    void httpFinished();
    void httpReadyRead();
    void updateDataReadProgress(qint64 bytesRead, qint64 totalBytes);

    const static QString GFWLIST_URL;
    QUrl url;
    QNetworkAccessManager qnam;
    QNetworkReply *reply = nullptr;
    int httpGetId;
    bool httpRequestAborted;
    QByteArray gfwContent;
};


#endif //GFWLISTUPDATER_H
