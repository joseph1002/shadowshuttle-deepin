#include "Configuration.h"
#include "common/utils.h"

using namespace Utils;


QString Configuration::getConfigFile() {
    return QDir(configPath()).filePath("gui-config.json");
}

Configuration Configuration::defaultConfiguration() {
    Configuration configuration;
    configuration.setIndex(0);
    configuration.setIsDefault(true);
    configuration.setLocalPort(1080);
    configuration.setAutoCheckUpdate(true);
    configuration.addServerConfig(getDefaultServer());
    configuration.setIsDefault(true);
    //    logViewer = new LogViewerConfig();
    //    proxy = new ProxyConfig();
    //    hotkey = new HotkeyConfig();
    return configuration;
}

void Configuration::addServerConfig(const ServerConfig& serverConfig) {
    serverConfigs << serverConfig;
}

ServerConfig Configuration::getCurrentServer() {
    if (index >= 0 && index < serverConfigs.length())
        return serverConfigs[index];
    else
        return getDefaultServer();
}

BaseResult Configuration::checkServerConfig(const ServerConfig& serverConfig) {
    BaseResult result = checkServer(serverConfig.getServer());
    if (!result.isOk())
        return result;

    result = checkPassword(serverConfig.getPassword());
    if (!result.isOk())
        return result;

    result = checkPort(serverConfig.getServerPort());
    if (!result.isOk())
        return result;

    return checkTimeout(serverConfig.getTimeout(), ServerConfig::MaxServerTimeoutSec);

}

std::optional<Configuration> Configuration::fromFile(QString configFile) {
    QFile file(configFile);
    if (!file.exists()) {
        return std::nullopt;
    }

    QFileInfo fileinfo(configFile);
    QDir dir = fileinfo.absoluteDir();
    if (!dir.exists()) {
        return std::nullopt;
    }


    file.open(QIODevice::ReadOnly | QIODevice::Text);
    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    qDebug() << doc.toJson();
    file.close();

    Configuration configuration;
    // handling configs. it's the most important
    QJsonObject json = doc.object();
    if (json.contains("configs")) {
        QJsonArray array = json.value("configs").toArray();
        for (const auto it:array) {
            QJsonObject serverJson = it.toObject();
            auto serverOpt = ServerConfig::fromJson(serverJson);
            if (serverOpt.has_value()) {
                configuration.addServerConfig(serverOpt.value());
            }
        }
    }

    /**
  "strategy": null,
  "index": 0,
  "global": false,
  "enabled": true,
  "shareOverLan": false,
  "isDefault": false,
  "localPort": 1080,
  "portableMode": true,
  "pacUrl": "https://raw.githubusercontent.com/gfwlist/gfwlist/master/gfwlist.txt",
  "useOnlinePac": false,
  "secureLocalPac": false,
  "availabilityStatistics": false,
  "autoCheckUpdate": true,
  "checkPreRelease": false,
  "isVerboseLogging": false,
*/
    if (json.contains("strategy"))
        configuration.setStrategy(json.value("strategy").toString());
    if (json.contains("index"))
        configuration.setIndex(json.value("index").toInt());
    if (json.contains("global"))
        configuration.setGlobal(json.value("global").toBool());
    if (json.contains("enabled"))
        configuration.setEnabled(json.value("enabled").toBool());
    if (json.contains("shareOverLan"))
        configuration.setShareOverLan(json.value("shareOverLan").toBool());
    if (json.contains("isDefault"))
        configuration.setIsDefault(json.value("isDefault").toBool());
    if (json.contains("localPort"))
        configuration.setLocalPort(json.value("localPort").toInt());
    if (json.contains("portableMode"))
        configuration.setPortableMode(json.value("portableMode").toBool());
    if (json.contains("pacUrl"))
        configuration.setPacUrl(json.value("pacUrl").toString());
    if (json.contains("useOnlinePac"))
        configuration.setUseOnlinePac(json.value("useOnlinePac").toBool());
    if (json.contains("secureLocalPac"))
        configuration.setSecureLocalPac(json.value("secureLocalPac").toBool());
    if (json.contains("availabilityStatistics"))
        configuration.setSecureLocalPac(json.value("availabilityStatistics").toBool());
    if (json.contains("autoCheckUpdate"))
        configuration.setAutoCheckUpdate(json.value("autoCheckUpdate").toBool());
    if (json.contains("checkPreRelease"))
        configuration.setCheckPreRelease(json.value("checkPreRelease").toBool());
    if (json.contains("isVerboseLogging"))
        configuration.setVerboseLogging(json.value("isVerboseLogging").toBool());

    // more to do: logViewer, proxy, hotkey

    return {configuration};
}

BaseResult Configuration::toFile(const Configuration& configuration, QString configFile) {
    QFile file(configFile);
    QFileInfo fileinfo(file);
    QDir dir = fileinfo.absoluteDir();
    if (!dir.exists()) {
        dir.mkpath(dir.absolutePath());
    }
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        return BaseResult::fail(QObject::tr("%1 open fail").arg(configFile));
    }

    QJsonObject json;

    if (!configuration.getServerConfigs().isEmpty()) {
        QJsonArray array;
        for (const ServerConfig& serverConfig: configuration.getServerConfigs()) {
            array.append(serverConfig.toJson());
        }
        json.insert("configs", array);
    }

    json.insert("strategy", configuration.getStrategy());

    json.insert("index", configuration.getIndex());
    json.insert("global", configuration.isGlobal());
    json.insert("enabled", configuration.isEnabled());
    json.insert("shareOverLan", configuration.isShareOverLan());
    json.insert("isDefault", configuration.getIsDefault());
    json.insert("localPort", configuration.getLocalPort());
    json.insert("portableMode", configuration.isPortableMode());
    json.insert("pacUrl", configuration.getPacUrl());
    json.insert("useOnlinePac", configuration.isUseOnlinePac());
    json.insert("secureLocalPac", configuration.isSecureLocalPac());
    json.insert("availabilityStatistics", configuration.isAvailabilityStatistics());
    json.insert("autoCheckUpdate", configuration.isAutoCheckUpdate());
    json.insert("checkPreRelease", configuration.isCheckPreRelease());
    json.insert("isVerboseLogging", configuration.IsVerboseLogging());

    // more to do: logViewer, proxy, hotkey

    QJsonDocument doc;
    doc.setObject(json);
    file.write(doc.toJson());
    qDebug().noquote().nospace() << doc.toJson();
    file.flush();
    file.close();

    return BaseResult::success();
}

Configuration Configuration::Configuration::Load() {
    std::optional<Configuration> opt = fromFile(getConfigFile());
    if (!opt.has_value())
        return defaultConfiguration();

    Configuration configuration = opt.value();
    configuration.setIsDefault(false);

    if (configuration.getServerConfigs().isEmpty())
        configuration.addServerConfig(getDefaultServer());
    if (configuration.localPort == 0)
        configuration.localPort = 1080;
    if (configuration.index == -1 && configuration.strategy.isEmpty())
        configuration.index = 0;
    //        if (configuration.logViewer == null)
    //            configuration.logViewer = new LogViewerConfig();
    //        if (configuration.proxy == null)
    //            configuration.proxy = new ProxyConfig();
    //        if (configuration.hotkey == null)
    //            configuration.hotkey = new HotkeyConfig();

    //        config.proxy.CheckConfig();

    return configuration;
}

BaseResult Configuration::save(Configuration& configuration) {
    if (configuration.getIndex() >= configuration.getServerConfigs().size())
        configuration.setIndex(configuration.getServerConfigs().size() - 1);
    if (configuration.getIndex() < -1)
        configuration.setIndex(-1);
    if (configuration.getIndex() == -1 && configuration.getStrategy().isEmpty())
        configuration.setIndex(0);
    configuration.setIsDefault(false);

    return toFile(configuration, getConfigFile());
}

ServerConfig Configuration::getDefaultServer() {
    return ServerConfig();
}

BaseResult Configuration::assert(bool condition) {
    if (!condition)
        return BaseResult::fail(QObject::tr("assertion failure"));

    return BaseResult::success();
}

BaseResult Configuration::checkPort(int port) {
    if (port <= 0 || port > 65535)
        return BaseResult::fail(QObject::tr("Port out of range"));

    return BaseResult::success();
}

BaseResult Configuration::checkLocalPort(int port) {
    checkPort(port);
    if (port == 8123)
        return BaseResult::fail(QObject::tr("Port can't be 8123"));

    return BaseResult::success();
}

BaseResult Configuration::checkPassword(const QString& password) {
    if (password.isEmpty())
        return BaseResult::fail(QObject::tr("Password can not be blank"));

    if (password.toInt() == 0)
        return BaseResult::fail(QObject::tr("Illegal port number format"));

    return BaseResult::success();
}

BaseResult Configuration::checkServer(const QString& server) {
    if (server.isEmpty())
        return BaseResult::fail(QObject::tr("Server IP can not be blank"));

    QHostAddress address(server);
    if (address.isNull()) {
        // reference: https://stackoverflow.com/questions/10306690/what-is-a-regular-expression-which-will-match-a-valid-domain-name-without-a-subd/25717506#25717506
        static const QRegularExpression addressParser(R"(^(?=.{1,253}\.?$)(?:(?!-|[^.]+_)[A-Za-z0-9-_]{1,63}(?<!-)(?:\.|$)){2,}$)");
        QRegularExpressionMatch matchScheme = addressParser.match(server);
        if (!matchScheme.hasMatch())
            return BaseResult::fail(QObject::tr("Invalid server address"));
    }

    return BaseResult::success();
}

BaseResult Configuration::checkTimeout(int timeout, int maxTimeout) {
    if (timeout <= 0 || timeout > maxTimeout)
        return BaseResult::fail(QObject::tr("Timeout is invalid, it should not exceed %1").arg(maxTimeout));

    return BaseResult::success();
}

QList<ServerConfig> &Configuration::getServerConfigs() {
    return serverConfigs;
}

const QList<ServerConfig> &Configuration::getServerConfigs() const {
    return serverConfigs;
}

void Configuration::setServerConfigs(const QList<ServerConfig> &configs) {
    Configuration::serverConfigs = configs;
}

const QString &Configuration::getStrategy() const {
    return strategy;
}

void Configuration::setStrategy(const QString &strategy) {
    Configuration::strategy = strategy;
}

int Configuration::getIndex() const {
    return index;
}

void Configuration::setIndex(int index) {
    Configuration::index = index;
}

bool Configuration::isGlobal() const {
    return global;
}

void Configuration::setGlobal(bool global) {
    Configuration::global = global;
}

bool Configuration::isEnabled() const {
    return enabled;
}

void Configuration::setEnabled(bool enabled) {
    Configuration::enabled = enabled;
}

bool Configuration::isShareOverLan() const {
    return shareOverLan;
}

void Configuration::setShareOverLan(bool shareOverLan) {
    Configuration::shareOverLan = shareOverLan;
}

bool Configuration::getIsDefault() const {
    return isDefault;
}

void Configuration::setIsDefault(bool isDefault) {
    Configuration::isDefault = isDefault;
}

int Configuration::getLocalPort() const {
    return localPort;
}

void Configuration::setLocalPort(int localPort) {
    Configuration::localPort = localPort;
}

bool Configuration::isPortableMode() const {
    return portableMode;
}

void Configuration::setPortableMode(bool portableMode) {
    Configuration::portableMode = portableMode;
}

const QString &Configuration::getPacUrl() const {
    return pacUrl;
}

void Configuration::setPacUrl(const QString &pacUrl) {
    Configuration::pacUrl = pacUrl;
}

bool Configuration::isUseOnlinePac() const {
    return useOnlinePac;
}

void Configuration::setUseOnlinePac(bool useOnlinePac) {
    Configuration::useOnlinePac = useOnlinePac;
}

bool Configuration::isSecureLocalPac() const {
    return secureLocalPac;
}

void Configuration::setSecureLocalPac(bool secureLocalPac) {
    Configuration::secureLocalPac = secureLocalPac;
}

bool Configuration::isAvailabilityStatistics() const {
    return availabilityStatistics;
}

void Configuration::setAvailabilityStatistics(bool availabilityStatistics) {
    Configuration::availabilityStatistics = availabilityStatistics;
}

bool Configuration::isAutoCheckUpdate() const {
    return autoCheckUpdate;
}

void Configuration::setAutoCheckUpdate(bool autoCheckUpdate) {
    Configuration::autoCheckUpdate = autoCheckUpdate;
}

bool Configuration::isCheckPreRelease() const {
    return checkPreRelease;
}

void Configuration::setCheckPreRelease(bool checkPreRelease) {
    Configuration::checkPreRelease = checkPreRelease;
}

bool Configuration::IsVerboseLogging() const {
    return verboseLogging;
}

void Configuration::setVerboseLogging(bool verboseLogging) {
    Configuration::verboseLogging = verboseLogging;
}
