//
// Created by wallestar on 5/29/19.
//

#include "SystemProxy.h"

SystemProxy& SystemProxy::Instance() {
    static SystemProxy systemProxy;

    return systemProxy;
}

SystemProxy::SystemProxy() :
    networkInter("com.deepin.daemon.Network", "/com/deepin/daemon/Network", QDBusConnection::sessionBus(), this) {
}

void SystemProxy::systemProxyToNone() {
    setProxyMethod("none");
}

void SystemProxy::systemProxyToAuto(QString pacURI) {
    auto w = new QDBusPendingCallWatcher(networkInter.SetAutoProxy(pacURI), this);
    QObject::connect(w, &QDBusPendingCallWatcher::finished, [=]() {
        qDebug() << "set pac URI " << pacURI;
        setProxyMethod("auto");
    });
}

void SystemProxy::systemProxyToManual(QString localAddress, int port) {
    QString type = "socks";
    QString addr = localAddress;
    QString portStr = QString::number(port);
    auto w = new QDBusPendingCallWatcher(networkInter.SetProxy(type, addr, portStr), this);
    QObject::connect(w, &QDBusPendingCallWatcher::finished, [=] {
        qDebug() << "set proxy" << type << addr << portStr;
        setProxyMethod("manual");
    });
    connect(w, &QDBusPendingCallWatcher::finished, w, &QDBusPendingCallWatcher::deleteLater);
}

void SystemProxy::setProxyMethod(QString proxyMethod) {
    QDBusPendingCallWatcher *w = new QDBusPendingCallWatcher(networkInter.SetProxyMethod(proxyMethod), this);
    QObject::connect(w, &QDBusPendingCallWatcher::finished, [=] {
        qDebug() << "success to set proxy method " << proxyMethod;
    });
    connect(w, &QDBusPendingCallWatcher::finished, w, &QDBusPendingCallWatcher::deleteLater);
}
