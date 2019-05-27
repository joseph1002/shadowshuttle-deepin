#include <DDesktopServices>
#include "common/utils.h"
#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "widgets/ConfigDialog.h"
#include "widgets/PACUrlDialog.h"
#include "widgets/ProxyDialog.h"
#include "widgets/LogMainWindow.h"
#include "Toolbar.h"
#include "QRCodeCapturer.h"
#include "SSValidator.h"
#include "ShareDialog.h"
#include "widgets/SystemTrayManager.h"

MainWindow::MainWindow(QWidget *parent) :
        QMainWindow(parent),
        ui(new Ui::MainWindow) {
    ui->setupUi(this);

    configDialog = new ConfigDialog(this);
    connect(configDialog, &ConfigDialog::configChanged, this, &MainWindow::serverConfigChanged);

    pacUrlDialog = new PACUrlDialog(this);
    proxyDialog = new ProxyDialog(this);

    initSystemTrayIcon();
    initProxy();
    LoadContextMenu();
}

MainWindow::~MainWindow() {
    proxyManager->stopSocksService();
    proxyManager->systemProxyToNone();
    delete ui;
}

void MainWindow::initProxy() {
    proxyManager = new ProxyManager(this);

    in = 0;
    out = 0;
    auto timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &MainWindow::updateTrayIcon);
    timer->start(300);

    connect(proxyManager, &ProxyManager::newBytesReceived, [=](quint64 n) {
        qDebug() << "newBytesReceived" << n;
        in += n;
    });
    connect(proxyManager, &ProxyManager::newBytesSent, [=](quint64 n) {
        qDebug() << "newBytesSent" << n;
        out += n;
    });
}

void MainWindow::initSystemTrayIcon() {
    systemTrayManager = new SystemTrayManager(this, ui->menuTray);
    connect(systemTrayManager, &SystemTrayManager::trayIconDoubleClicked, this,
            &MainWindow::on_actionEdit_Servers_triggered);
}

void MainWindow::LoadContextMenu() {
    menuServerGroup = new QActionGroup(this);
    menuServerGroup->setExclusive(true);

    menuPacGroup = new QActionGroup(this);
    menuPacGroup->setExclusive(true);
    menuPacGroup->addAction(ui->actionGlobal);
    menuPacGroup->addAction(ui->actionPAC);

    loadMenuServers();

    ShadowsocksController& controller = ShadowsocksController::Instance();
    const Configuration& configuration = controller.getConfiguration();
    // don't check the acton if there is no server available
    if (!controller.getCurrentServer().getServer().isEmpty()) {
    //    QList<QAction*> actions = menuServerGroup->actions();
    //    QList<QAction*> actions2 = ui->menuServers->actions();
    //    qDebug() << actions.size() << "    " << actions2.size();
        menuServerGroup->actions().at(3 + configuration.getIndex())->activate(QAction::Trigger);
    }

    if (configuration.isEnabled()) {
        ui->menuMode->setEnabled(true);
        ui->actionEnable_System_Proxy->activate(QAction::Trigger);

        if (configuration.isGlobal()) {
            ui->actionGlobal->activate(QAction::Trigger);
        } else {
            ui->actionPAC->activate(QAction::Trigger);
        }
    } else {
        ui->menuMode->setEnabled(false);
        ui->actionEnable_System_Proxy->setChecked(false);
    }

    // todo
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
    if (Utils::isAutoStart()) {
         ui->actionStart_on_Boot->setChecked(true);
    } else {
         ui->actionStart_on_Boot->setChecked(false);
    }

    // set invisible because they are not implemented
    ui->menuHelp->menuAction()->setVisible(false);
    ui->menuPAC->menuAction()->setVisible(false);
    ui->actionLoad_Balance->setVisible(false);
    ui->actionHigh_Availability->setVisible(false);
    ui->actionChoose_by_statistics->setVisible(false);
}

void MainWindow::loadMenuServers() {
    QList<QAction *> actionList = menuServerGroup->actions();
    if (!actionList.empty()) {
        for (auto action : actionList) {
            menuServerGroup->removeAction(action);
            ui->menuServers->removeAction(action);
            if (action != ui->actionLoad_Balance
                    && action != ui->actionHigh_Availability
                    && action != ui->actionChoose_by_statistics) {
                action->deleteLater();
            }
        }
    }

    menuServerGroup->addAction(ui->actionLoad_Balance);
    menuServerGroup->addAction(ui->actionHigh_Availability);
    menuServerGroup->addAction(ui->actionChoose_by_statistics);

    ui->menuServers->clear();
    // draw menu server
    ui->menuServers->addAction(ui->actionLoad_Balance);
    ui->menuServers->addAction(ui->actionHigh_Availability);
    ui->menuServers->addAction(ui->actionChoose_by_statistics);

    ui->menuServers->addSeparator();

    // create dynamic server actions
    const Configuration& configuration = ShadowsocksController::Instance().getConfiguration();
    for (int i = 0; i < configuration.getServerConfigs().size(); ++i) {
        const ServerConfig& serverConfig = configuration.getServerConfigs()[i];
        QString name = serverConfig.friendlyName();
        QAction* action = ui->menuServers->addAction(name, [=]() {
            ShadowsocksController::Instance().selectServerIndex(i);
            proxyManager->launchSocksService(serverConfig, configuration.getLocalPort());
        });
        action->setCheckable(true);
        action->setActionGroup(menuServerGroup);
//        menuServerGroup->addAction(action);
    }

    ui->menuServers->addSeparator();

    ui->menuServers->addAction(ui->actionEdit_Servers);
    ui->menuServers->addAction(ui->actionStatistics_Config);

    ui->menuServers->addSeparator();

    ui->menuServers->addAction(ui->actionImport_from_gui_config_json);
    ui->menuServers->addAction(ui->actionExport_as_gui_config_json);

    ui->menuServers->addSeparator();

    ui->menuServers->addAction(ui->actionShare_Server_Config);
    ui->menuServers->addAction(ui->actionScan_QRCode_from_Screen);
    ui->menuServers->addAction(ui->actionImport_URL_from_Clipboard);

}

void MainWindow::updateTrayIcon() {
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

void MainWindow::on_actionEnable_System_Proxy_triggered(bool checked) {
    ShadowsocksController& controller = ShadowsocksController::Instance();
    const Configuration& configuration = controller.getConfiguration();
    ui->menuMode->setEnabled(checked);
    if (!checked) {
        proxyManager->systemProxyToNone();
    } else {
        if (configuration.isGlobal()) {
            ui->actionGlobal->activate(QAction::Trigger);
        } else {
            ui->actionPAC->activate(QAction::Trigger);
        }
    }
    controller.toggleEnable(checked);
}

void MainWindow::on_actionPAC_triggered(bool checked) {
    qDebug() << "pac" << checked;
    ShadowsocksController& controller = ShadowsocksController::Instance();
    const Configuration& configuration = controller.getConfiguration();

    if (!checked) {
        proxyManager->stopSocksService();
        proxyManager->systemProxyToNone();
    } else {
        auto configs = configuration.getServerConfigs();
        auto index = configuration.getIndex();
        if (configs.size() < index + 1) {
            //TODO: choose a server to start
            Utils::warning("choose server to start");
        } else {
            auto config = configs[index];
            proxyManager->launchSocksService(config, configuration.getLocalPort());
            proxyManager->systemProxyToAuto(controller.getPACUrlForCurrentServer());
        }
    }

    controller.toggleGlobal(false);
}

void MainWindow::on_actionGlobal_triggered(bool checked) {
    qDebug() << "global" << checked;
    ShadowsocksController& controller = ShadowsocksController::Instance();
    const Configuration& configuration = controller.getConfiguration();
    if (checked) {
        proxyManager->systemProxyToManual("127.0.0.1", configuration.getLocalPort());
    }
    controller.toggleGlobal(checked);
}

void MainWindow::on_actionStart_on_Boot_triggered(bool checked) {
    const bool autoStartFlag = Utils::isAutoStart();
    if (autoStartFlag == checked)
        return;

    if (!checked) {
        Utils::removeAutoStart();
    } else {
        Utils::autoStart();
    }
}

void MainWindow::on_actionQuit_triggered() {
    qApp->exit();
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
//            updateMenu();
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
//        updateMenu();
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
//    updateMenu();
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

void MainWindow::serverConfigChanged() {
    loadMenuServers();
    const Configuration& configuration = ShadowsocksController::Instance().getConfiguration();
    menuServerGroup->actions().at(3 + configuration.getIndex())->activate(QAction::Trigger);
}

bool MainWindow::event(QEvent *event) {
    int res = QWidget::event(event);

    if (event->type() == QEvent::Polish) {
        // show config dialog if there is no server
        ShadowsocksController& controller = ShadowsocksController::Instance();
        const Configuration& configuration = controller.getConfiguration();
        // don't check the acton if there is no server available
        if (controller.getCurrentServer().getServer().isEmpty()) {
            showDialog<ConfigDialog>(nullptr, configDialog, this);
        }
    }

    return res;
}
