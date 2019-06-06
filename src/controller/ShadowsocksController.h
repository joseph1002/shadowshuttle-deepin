//
// Created by wallestar on 4/19/19.
//

#ifndef SHADOWSOCKSCONTROLLER_H
#define SHADOWSOCKSCONTROLLER_H

#include "model/Configuration.h"
#include "model/ServerConfig.h"

class SystemProxy;
class PacServer;
class GfwListUpdater;

class ShadowsocksController {
public:
    static ShadowsocksController& Instance();
    // delete copy and move constructors and assign operators
    ShadowsocksController(ShadowsocksController const&) = delete;             // Copy construct
    ShadowsocksController(ShadowsocksController&&) = delete;                  // Move construct
    ShadowsocksController& operator=(ShadowsocksController const&) = delete;  // Copy assign
    ShadowsocksController& operator=(ShadowsocksController &&) = delete;      // Move assign

    ServerConfig getCurrentServer();
    const Configuration& getConfiguration() const;

    void saveServers(const QList<ServerConfig>& servers, int localPort, int index, bool portableMode);

    BaseResult addServerBySSURL(QString ssURL);
    void saveConfig(Configuration& newConfig);

    void toggleEnable(bool enabled);
    void toggleGlobal(bool global);
    void toggleShareOverLAN(bool enabled);
    // void SaveProxy(ProxyConfig proxyConfig);
    void toggleVerboseLogging(bool enabled);
    void selectServerIndex(int index);
    void selectStrategy(QString strategyID);

    QString getServerURLForCurrentServer();
    static QString getServerURL(const ServerConfig& serverConfig);

    // void updateStatisticsConfiguration(bool enabled);
    void savePacUrl(QString pacUrl);
    void useOnlinePac(bool useOnlinePac);
    void toggleSecureLocalPac(bool enabled);
    void toggleCheckingUpdate(bool enabled);
    void toggleCheckingPreRelease(bool enabled);
    // void saveLogViewerConfig(LogViewerConfig newConfig);
    // void saveHotkeyConfig(HotkeyConfig newConfig);
    // void updateLatency(Server server, TimeSpan latency);
    // void updateInboundCounter(ServerConfig server, long n);
    // void updateOutboundCounter(ServerConfig server, long n);
    BaseResult importFrom(QString fileName);
    BaseResult exportAs(QString fileName);

    void updateSystemProxy();
    void closeSystemProxy();

    bool isAutoStart();
    BaseResult autoStart(bool start);
    BaseResult removeAutoStart();
    QString touchPacFile();
    QString touchPacFile(QString fileContent);

    void updatePacFromGFWList();
    QString getLocalPacUrl();
    QString touchUserRuleFile();
private:
    ShadowsocksController();

    Configuration configuration;
    SystemProxy* systemProxy;
    std::unique_ptr<PacServer> pacServer;
    std::unique_ptr<GfwListUpdater> gfwListUpdater;
};

#endif // SHADOWSOCKSCONTROLLER_H

