#include "ProxyManager.h"
#include "utils/utils.h"
#include "model/ServerConfig.h"

ProxyManager::ProxyManager(QObject *parent) : QObject(parent) {
    isRunning = false;
    controller = nullptr;
}

void ProxyManager::setConfig(const ServerConfig& serverConfig, int localPort) {
    if (controller != nullptr) {
        if (isRunning) {
            controller->stop();
            isRunning = false;
        }
        disconnectController();
        delete controller;
    }
    QSS::Profile profile;
    getProfile(serverConfig, localPort, profile);
    // profile.enableDebug();
    controller = new QSS::Controller(profile, true, true, this);
    connectController();
}

bool ProxyManager::start() {
    bool flag = controller->start();
    if (!flag) {
        Utils::critical("start fail");
    } else {
        isRunning = true;
    }
    return flag;
}

void ProxyManager::stop() {
    if (isRunning) {
        controller->stop();
        isRunning = false;
    }
}

void ProxyManager::disconnectController() {
    disconnect(controller);
}

void ProxyManager::connectController() {
    connect(controller, &QSS::Controller::bytesReceivedChanged, this, &ProxyManager::bytesReceivedChanged);
    connect(controller, &QSS::Controller::bytesSentChanged, this, &ProxyManager::bytesSentChanged);
    connect(controller, &QSS::Controller::newBytesReceived, this, &ProxyManager::newBytesReceived);
    connect(controller, &QSS::Controller::newBytesSent, this, &ProxyManager::newBytesSent);
    connect(controller, &QSS::Controller::runningStateChanged, this, &ProxyManager::runningStateChanged);
    connect(controller, &QSS::Controller::tcpLatencyAvailable, this, &ProxyManager::tcpLatencyAvailable);
}

void ProxyManager::getProfile(const ServerConfig& serverConfig, int localPort, QSS::Profile &profile) {
    QString server = serverConfig.getServer();
    if (server.isEmpty()) {
        Utils::critical("server is empty.");
    }
    profile.setServerAddress(server.toStdString());
    QString password = serverConfig.getPassword();
    if (password.isEmpty()) {
        Utils::warning("password is empty.");
    }
    profile.setPassword(password.toStdString());
    int server_port = serverConfig.getServerPort();
    if (server_port == 0) {
        Utils::warning("server port is 0.");
    }
    profile.setServerPort(server_port);
    QString method = serverConfig.getMethod();
    if (method.isEmpty()) {
        Utils::warning("method is empty.");
    }
    profile.setMethod(method.toStdString());
    int timeout = serverConfig.getTimeout() * 10;
    if (timeout == 0) {
        Utils::warning("timeout is 0. we will change the value to 6.");
        timeout = 6;
    }
    profile.setTimeout(timeout);
    profile.setLocalPort(localPort);
    QString local_addr = "127.0.0.1";
    profile.setLocalAddress(local_addr.toStdString());
}
