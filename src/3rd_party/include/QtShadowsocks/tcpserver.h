/*
 * tcpserver.h - Shadowsocks TCP Server
 *
 * Copyright (C) 2015-2017 Symeon Huang <hzwhuang@gmail.com>
 *
 * This file is part of the libQtShadowsocks.
 *
 * libQtShadowsocks is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * libQtShadowsocks is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with libQtShadowsocks; see the file LICENSE. If not, see
 * <http://www.gnu.org/licenses/>.
 */

#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <QTcpServer>
#include <list>
#include <memory>
#include "address.h"
#include "export.h"
#include "tcprelay.h"

namespace QSS {

class QSS_EXPORT TcpServer : public QTcpServer
{
    Q_OBJECT
public:
    TcpServer(std::string method,
              std::string password,
              int timeout,
              bool is_local,
              bool auto_ban,
              Address serverAddress);
    ~TcpServer();

    TcpServer(const TcpServer &) = delete;

signals:
    void bytesRead(quint64);
    void bytesSend(quint64);
    void latencyAvailable(int);

protected:
    void incomingConnection(qintptr socketDescriptor) Q_DECL_OVERRIDE;

private:
    const std::string method;
    const std::string password;
    const bool isLocal;
    const bool autoBan;
    const Address serverAddress;
    const int timeout;

    std::list<std::shared_ptr<TcpRelay> > conList;
};

}

#endif // TCPSERVER_H
