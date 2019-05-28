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

#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include <QtCore>
#include "ServerConfig.h"
#include "common/BaseResult.h"

class Configuration {
public:
    static QString getConfigFile();
    static Configuration defaultConfiguration();

    static std::optional<Configuration> fromFile(QString configFile);
    static BaseResult toFile(const Configuration& config, QString configFile);

    static Configuration Load();
    static BaseResult save(Configuration& configuration);

    static ServerConfig getDefaultServer();

    static BaseResult checkServerConfig(const ServerConfig& serverConfig);
    static BaseResult assert(bool condition);
    static BaseResult checkPort(int port);
    static BaseResult checkLocalPort(int port);
    static BaseResult checkPassword(const QString& password);
    static BaseResult checkServer(const QString& server);
    static BaseResult checkTimeout(int timeout, int maxTimeout);
    ServerConfig getCurrentServer();
    void addServerConfig(const ServerConfig& serverConfig);

    QList<ServerConfig>& getServerConfigs();
    const QList<ServerConfig>& getServerConfigs() const;

    void setServerConfigs(const QList<ServerConfig> &configs);

    const QString &getStrategy() const;

    void setStrategy(const QString &strategy);

    int getIndex() const;

    void setIndex(int index);

    bool isGlobal() const;

    void setGlobal(bool global);

    bool isEnabled() const;

    void setEnabled(bool enabled);

    bool isShareOverLan() const;

    void setShareOverLan(bool shareOverLan);

    bool getIsDefault() const;

    void setIsDefault(bool isDefault);

    int getLocalPort() const;

    void setLocalPort(int localPort);

    bool isPortableMode() const;

    void setPortableMode(bool portableMode);

    const QString &getPacUrl() const;

    void setPacUrl(const QString &pacUrl);

    bool isUseOnlinePac() const;

    void setUseOnlinePac(bool useOnlinePac);

    bool isSecureLocalPac() const;

    void setSecureLocalPac(bool secureLocalPac);

    bool isAvailabilityStatistics() const;

    void setAvailabilityStatistics(bool availabilityStatistics);

    bool isAutoCheckUpdate() const;

    void setAutoCheckUpdate(bool autoCheckUpdate);

    bool isCheckPreRelease() const;

    void setCheckPreRelease(bool checkPreRelease);

    bool IsVerboseLogging() const;

    void setVerboseLogging(bool verboseLogging);

private:
    QList<ServerConfig> serverConfigs;

    // when strategy is set, index is ignored
    QString strategy;
    int index;
    bool global;
    bool enabled;
    bool shareOverLan;
    bool isDefault;
    int localPort;
    bool portableMode = true;
    QString pacUrl;
    bool useOnlinePac;
    bool secureLocalPac = true;
    bool availabilityStatistics;
    bool autoCheckUpdate;
    bool checkPreRelease;
    bool verboseLogging;

private:
//LogViewerConfig logViewer;
//ProxyConfig proxy;
//HotkeyConfig hotkey;

};

#endif // CONFIGURATION_H
