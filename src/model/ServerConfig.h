/**
 * Copyright (C) 2019 ~ 2020 WalleStar
 *
 * Author:     WalleStar <joseph1002.lee@gmail.com>
 * Maintainer: WalleStar <joseph1002.lee@gmail.com>
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

#ifndef SERVERCONFIG_H
#define SERVERCONFIG_H

#include "stdafx.h"

class ServerConfig {
public:
    static const int DefaultServerTimeoutSec;
    static const int MaxServerTimeoutSec;
    static const QRegularExpression UrlFinder;
    static const QRegularExpression DetailsParser;

    ServerConfig();
    uint getHashCode();

    bool equals(const ServerConfig& obj);

    QString friendlyName() const;

    static QString formatHostName(QString hostName);

    static QList<ServerConfig> getServers(QString ssURL);

    static std::optional<ServerConfig> fromJson(const QJsonObject &json);
    QJsonObject toJson() const;

    QString identifier();

    const QString &getServer() const;

    void setServer(const QString &server);

    int getServerPort() const;

    void setServerPort(int serverPort);

    const QString &getPassword() const;

    void setPassword(const QString &password);

    const QString &getMethod() const;

    void setMethod(const QString &method);

    const QString &getPlugin() const;

    void setPlugin(const QString &plugin);

    const QString &getPluginOpts() const;

    void setPluginOpts(const QString &pluginOpts);

    const QString &getPluginArgs() const;

    void setPluginArgs(const QString &pluginArgs);

    const QString &getRemarks() const;

    void setRemarks(const QString &remarks);

    int getTimeout() const;

    void setTimeout(int timeout);


private:
    static std::optional<ServerConfig> parseLegacyURL(QString ssURL);

    QString server;
    int serverPort;
    QString password;
    QString method;
    QString plugin;
    QString pluginOpts;
    QString pluginArgs;
    QString remarks;
    int timeout;
};

#endif // SERVERCONFIG_H
