#include "ProxyManager.h"
#include "common/utils.h"
#include "model/ServerConfig.h"

bool operator==(const QSS::Profile& pf1, const QSS::Profile& pf2) {
    return pf1.name() == pf2.name()
            && pf1.method() == pf2.method()
            && pf1.serverAddress() == pf2.serverAddress()
            && pf1.serverPort() == pf2.serverPort()
            && pf1.timeout() == pf2.timeout()
            && pf1.password() == pf2.password();
}

ProxyManager::ProxyManager(QObject *parent) : QObject(parent),
    networkInter("com.deepin.daemon.Network", "/com/deepin/daemon/Network", QDBusConnection::sessionBus(), this) {
    controller = nullptr;
    currentProfile = nullptr;
}

void ProxyManager::systemProxyToNone() {
    setProxyMethod("none");
}

void ProxyManager::systemProxyToAuto(QString pacURI) {
    auto w = new QDBusPendingCallWatcher(networkInter.SetAutoProxy(pacURI), this);
    QObject::connect(w, &QDBusPendingCallWatcher::finished, [=]() {
        qDebug() << "set pac URI " << pacURI;
        setProxyMethod("auto");
    });
}

void ProxyManager::systemProxyToManual(QString localAddress, int port) {
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

void ProxyManager::setProxyMethod(QString proxyMethod) {
    QDBusPendingCallWatcher *w = new QDBusPendingCallWatcher(networkInter.SetProxyMethod(proxyMethod), this);
    QObject::connect(w, &QDBusPendingCallWatcher::finished, [=] {
        qDebug() << "success to set proxy method " << proxyMethod;
    });
    connect(w, &QDBusPendingCallWatcher::finished, w, &QDBusPendingCallWatcher::deleteLater);
}

void ProxyManager::launchSocksService(const ServerConfig& serverConfig, int localPort) {
    std::unique_ptr<QSS::Profile> newProfile = getProfile(serverConfig, localPort);

    if (currentProfile != nullptr && *newProfile == *currentProfile) {
        return;
    }

    stopSocksService();
    controller = std::make_unique<QSS::Controller>(*newProfile.get(), true, true, this);
    startSocksService();
    currentProfile = std::move(newProfile);
}

bool ProxyManager::startSocksService() {
    bool flag = controller->start();
    if (!flag) {
        Utils::critical("start fail");
        controller = nullptr;
        currentProfile = nullptr;
    }
    connectController();
    return flag;
}

void ProxyManager::stopSocksService() {
    if (currentProfile) {
        controller->stop();
        disconnectController();
    }
    controller = nullptr;
    currentProfile = nullptr;
}

void ProxyManager::disconnectController() {
    disconnect(controller.get());
}

void ProxyManager::connectController() {
    connect(controller.get(), &QSS::Controller::bytesReceivedChanged, this, &ProxyManager::bytesReceivedChanged);
    connect(controller.get(), &QSS::Controller::bytesSentChanged, this, &ProxyManager::bytesSentChanged);
    connect(controller.get(), &QSS::Controller::newBytesReceived, this, &ProxyManager::newBytesReceived);
    connect(controller.get(), &QSS::Controller::newBytesSent, this, &ProxyManager::newBytesSent);
    connect(controller.get(), &QSS::Controller::runningStateChanged, this, &ProxyManager::runningStateChanged);
    connect(controller.get(), &QSS::Controller::tcpLatencyAvailable, this, &ProxyManager::tcpLatencyAvailable);
}

std::unique_ptr<QSS::Profile> ProxyManager::getProfile(const ServerConfig& serverConfig, int localPort) {
    std::unique_ptr<QSS::Profile> profile(new QSS::Profile());

    profile->setName(serverConfig.getRemarks().toStdString());
    QString server = serverConfig.getServer();
    if (server.isEmpty()) {
        Utils::critical("server is empty.");
    }
    profile->setServerAddress(server.toStdString());
    QString password = serverConfig.getPassword();
    if (password.isEmpty()) {
        Utils::warning("password is empty.");
    }
    profile->setPassword(password.toStdString());
    int server_port = serverConfig.getServerPort();
    if (server_port == 0) {
        Utils::warning("server port is 0.");
    }
    profile->setServerPort(server_port);
    QString method = serverConfig.getMethod();
    if (method.isEmpty()) {
        Utils::warning("method is empty.");
    }
    profile->setMethod(method.toStdString());
    int timeout = serverConfig.getTimeout() * 10;
    if (timeout == 0) {
        Utils::warning("timeout is 0. we will change the value to 6.");
        timeout = 6;
    }
    profile->setTimeout(timeout);
    profile->setLocalPort(localPort);
    QString local_addr = "127.0.0.1";
    profile->setLocalAddress(local_addr.toStdString());

    return profile;
}

