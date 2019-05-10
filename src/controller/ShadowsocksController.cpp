#include "ShadowsocksController.h"

ShadowsocksController& ShadowsocksController::Instance() {
    static ShadowsocksController controller;

    // Return a reference to our instance.
    return controller;
}

ShadowsocksController::ShadowsocksController():configuration(Configuration::Load()) {

}

ServerConfig ShadowsocksController::getCurrentServer()
{
    return configuration.getCurrentServer();
}

const Configuration& ShadowsocksController::getCurrentConfiguration() const
{
    return configuration;
}

void ShadowsocksController::saveServers(const QList<ServerConfig>& servers, int localPort, bool portableMode)
{
    configuration.setServerConfigs(servers);
    configuration.setLocalPort(localPort);
    configuration.setPortableMode(portableMode);
    Configuration::save(configuration);
}

void ShadowsocksController::saveServers(const QList<ServerConfig>& servers, int localPort, int index, bool portableMode)
{
    configuration.setServerConfigs(servers);
    configuration.setLocalPort(localPort);
    configuration.setIndex(index);
    configuration.setPortableMode(portableMode);
    Configuration::save(configuration);
}

BaseResult ShadowsocksController::addServerBySSURL(QString ssURL)
{
    if (ssURL.isEmpty() || ssURL.trimmed().isEmpty())
        return BaseResult::fail();

    auto servers = ServerConfig::getServers(ssURL);
    if (servers.isEmpty())
        return BaseResult::fail();

    foreach (ServerConfig serverConfig, servers)
    {
        configuration.addServerConfig(serverConfig);
    }
    configuration.setIndex(configuration.getServerConfigs().size() - 1);
    saveConfig(configuration);
    return BaseResult::success();

}

void ShadowsocksController::toggleEnable(bool enabled)
{
    configuration.setEnabled(enabled);
    saveConfig(configuration);
    //    if (EnableStatusChanged != null)
    //    {
    //        EnableStatusChanged(this, new EventArgs());
    //    }
}

void ShadowsocksController::toggleGlobal(bool global)
{
    configuration.setGlobal(global);
    saveConfig(configuration);
    //    if (EnableGlobalChanged != null)
    //    {
    //        EnableGlobalChanged(this, new EventArgs());
    //    }
}

void ShadowsocksController::toggleShareOverLAN(bool enabled)
{
    configuration.setShareOverLan(enabled);
    saveConfig(configuration);
    //    if (ShareOverLANStatusChanged != null)
    //    {
    //        ShareOverLANStatusChanged(this, new EventArgs());
    //    }
}

//void ShadowsocksController::SaveProxy(ProxyConfig proxyConfig)
//{
//    configuration.proxy = proxyConfig;
//    SaveConfig(configuration);
//}


void ShadowsocksController::toggleVerboseLogging(bool enabled)
{
    configuration.setVerboseLogging(enabled);
    saveConfig(configuration);
    //    if (VerboseLoggingStatusChanged != null)
    //    {
    //        VerboseLoggingStatusChanged(this, new EventArgs());
    //    }
}

void ShadowsocksController::ShadowsocksController::selectServerIndex(int index)
{
    configuration.setIndex(index);
    configuration.setStrategy("");
    saveConfig(configuration);
}

void ShadowsocksController::selectStrategy(QString strategyID)
{
    configuration.setIndex(-1);
    configuration.setStrategy(strategyID);
    saveConfig(configuration);
}

QString ShadowsocksController::getServerURLForCurrentServer()
{
    ServerConfig server = getCurrentServer();
    return getServerURL(server);
}

QString ShadowsocksController::getServerURL(const ServerConfig& serverConfig)
{
    QString tag = "";
    QString url = "";

    if (serverConfig.getPlugin().isEmpty()) {
        // For backwards compatiblity, if no plugin, use old url format
        QString parts = QString("%1:%2@%3:%4").arg(serverConfig.getMethod(), serverConfig.getPassword(),
                                   serverConfig.getServer(), QString::number(serverConfig.getServerPort()));

        QString base64 = QString::fromUtf8(parts.toUtf8().toBase64());
        url = base64;
    } else
    {
        // SIP002
        QString parts = QString("%1:%2").arg(serverConfig.getMethod(), serverConfig.getPassword());
        QString base64 = QString::fromUtf8(parts.toUtf8().toBase64());
        QString websafeBase64 = base64.replace('+', '-').replace('/', '_');
        if (websafeBase64.endsWith("=")) {
            websafeBase64.chop(1);
        }

        QString pluginPart = serverConfig.getPlugin();
        if (!serverConfig.getPluginOpts().trimmed().isEmpty())
        {
            pluginPart += ";" + serverConfig.getPluginOpts();
        }

        url = QString(
            "%1@%2:%3/?plugin=%4").arg(
            websafeBase64,
            ServerConfig::formatHostName(serverConfig.getServer()),
            QString::number(serverConfig.getServerPort()),
            QString::fromUtf8(QUrl(pluginPart).toEncoded()));
    }

    if (!serverConfig.getRemarks().isEmpty())
    {
        tag = QString("#%1").arg(QString::fromUtf8(QUrl(serverConfig.getRemarks()).toEncoded()));
    }
    return QString("ss://%1%2").arg(url, tag);
}

//void ShadowsocksController::updatePACFromGFWList()
//{
//    if (gfwListUpdater != null)
//    {
//        gfwListUpdater.UpdatePACFromGFWList(configuration);
//    }
//}

//void UpdateStatisticsConfiguration(bool enabled)
//{
//    if (availabilityStatistics == null) return;
//    availabilityStatistics.UpdateConfiguration(this);
//    configuration.availabilityStatistics = enabled;
//    SaveConfig(configuration);
//}

void ShadowsocksController::savePACUrl(QString pacUrl)
{
    configuration.setPacUrl(pacUrl);
    saveConfig(configuration);
//    if (ConfigChanged != null)
//    {
//        ConfigChanged(this, new EventArgs());
//    }
}

void ShadowsocksController::useOnlinePAC(bool useOnlinePac)
{
    configuration.setUseOnlinePac(useOnlinePac);
    saveConfig(configuration);
//    if (ConfigChanged != null)
//    {
//        ConfigChanged(this, new EventArgs());
//    }
}

void ShadowsocksController::toggleSecureLocalPac(bool enabled)
{
    configuration.setSecureLocalPac(enabled);
    saveConfig(configuration);
//    if (ConfigChanged != null)
//    {
//        ConfigChanged(this, new EventArgs());
//    }
}

void ShadowsocksController::toggleCheckingUpdate(bool enabled)
{
    configuration.setAutoCheckUpdate(enabled);
    Configuration::save(configuration);
//    if (ConfigChanged != null)
//    {
//        ConfigChanged(this, new EventArgs());
//    }
}

void ShadowsocksController::toggleCheckingPreRelease(bool enabled)
{
    configuration.setCheckPreRelease(enabled);
    Configuration::save(configuration);
//    if (ConfigChanged != null)
//    {
//        ConfigChanged(this, new EventArgs());
//    }
}

//void ShadowsocksController::saveLogViewerConfig(LogViewerConfig newConfig)
//{
//    configuration.logViewer = newConfig;
//    newConfig.SaveSize();
//    Configuration::save(configuration);
////    if (ConfigChanged != null)
////    {
////        ConfigChanged(this, new EventArgs());
////    }
//}

//void ShadowsocksController::saveHotkeyConfig(HotkeyConfig newConfig)
//{
//    configuration.hotkey = newConfig;
//    SaveConfig(configuration);
////    if (ConfigChanged != null)
////    {
////        ConfigChanged(this, new EventArgs());
////    }
//}

//void ShadowsocksController::UpdateLatency(Server server, TimeSpan latency)
//{
//    if (configuration.availabilityStatistics)
//    {
//        availabilityStatistics.UpdateLatency(server, (int)latency.TotalMilliseconds);
//    }
//}

//void ShadowsocksController::updateInboundCounter(Server server, long n)
//{
//    Interlocked.Add(ref _inboundCounter, n);
//    if (configuration.availabilityStatistics)
//    {
//        availabilityStatistics.UpdateInboundCounter(server, n);
//    }
//}

//void ShadowsocksController::updateOutboundCounter(Server server, long n)
//{
//    Interlocked.Add(ref _outboundCounter, n);
//    if (configuration.availabilityStatistics)
//    {
//        availabilityStatistics.UpdateOutboundCounter(server, n);
//    }
//}


void ShadowsocksController::saveConfig(Configuration& newConfig)
{
    Configuration::save(newConfig);
}

BaseResult ShadowsocksController::importFrom(QString fileName) {
    if (fileName == configuration.getConfigFile())
        return BaseResult::success();

     std::optional<Configuration> opt = Configuration::fromFile(fileName);
     if (!opt.has_value())
         return BaseResult::fail();

     configuration = opt.value();
     saveConfig(configuration);

     return BaseResult::success();
}

BaseResult ShadowsocksController::exportAs(QString fileName) {
    if (fileName == configuration.getConfigFile())
        return BaseResult::success();

    return Configuration::toFile(configuration, fileName);
}
