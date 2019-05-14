#include "ServerConfig.h"
#include "common/utils.h"


const int ServerConfig::DefaultServerTimeoutSec = 5;
const int ServerConfig::MaxServerTimeoutSec = 20;

const QRegularExpression ServerConfig::UrlFinder(
        R"(ss://(?<base64>[A-Za-z0-9+-/=_]+)(?:#(?<tag>\S+))?)",
        QRegularExpression::CaseInsensitiveOption);

const QRegularExpression ServerConfig::DetailsParser(
        R"(^((?<method>.+?):(?<password>.*)@(?<hostname>.+?):(?<port>\d+?))$)",
        QRegularExpression::CaseInsensitiveOption);

ServerConfig::ServerConfig() {
    server = "";
    serverPort = 8388;
    method = "aes-256-cfb";
    plugin = "";
    pluginOpts = "";
    pluginArgs = "";
    password = "";
    remarks = "";
    timeout = DefaultServerTimeoutSec;
}

uint ServerConfig::getHashCode() {
    return qHash(server) ^ static_cast<uint>(serverPort);
}

bool ServerConfig::equals(const ServerConfig& obj) {
    return server == obj.server && serverPort == obj.serverPort;
}

QString ServerConfig::friendlyName() const {
    if (server.isEmpty()) {
        return QObject::tr("New server");
    }

    QString serverStr = server + ":" + QString::number(serverPort);

    return remarks.isEmpty() ? serverStr : remarks + " (" + serverStr + ")";
}

QString ServerConfig::formatHostName(QString hostName) {
    QHostAddress addr(hostName);

    switch (addr.protocol()) {
    case QAbstractSocket::IPv6Protocol:  // Add square bracket when IPv6 (RFC3986)
        return QString("[%1]").arg(hostName);
    default:    // IPv4 or domain name
        return hostName;
    }
}

std::optional<ServerConfig> ServerConfig::parseLegacyURL(QString ssURL) {
    QRegularExpressionMatch matchScheme = UrlFinder.match(ssURL);
    if (!matchScheme.hasMatch())
        return std::nullopt;

    //    Shadowsocks for Android / iOS also accepts BASE64 encoded URI format configs:
    //        ss://BASE64-ENCODED-STRING-WITHOUT-PADDING#TAG
    //    Where the plain URI should be:
    //        ss://method:password@hostname:port

    QString base64 = matchScheme.captured("base64");
    if (base64.endsWith("/")) {
        base64.chop(1);
    }
    QString details = QString::fromUtf8(QByteArray::fromBase64(base64.toUtf8()));

    QString tag = matchScheme.captured("tag");
    qDebug() << QString("base64 raw:%1; decoded:%2, tag:%3").arg(base64, details, tag);

    QRegularExpressionMatch matchServer = DetailsParser.match(details);
    if (matchServer.hasMatch())
        return std::nullopt;

    ServerConfig server;
    if (!tag.isEmpty()) {
        server.setRemarks(tag);
    }

    QString method = matchServer.captured("method");
    QString password = matchServer.captured("password");
    QString hostname = matchServer.captured("hostname");
    QString port = matchServer.captured("port");

    qDebug() << QString("method:%1; password:%2, hostname:%3, port:%4")
                .arg(method, password, hostname, port);
    server.setMethod(method);
    server.setPassword(password);
    server.setServer(hostname);
    server.setServerPort(port.toInt());

    return {server};
}

QList<ServerConfig> ServerConfig::getServers(QString ssURL) {
    QStringList serverUrls = ssURL.split(QRegularExpression(R"((\r|\n| ))"), QString::SkipEmptyParts);

    QList<ServerConfig> servers;
    foreach(QString serverUrl, serverUrls)
    {
        QString _serverUrl = serverUrl.trimmed();
        if (!_serverUrl.startsWith("ss://")) {
            continue;
        }

        std::optional<ServerConfig> legacyServer = parseLegacyURL(serverUrl);
        if (legacyServer.has_value())   //legacy
        {
            servers << legacyServer.value();
        } else   //SIP002
        {
            //            SIP002 purposed a new URI scheme, following RFC3986:
            //            SS-URI = "ss://" userinfo "@" hostname ":" port [ "/" ] [ "?" plugin ] [ "#" tag ]
            //            userinfo = websafe-base64-encode-utf8(method  ":" password)
            QUrl parsedUrl(serverUrl);
            if (!parsedUrl.isValid()) {
                qDebug() << "not valid url";
                continue;
            }
            ServerConfig server;
            if (parsedUrl.hasFragment()) {
                server.setRemarks(parsedUrl.fragment());
            }
            server.setServer(parsedUrl.host());
            server.setServerPort(parsedUrl.port());

            // parse base64 UserInfo
            QString rawUserInfo = parsedUrl.userInfo();
            QString base64 = rawUserInfo.replace('-', '+').replace('_', '/');    // Web-safe base64 to normal base64
            QString userInfo = QString::fromUtf8(QByteArray::fromBase64(base64.toUtf8()));

            QStringList userInfoParts = userInfo.split(":");
            if (userInfoParts.size() != 2) {
                continue;
            }
            server.setMethod(userInfoParts[0]);
            server.setPassword(userInfoParts[1]);

            if (parsedUrl.hasQuery()) {
                QUrlQuery query(parsedUrl);
                if (query.hasQueryItem("plugin")) {
                    QString plugin = query.queryItemValue("plugin", QUrl::FullyDecoded);
                    QStringList pluginParts = plugin.split(";");
                    if (pluginParts.size() > 0) {
                        server.setPlugin(pluginParts[0]);
                    }

                    if (pluginParts.size() > 1) {
                        server.setPluginOpts(pluginParts[1]);
                    }
                }
            }
            servers << server;
        }
    }
    return servers;
}

std::optional<ServerConfig> ServerConfig::fromJson(const QJsonObject &json) {
    /**
    {
      "server": "69.194.8.148",
      "server_port": 10010,
      "password": "10010",
      "method": "rc4-md5",
      "plugin": "",
      "plugin_opts": "",
      "plugin_args": "",
      "remarks": "my",
      "timeout": 5
    }
*/
    if (json.empty())
        return std::nullopt;

    ServerConfig server;
    if (json.contains("server"))
        server.setServer(json.value("server").toString());

    if (json.contains("server_port"))
        server.setServerPort(json.value("server_port").toInt());

    if (json.contains("password"))
        server.setPassword(json.value("password").toString());

    if (json.contains("method"))
        server.setMethod(json.value("method").toString());

    if (json.contains("plugin"))
        server.setPlugin(json.value("plugin").toString());

    if (json.contains("plugin_opts"))
        server.setPluginOpts(json.value("plugin_opts").toString());

    if (json.contains("plugin_args"))
        server.setPluginArgs(json.value("plugin_args").toString());

    if (json.contains("remarks"))
        server.setRemarks(json.value("remarks").toString());

    if (json.contains("timeout"))
        server.setTimeout(json.value("timeout").toInt());

    return {server};
}

QJsonObject ServerConfig::toJson() const {
    QJsonObject json;
    json.insert("server", getServer());
    json.insert("server_port", getServerPort());
    json.insert("password", getPassword());
    json.insert("method", getMethod());
    json.insert("plugin", getPlugin());
    json.insert("plugin_opts", getPluginOpts());
    json.insert("plugin_args", getPluginArgs());
    json.insert("remarks", getRemarks());
    json.insert("timeout", getTimeout());

    return json;
}

QString ServerConfig::identifier() {
    return server + ':' + QString::number(serverPort);
}

const QString &ServerConfig::getServer() const {
    return server;
}

void ServerConfig::setServer(const QString &server) {
    ServerConfig::server = server;
}

int ServerConfig::getServerPort() const {
    return serverPort;
}

void ServerConfig::setServerPort(int serverPort) {
    ServerConfig::serverPort = serverPort;
}

const QString &ServerConfig::getPassword() const {
    return password;
}

void ServerConfig::setPassword(const QString &password) {
    ServerConfig::password = password;
}

const QString &ServerConfig::getMethod() const {
    return method;
}

void ServerConfig::setMethod(const QString &method) {
    ServerConfig::method = method;
}

const QString &ServerConfig::getPlugin() const {
    return plugin;
}

void ServerConfig::setPlugin(const QString &plugin) {
    ServerConfig::plugin = plugin;
}

const QString &ServerConfig::getPluginOpts() const {
    return pluginOpts;
}

void ServerConfig::setPluginOpts(const QString &pluginOpts) {
    ServerConfig::pluginOpts = pluginOpts;
}

const QString &ServerConfig::getPluginArgs() const {
    return pluginArgs;
}

void ServerConfig::setPluginArgs(const QString &pluginArgs) {
    ServerConfig::pluginArgs = pluginArgs;
}

const QString &ServerConfig::getRemarks() const {
    return remarks;
}

void ServerConfig::setRemarks(const QString &remarks) {
    ServerConfig::remarks = remarks;
}

int ServerConfig::getTimeout() const {
    return timeout;
}

void ServerConfig::setTimeout(int timeout) {
    ServerConfig::timeout = timeout;
}






