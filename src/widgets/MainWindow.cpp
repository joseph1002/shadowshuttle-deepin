#include "stdafx.h"
#include "common/utils.h"
#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "widgets/ConfigDialog.h"
#include "widgets/PACUrlDialog.h"
#include "widgets/ProxyDialog.h"
#include "widgets/LogMainWindow.h"
#include "Toolbar.h"
#include "DDEProxyModeManager.h"
#include "QRCodeCapturer.h"
#include "SSValidator.h"
#include "ShareDialog.h"
#include "widgets/SystemTrayManager.h"
#include <DDesktopServices>

MainWindow::MainWindow(QWidget *parent) :
        QMainWindow(parent),
        ui(new Ui::MainWindow),
        configDialog(nullptr), pacUrlDialog(nullptr), proxyDialog(nullptr) {
    ui->setupUi(this);
    installEventFilter(this);   // add event filter

    initSystemTrayIcon();

    proxyManager = new ProxyManager(this);

    systemProxyModeManager = new DDEProxyModeManager(this);
    const Configuration& configuration = ShadowsocksController::Instance().getCurrentConfiguration();
    if (configuration.isEnabled()) {
        on_actionEnable_System_Proxy_triggered(true);
        if (configuration.isGlobal()) {
            switchToGlobal(configuration);
        } else {
            switchToPacMode(configuration);
        }
    }

    in = 0;
    out = 0;
    ins.clear();
    outs.clear();

    auto timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &MainWindow::updateTrayIcon);
    timer->start(150);

    connect(proxyManager, &ProxyManager::newBytesReceived, [=](quint64 n) {
        qDebug() << "newBytesReceived" << n;
        in += n;
    });
    connect(proxyManager, &ProxyManager::newBytesSent, [=](quint64 n) {
        qDebug() << "newBytesSent" << n;
        out += n;
    });
    updateTrayIcon();
    updateMenu();
}

MainWindow::~MainWindow() {
    proxyManager->stop();
    systemProxyModeManager->switchToNone();
    delete ui;
}

void MainWindow::initSystemTrayIcon() {
    systemTrayManager = new SystemTrayManager(this, ui->menuTray);
    connect(systemTrayManager, &SystemTrayManager::trayIconDoubleClicked, this,
            &MainWindow::on_actionEdit_Servers_triggered);
}

void MainWindow::switchToPacMode(const Configuration& configuration) {
    QString online_pac_uri = "http://file.lolimay.cn/autoproxy.pac";
    QString pacURI = "";
    if (configuration.isUseOnlinePac()) {
        pacURI = configuration.getPacUrl();
        if (pacURI.isEmpty()) {
            qDebug() << "\033[30mWARNING: online pac uri is empty. we will use default uri.";
            pacURI = online_pac_uri;
            // to do
//            guiConfig->set("pacUrl", pacURI);
        }
    } else {
        QString pac_file = QDir(Utils::configPath()).filePath("autoproxy.pac");
        QFile file(pac_file);
        if (!file.exists()) {
            Utils::warning("local pac does not exist. we will use on pac file. you can change it");
            pacURI = online_pac_uri;
            // todo
//            guiConfig->set("pacUrl", pacURI);
//            guiConfig->set("useOnlinePac", true);
        } else {
            pacURI = "file://" + pac_file;
        }
    }
    systemProxyModeManager->switchToAuto(pacURI);
}

void MainWindow::switchToGlobal(const Configuration& configuration) {
    int local_port = configuration.getLocalPort();
    systemProxyModeManager->switchToManual("127.0.0.1", local_port);
}

void MainWindow::contextMenuEvent(QContextMenuEvent *) {
    qDebug() << "right click";
}

void MainWindow::updateTrayIcon() {
    ins.append(in);
    outs.append(out);

    systemTrayManager->updateTrayIcon(in > 0, out > 0);
    in = 0;
    out = 0;
}

template<typename Type1, typename Type2, typename Type3>
void showDialog(Type1 *, Type2 *&dialog, Type3 *parent) {
    if (dialog == nullptr) {
        dialog = new Type1(parent);
    }
    if (dialog->isHidden()) {
        dialog->show();
    }
    dialog->raise();
    dialog->activateWindow();
}

void MainWindow::on_actionEdit_Servers_triggered() {
    showDialog<ConfigDialog>(nullptr, configDialog, this);
    updateMenu();
}

void MainWindow::on_actionEdit_Online_PAC_URL_triggered() {
    showDialog<PACUrlDialog>(nullptr, pacUrlDialog, this);
}

void MainWindow::on_actionForward_Proxy_triggered() {
    showDialog<ProxyDialog>(nullptr, proxyDialog, this);
}

void MainWindow::on_actionShow_Logs_triggered() {
    LogMainWindow *w = new LogMainWindow(this);
    w->show();
}

void MainWindow::updateMenu() {
    const Configuration& configuration = ShadowsocksController::Instance().getCurrentConfiguration();
    if (configuration.isEnabled()) {
        ui->actionEnable_System_Proxy->setChecked(true);
        ui->menuMode->setEnabled(true);
    } else {
        ui->actionEnable_System_Proxy->setChecked(false);
        ui->menuMode->setEnabled(false);
    }
    if (configuration.isGlobal()) {
        ui->actionGlobal->setChecked(true);
        ui->actionPAC->setChecked(false);
    } else {
        ui->actionGlobal->setChecked(false);
        ui->actionPAC->setChecked(true);
    }
    if (configuration.isUseOnlinePac()) {
        ui->actionOnline_PAC->setChecked(true);
        ui->actionLocal_PAC->setChecked(false);
    } else {
        ui->actionOnline_PAC->setChecked(false);
        ui->actionLocal_PAC->setChecked(true);
    }
    if (configuration.isSecureLocalPac()) {
        ui->actionSecure_Local_PAC->setChecked(true);
    } else {
        ui->actionSecure_Local_PAC->setChecked(false);
    }
    if (configuration.isShareOverLan()) {
        ui->actionAllow_Clients_from_LAN->setChecked(true);
    } else {
        ui->actionAllow_Clients_from_LAN->setChecked(false);
    }
    if (configuration.IsVerboseLogging()) {
        ui->actionVerbose_Logging->setChecked(true);
    } else {
        ui->actionVerbose_Logging->setChecked(false);
    }
    if (configuration.isAutoCheckUpdate()) {
        ui->actionCheck_for_Updates_at_Startup->setChecked(true);
    } else {
        ui->actionCheck_for_Updates_at_Startup->setChecked(false);
    }
    if (configuration.isCheckPreRelease()) {
        ui->actionCheck_Pre_release_Version->setChecked(true);
    } else {
        ui->actionCheck_Pre_release_Version->setChecked(false);
    }
    if (isAutoStart()) {
         ui->actionStart_on_Boot->setChecked(true);
    } else {
         ui->actionStart_on_Boot->setChecked(false);
    }
    ui->menuServers->clear();
    QList<QAction *> action_list;
    action_list << ui->actionLoad_Balance << ui->actionHigh_Availability << ui->actionChoose_by_statistics;
    ui->menuServers->addActions(action_list);
    ui->menuServers->addSeparator();
    action_list.clear();
    for (int i = 0; i < configuration.getServerConfigs().size(); ++i) {
        const ServerConfig& serverConfig = configuration.getServerConfigs()[i];
        QString name = serverConfig.friendlyName();
        auto action = ui->menuServers->addAction(name, [=]() {
            ShadowsocksController::Instance().selectServerIndex(i);
            on_actionEnable_System_Proxy_triggered(true);
        });
        action->setCheckable(true);
        if (configuration.isEnabled() && configuration.getIndex() == i) {
            action->setChecked(true);
        }
    }
    ui->menuServers->addSeparator();
    action_list << ui->actionEdit_Servers << ui->actionStatistics_Config << ui->actionImport_from_gui_config_json
                << ui->actionExport_as_gui_config_json;
    ui->menuServers->addActions(action_list);
    ui->menuServers->addSeparator();
    ui->menuServers->addSeparator();
    action_list << ui->actionShare_Server_Config << ui->actionScan_QRCode_from_Screen
                << ui->actionImport_URL_from_Clipboard;
    ui->menuServers->addActions(action_list);
    ui->menuServers->addSeparator();

    ui->menuHelp->menuAction()->setVisible(false);
    ui->menuPAC->menuAction()->setVisible(false);
}

void MainWindow::on_actionEnable_System_Proxy_triggered(bool flag) {
    const Configuration& configuration = ShadowsocksController::Instance().getCurrentConfiguration();
    if (!flag) {
        proxyManager->stop();
        // 又混着了
        systemProxyModeManager->switchToNone();
    } else {
        auto configs = configuration.getServerConfigs();
        auto index = configuration.getIndex();
        if (configs.size() < index + 1) {
            //TODO: choose a server to start
            Utils::warning("choose server to start");
        } else {
            auto config = configs[index];
            proxyManager->setConfig(config, configuration.getLocalPort());
            proxyManager->start();
            if (configuration.isGlobal()) {
                switchToGlobal(configuration);
            } else {
                switchToPacMode(configuration);
            }
        }
    }
//    guiConfig->set("enabled", flag);

    updateMenu();
}

void MainWindow::on_actionPAC_triggered(bool checked) {
    qDebug() << "pac" << checked;
    const Configuration& configuration = ShadowsocksController::Instance().getCurrentConfiguration();
    // todo
//    ShadowsocksController::Instance().t(checked);

//    if (guiConfig->get("global").toBool() == checked) {
//        guiConfig->set("global", !checked);
//        switchToPacMode();
//    }
    updateMenu();
}

void MainWindow::on_actionGlobal_triggered(bool checked) {
    qDebug() << "global" << checked;
    const Configuration& configuration = ShadowsocksController::Instance().getCurrentConfiguration();
    ShadowsocksController::Instance().toggleGlobal(checked);
    if (checked) {
        switchToGlobal(configuration);
    }
    updateMenu();
}

bool MainWindow::isAutoStart() {
    QString url = "/usr/share/applications/shadowshuttle-deepin.desktop";
    QDBusPendingReply<bool> reply = startManagerInter.IsAutostart(url);
    reply.waitForFinished();
    if (reply.isError()) {
        qCritical() << reply.error().name() << reply.error().message();
        Utils::critical("query auto start error");
        return false;
    } else {
        qDebug() << "auto start flag:" << reply.argumentAt(0).toBool();
        return reply.argumentAt(0).toBool();
    }
}

void MainWindow::on_actionStart_on_Boot_triggered(bool checked) {
    // 如果使用flatpak，这里可能有问题
    QString url = "/usr/share/applications/shadowshuttle-deepin.desktop";
    const bool autoStartFlag = isAutoStart();
    if (autoStartFlag == checked)
        return;

    if (!checked) {
        QDBusPendingReply<bool> reply = startManagerInter.RemoveAutostart(url);
        reply.waitForFinished();
        if (!reply.isError()) {
            bool ret = reply.argumentAt(0).toBool();
            qDebug() << "remove from startup:" << ret;
        } else {
            qCritical() << reply.error().name() << reply.error().message();
            Utils::critical("change auto boot error");
        }
    } else {
        QDBusPendingReply<bool> reply = startManagerInter.AddAutostart(url);
        reply.waitForFinished();
        if (!reply.isError()) {
            bool ret = reply.argumentAt(0).toBool();
            qDebug() << "add to startup:" << ret;
        } else {
            qCritical() << reply.error().name() << reply.error().message();
            Utils::critical("change auto boot error");
        }
    }
}

void MainWindow::on_actionQuit_triggered() {
    qApp->exit();
}

bool MainWindow::eventFilter(QObject *, QEvent *event) {
    //    qDebug()<<event->type();
    if (event->type() == QEvent::WindowStateChange) {
        //        adjustStatusBarWidth();
    } else if (event->type() == QEvent::KeyPress) {
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
        if (keyEvent->key() == Qt::Key_F) {
            if (keyEvent->modifiers() == Qt::ControlModifier) {
                toolbar->focusInput();
            }
        }
    } else if (event->type() == QEvent::Close) {

    }

    return false;
}

void MainWindow::on_actionDisconnect_triggered() {
    on_actionEnable_System_Proxy_triggered(false);
}

void MainWindow::on_actionScan_QRCode_from_Screen_triggered() {
    QString uri = QRCodeCapturer::scanEntireScreen();
    if (uri.isNull()) {
        QMessageBox::critical(
                nullptr,
                tr("QR Code Not Found"),
                tr("Can't find any QR code image that contains valid URI on your screen(s)."));
    } else {
        qDebug() << "scan uri" << uri;
        Utils::info(tr("found URI %1").arg(uri));
        BaseResult baseResult = ShadowsocksController::Instance().addServerBySSURL(uri);
        if (baseResult.isOk()) {
            updateMenu();
            on_actionEdit_Servers_triggered();
        } else {
            Utils::info(tr("URI is invalid"));
        }
//        if (uri.startsWith("ss://")) {
//            qDebug() << "shadowsocks";
//            if (SSValidator::validate(uri)) {
//                Utils::info(tr("URI is valid"));
//                GuiConfig::instance()->addConfig(uri);

//                updateMenu();
//                on_actionEdit_Servers_triggered();
//            } else {
//                Utils::info(tr("URI is invalid"));
//            }
//        }
    }
}

void MainWindow::on_actionImport_URL_from_Clipboard_triggered() {
    QString uri = QApplication::clipboard()->text();
    BaseResult baseResult = ShadowsocksController::Instance().addServerBySSURL(uri);
    if (baseResult.isOk()) {
        updateMenu();
        on_actionEdit_Servers_triggered();
    } else {
        Utils::info(tr("URI is invalid"));
    }
}

void MainWindow::on_actionShare_Server_Config_triggered() {
    ShareDialog *d = new ShareDialog();
    d->setAttribute(Qt::WA_DeleteOnClose);
    d->exec();
}

void MainWindow::on_actionImport_from_gui_config_json_triggered() {
    QString fileName = QFileDialog::getOpenFileName(this, tr("choose gui-config.json file"), QDir::homePath(),
                                                    "gui-config.json");
    if (fileName.isEmpty()) {
        return;
    }
    ShadowsocksController::Instance().importFrom(fileName);
    updateMenu();
}

void MainWindow::on_actionExport_as_gui_config_json_triggered() {
    QString fileName = QFileDialog::getExistingDirectory(nullptr, tr("Save gui-config.json"),
                                                         QStandardPaths::standardLocations(
                                                                 QStandardPaths::DocumentsLocation).first());
    if (fileName.isEmpty()) {
        return;
    }
    fileName = fileName + "/gui-config.json";
    ShadowsocksController::Instance().exportAs(fileName);
    DDesktopServices::showFileItem(fileName);
}
