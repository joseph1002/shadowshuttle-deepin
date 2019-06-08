//#include "stdafx.h"
#include "utils.h"
#include <QApplication>
#include <QDir>
#include <QStandardPaths>
#include <QMessageBox>
#include <QtDBus/QtDBus>

#include <QApplication>
#include <QDateTime>
#include <QDebug>
#include <QDir>
#include <QFontMetrics>
#include <QIcon>
#include <QImageReader>
#include <QLayout>
#include <QPainter>
#include <QPixmap>
#include <QStandardPaths>
#include <QString>
#include <QWidget>
#include <QtDBus>
#include <QtMath>
#include <QtX11Extras/QX11Info>
#include <X11/extensions/shape.h>
#include <fstream>
#include <qdiriterator.h>
#include <sstream>
#include <stdio.h>
#include <string>
#include <time.h>
#include <unordered_set>
#include "common/constant.h"
#include <JlCompress.h>

namespace Utils {
QString getQrcTrayIconPath(QString imageName) {
    return QString(":/tray/%1").arg(imageName);
}

QString getQrcDataPath(QString fileName) {
    return QString(":/data/%1").arg(fileName);
}

qint64 getTimestamp() {
    return QDateTime::currentDateTime().toMSecsSinceEpoch(); //毫秒级
}

QString configPath() {
    return QDir(QDir(QStandardPaths::standardLocations(QStandardPaths::ConfigLocation).first()).filePath(
                    qApp->organizationName())).filePath(qApp->applicationName());
}

QString tmpPath() {
    return QDir(QDir(QStandardPaths::standardLocations(QStandardPaths::TempLocation).first()).filePath(
                    qApp->organizationName())).filePath(qApp->applicationName());
}

QString uncompressFile(QString fileCompressed, QString fileName, QString fileDest) {
    if (fileDest.isEmpty()) {
        fileDest = tmpPath() + "/" + fileName;
    }

    QFile file(fileDest);
    QFileInfo fileinfo(file);
    QDir dir = fileinfo.absoluteDir();
    if (!dir.exists()) {
        dir.mkpath(dir.absolutePath());
    }

    if (!file.exists()) {
        QString filePath = JlCompress::extractFile(fileCompressed, fileName, fileDest);
        qDebug() << "filePath" << filePath << "        fileDest:" << fileDest;
    }

    file.open(QIODevice::ReadOnly | QIODevice::Text);

    QString fileContent = file.readAll();
    file.close();

    return fileContent;
}

void critical(QString msg) {
    QMessageBox::critical(nullptr, "critical", msg);
}

void warning(QString msg) {
    QMessageBox::warning(nullptr, "warning", msg);
}

void info(QString msg) {
    QMessageBox::information(nullptr, "info", msg);
}

// copy from deepin-system-monitor

QSize getRenderSize(int fontSize, QString string) {
    QFont font;
    font.setPointSize(fontSize);
    QFontMetrics fm(font);

    int width = 0;
    int height = 0;
    foreach (auto line, string.split("\n")) {
        int lineWidth = fm.width(line);
        int lineHeight = fm.height();

        if (lineWidth > width) {
            width = lineWidth;
        }
        height += lineHeight;
    }

    return QSize(width, height);
}

QString formatBandwidth(double v) {
    static const char *orders[] = {"KB/s", "MB/s", "GB/s", "TB/s"};

    return formatUnitSize(v, orders, sizeof(orders) / sizeof(orders[0]));
}

QString formatByteCount(double v) {
    static const char *orders[] = {"B", "KB", "MB", "GB", "TB"};

    return formatUnitSize(v, orders, sizeof(orders) / sizeof(orders[0]));
}

QString formatUnitSize(double v, const char **orders, int nb_orders) {
    int order = 0;
    while (v >= 1024 && order + 1 < nb_orders) {
        order++;
        v = v / 1024;
    }
    char buffer1[30];
    snprintf(buffer1, sizeof(buffer1), "%.1lf %s", v, orders[order]);

    return QString(buffer1);
}

QString formatMillisecond(int millisecond) {
    if (millisecond / 1000 < 3600) {
        // At least need return 1 seconds.
        return QDateTime::fromTime_t(std::max(1, millisecond / 1000)).toUTC().toString("mm:ss");
    } else {
        return QDateTime::fromTime_t(millisecond / 1000).toUTC().toString("hh:mm:ss");
    }
}

QString getQrcPath(QString imageName) {
    return QString(":/image/%1").arg(imageName);
}

QString getQssPath(QString qssName) {
    return QString(":/qss/%1").arg(qssName);
}

qreal easeInOut(qreal x) {
    return (1 - qCos(M_PI * x)) / 2;
}

qreal easeInQuad(qreal x) {
    return qPow(x, 2);
}

qreal easeOutQuad(qreal x) {
    return -1 * qPow(x - 1, 2) + 1;
}

qreal easeInQuint(qreal x) {
    return qPow(x, 5);
}

qreal easeOutQuint(qreal x) {
    return qPow(x - 1, 5) + 1;
}

void setFontSize(QPainter &painter, int textSize) {
    QFont font = painter.font();
    font.setPointSize(textSize);
    painter.setFont(font);
}

// reference: https://github.com/linuxdeepin/dtkwidget/blob/master/src/util/ddesktopservices_linux.cpp
#define EASY_CALL_DBUS(name)\
    QDBusInterface *interface = fileManager1DBusInterface();\
    return interface && interface->call(#name, urls2uris(urls), startupId).type() != QDBusMessage::ErrorMessage;

QDBusInterface *fileManager1DBusInterface()
{
    static QDBusInterface interface(QStringLiteral("org.freedesktop.FileManager1"),
                                        QStringLiteral("/org/freedesktop/FileManager1"),
                                        QStringLiteral("org.freedesktop.FileManager1"));
    return &interface;
}

QStringList urls2uris(const QList<QUrl> &urls)
{
    QStringList list;

    list.reserve(urls.size());

    for (const QUrl &url : urls) {
        list << url.toString();
    }

    return list;
}

QList<QUrl> path2urls(const QList<QString> &paths)
{
    QList<QUrl> list;

    list.reserve(paths.size());

    for (const QString &path : paths) {
        list << QUrl::fromLocalFile(path);
    }

    return list;
}

bool showFileItem(QString localFilePath, const QString &startupId)
{
    return showFileItem(QUrl::fromLocalFile(localFilePath), startupId);
}

bool showFileItems(const QList<QString> localFilePaths, const QString &startupId)
{
    return showFileItems(path2urls(localFilePaths), startupId);
}

bool showFileItem(QUrl url, const QString &startupId)
{
    return showFileItems(QList<QUrl>() << url, startupId);
}

bool showFileItems(const QList<QUrl> urls, const QString &startupId)
{
    EASY_CALL_DBUS(ShowItems)
}


}
