/**
 * Copyright (C) 2017 ~ 2018 PikachuHy
 *
 * Author:     PikachuHy <pikachuhy@163.com>
 * Maintainer: PikachuHy <pikachuhy@163.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef UTILS_H
#define UTILS_H

#include <QString>
#include <QDateTime>
#include <QFileInfoList>
#include <QLayout>
#include <QObject>
#include <QPainter>
#include <QString>
#include <unordered_set>

namespace Utils {
    QString getIconQrcPath(QString imageName);

    QString getQrcDataPath(QString fileName);

    qint64 getTimestamp();

    QString configPath();

    QString tmpPath();

    QString uncompressFile(QString fileCompressed, QString fileName, QString fileDest = QString());

    void critical(QString msg);

    void warning(QString msg);

    void info(QString msg);
// copy from deepin-system-monitor

    QSize getRenderSize(int fontSize, QString string);

    QString formatBandwidth(double v);

    QString formatByteCount(double v);

    QString formatMillisecond(int millisecond);

    QString formatUnitSize(double v, const char **orders, int nb_orders);

    QString getQrcTrayIconPath(QString imageName);

    QString getQssPath(QString qssName);

    qreal easeInOut(qreal x);

    qreal easeInQuad(qreal x);

    qreal easeInQuint(qreal x);

    qreal easeOutQuad(qreal x);

    qreal easeOutQuint(qreal x);

    void setFontSize(QPainter &painter, int textSize);

    bool showFileItem(QString localFilePath, const QString &startupId = QString());

    bool showFileItems(const QList<QString> localFilePaths, const QString &startupId = QString());

    bool showFileItem(QUrl url, const QString &startupId = QString());

    bool showFileItems(const QList<QUrl> urls, const QString &startupId = QString());

    void moveToCenter(QWidget *w);

}
#endif // UTILS_H
