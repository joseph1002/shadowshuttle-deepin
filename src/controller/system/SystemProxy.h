//
// Created by wallestar on 5/29/19.
//

#ifndef SYSTEMPROXY_H
#define SYSTEMPROXY_H

#include <QString>
#include <com_deepin_daemon_network.h>

using NetworkInter = com::deepin::daemon::Network;

class SystemProxy : public QObject {
Q_OBJECT
public:
    static SystemProxy& Instance();
    SystemProxy();
    void systemProxyToNone();
    void systemProxyToAuto(QString pacURI);
    void systemProxyToManual(QString localAddress, int port);

private:
    void setProxyMethod(QString proxyMethod);
    NetworkInter networkInter;

};


#endif //SYSTEMPROXY_H
