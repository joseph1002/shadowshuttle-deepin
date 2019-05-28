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

#ifndef SOCKS5PROXY_H
#define SOCKS5PROXY_H

#include <QtCore>
#include <QtShadowsocks>
#include <com_deepin_daemon_network.h>

class ServerConfig;
using NetworkInter = com::deepin::daemon::Network;

class Socks5Proxy : public QObject {
Q_OBJECT
public:
    Socks5Proxy(QObject *parent = nullptr);

    void launchSocksService(const ServerConfig& serverConfig, int localPort);
    void stopSocksService();
    void systemProxyToNone();
    void systemProxyToAuto(QString pacURI);
    void systemProxyToManual(QString localAddress, int port);

signals:
    void runningStateChanged(bool);
    void newBytesReceived(quint64);
    void newBytesSent(quint64);
    void bytesReceivedChanged(quint64);
    void bytesSentChanged(quint64);
    void tcpLatencyAvailable(int);

private:
    bool startSocksService();
    NetworkInter networkInter;
    std::unique_ptr<QSS::Controller> controller;
    std::unique_ptr<QSS::Profile> currentProfile;

    void disconnectController();
    void connectController();

    std::unique_ptr<QSS::Profile> getProfile(const ServerConfig& serverConfig, int localPort);

    void setProxyMethod(QString proxyMethod);
};

#endif // SOCKS5PROXY_H
