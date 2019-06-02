#include "Socks5Proxy.h"
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

Socks5Proxy::Socks5Proxy(QObject *parent) : QObject(parent) {
    controller = nullptr;
    currentProfile = nullptr;
}

Socks5Proxy::~Socks5Proxy() {
    stop();
}

void Socks5Proxy::start(const ServerConfig& serverConfig, int localPort) {
    std::unique_ptr<QSS::Profile> newProfile = getProfile(serverConfig, localPort);

    if (currentProfile != nullptr && *newProfile == *currentProfile) {
        return;
    }

    stop();
    controller = new QSS::Controller(*newProfile.get(), true, true, this);
    if (start()) {
        currentProfile = std::move(newProfile);
    }
}

bool Socks5Proxy::start() {
    bool flag = controller->start();
    if (!flag) {
        Utils::critical("start fail");
        controller = nullptr;
        currentProfile = nullptr;
    } else {
        connectController();
    }
    return flag;
}

void Socks5Proxy::stop() {
    if (controller) {
        controller->stop();
        disconnectController();
    }
    controller = nullptr;
    currentProfile = nullptr;
}

void Socks5Proxy::disconnectController() {
    disconnect(controller);
}

void Socks5Proxy::connectController() {
    connect(controller, &QSS::Controller::bytesReceivedChanged, this, &Socks5Proxy::bytesReceivedChanged);
    connect(controller, &QSS::Controller::bytesSentChanged, this, &Socks5Proxy::bytesSentChanged);
    connect(controller, &QSS::Controller::newBytesReceived, this, &Socks5Proxy::newBytesReceived);
    connect(controller, &QSS::Controller::newBytesSent, this, &Socks5Proxy::newBytesSent);
    connect(controller, &QSS::Controller::runningStateChanged, this, &Socks5Proxy::runningStateChanged);
    connect(controller, &QSS::Controller::tcpLatencyAvailable, this, &Socks5Proxy::tcpLatencyAvailable);
}

std::unique_ptr<QSS::Profile> Socks5Proxy::getProfile(const ServerConfig& serverConfig, int localPort) {
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

