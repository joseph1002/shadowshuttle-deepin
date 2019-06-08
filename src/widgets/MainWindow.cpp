#include "common/utils.h"
#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "widgets/ConfigDialog.h"
#include "widgets/PACUrlDialog.h"
#include "widgets/ProxyDialog.h"
#include "widgets/LogMainWindow.h"
#include "QRCodeCapturer.h"
#include "ShareDialog.h"
#include "widgets/SystemTrayManager.h"
#include "proxy/Socks5Proxy.h"

MainWindow::MainWindow(QWidget *parent) :
        QMainWindow(parent),
        ui(new Ui::MainWindow) {
    ui->setupUi(this);
    controller = &ShadowsocksController::Instance();

    configDialog = new ConfigDialog(this);
    connect(configDialog, &ConfigDialog::configChanged, this, &MainWindow::serverConfigChanged);

    pacUrlDialog = new PACUrlDialog(this);
    connect(pacUrlDialog, &PACUrlDialog::pacUrlChanged, this, &MainWindow::pacUrlChanged);

    proxyDialog = new ProxyDialog(this);

    initSystemTrayIcon();
    initProxy();
    // this method should be called only once
    loadContextMenu();
}

MainWindow::~MainWindow() {
    socks5Proxy->stop();
    controller->closeSystemProxy();
    delete ui;
}

void MainWindow::initProxy() {
    socks5Proxy = new Socks5Proxy(this);

    in = 0;
    out = 0;
    auto timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &MainWindow::updateTrayIcon);
    timer->start(300);

    connect(socks5Proxy, &Socks5Proxy::newBytesReceived, [=](quint64 n) {
        qDebug() << "newBytesReceived" << n;
        in += n;
    });
    connect(socks5Proxy, &Socks5Proxy::newBytesSent, [=](quint64 n) {
        qDebug() << "newBytesSent" << n;
        out += n;
    });
}

void MainWindow::initSystemTrayIcon() {
    systemTrayManager = new SystemTrayManager(this, ui->menuTray);
    connect(systemTrayManager, &SystemTrayManager::trayIconDoubleClicked, this,
            &MainWindow::on_actionEdit_Servers_triggered);
}

void MainWindow::loadContextMenu() {
    menuProxyGroup = new QActionGroup(this);
    menuProxyGroup->setExclusive(true);
    menuProxyGroup->addAction(ui->actionDisable);
    menuProxyGroup->addAction(ui->actionGlobal);
    menuProxyGroup->addAction(ui->actionPAC);

    menuPacGroup = new QActionGroup(this);
    menuPacGroup->setExclusive(true);
    menuPacGroup->addAction(ui->actionLocal_PAC);
    menuPacGroup->addAction(ui->actionOnline_PAC);

    menuLocalPacGroup = new QActionGroup(this);
    menuLocalPacGroup->addAction(ui->actionEdit_Local_PAC_File);
    menuLocalPacGroup->addAction(ui->actionUpdate_Local_PAC_from_GFWList);
    menuLocalPacGroup->addAction(ui->action_Edit_User_Rule_for_GFWList);

    menuServerGroup = new QActionGroup(this);
    menuServerGroup->setExclusive(true);
    loadMenuServers();

    const Configuration &configuration = controller->getConfiguration();
    // launch socks service
    // don't check the acton if there is no server available
    if (!controller->getCurrentServer().getServer().isEmpty()) {
        //    QList<QAction*> actions = menuServerGroup->actions();
        //    QList<QAction*> actions2 = ui->menuServers->actions();
        //    qDebug() << actions.size() << "    " << actions2.size();
        menuServerGroup->actions().at(3 + configuration.getIndex())->activate(QAction::Trigger);
    }

    // set system proxy
    if (configuration.isEnabled()) {
        if (configuration.isGlobal()) {
            ui->actionGlobal->activate(QAction::Trigger);
        } else {
            ui->actionPAC->activate(QAction::Trigger);
        }
    } else {
        ui->actionDisable->setChecked(true);
    }

    if (configuration.isUseOnlinePac()) {
        ui->actionOnline_PAC->setChecked(true);
        menuLocalPacGroup->setEnabled(false);
    } else {
        ui->actionLocal_PAC->setChecked(true);
        ui->actionEdit_Online_PAC_URL->setEnabled(false);
    }

    // todo
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
    if (controller->isAutoStart()) {
        ui->actionStart_on_Boot->setChecked(true);
    } else {
        ui->actionStart_on_Boot->setChecked(false);
    }

    // set invisible because they are not implemented
    ui->menuHelp->menuAction()->setVisible(false);
    ui->actionLoad_Balance->setVisible(false);
    ui->actionHigh_Availability->setVisible(false);
    ui->actionChoose_by_statistics->setVisible(false);
    ui->actionSecure_Local_PAC->setVisible(false);
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
    const Configuration &configuration = controller->getConfiguration();
    for (int i = 0; i < configuration.getServerConfigs().size(); ++i) {
        const ServerConfig &serverConfig = configuration.getServerConfigs()[i];
        QString name = serverConfig.friendlyName();
        QAction *action = ui->menuServers->addAction(name, [=]() {
            controller->selectServerIndex(i);
            socks5Proxy->start(serverConfig, configuration.getLocalPort());
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

template<typename Type1>
void showDialog(Type1 *&dialog) {
    if (dialog->isHidden()) {
        dialog->show();
    }
    dialog->raise();
    dialog->activateWindow();
}

void MainWindow::on_actionEdit_Servers_triggered() {
    showDialog<ConfigDialog>(configDialog);
}

void MainWindow::on_actionEdit_Online_PAC_URL_triggered() {
    showDialog<PACUrlDialog>(pacUrlDialog);
}

void MainWindow::on_actionForward_Proxy_triggered() {
    showDialog<ProxyDialog>(proxyDialog);
}

void MainWindow::on_actionShow_Logs_triggered() {
    LogMainWindow *w = new LogMainWindow(this);
    w->show();
}

void MainWindow::on_actionDisable_triggered(bool checked) {
    controller->toggleEnable(!checked);
    controller->updateSystemProxy();
}

void MainWindow::on_actionPAC_triggered(bool checked) {
    controller->toggleEnable(true);
    controller->toggleGlobal(false);
    controller->updateSystemProxy();
}

void MainWindow::on_actionGlobal_triggered(bool checked) {
    controller->toggleEnable(true);
    controller->toggleGlobal(checked);
    controller->updateSystemProxy();
}

void MainWindow::on_actionLocal_PAC_triggered(bool checked) {
    menuLocalPacGroup->setEnabled(true);
    controller->useOnlinePac(false);
    controller->updateSystemProxy();
    ui->actionEdit_Online_PAC_URL->setEnabled(false);
}

void MainWindow::on_actionOnline_PAC_triggered(bool checked) {
    if (controller->getConfiguration().getPacUrl().isEmpty()) {
        showDialog<PACUrlDialog>(pacUrlDialog);
    } else {
        controller->useOnlinePac(true);
        controller->updateSystemProxy();
        menuLocalPacGroup->setEnabled(false);
        ui->actionEdit_Online_PAC_URL->setEnabled(true);
    }
}

void MainWindow::on_actionEdit_Local_PAC_File_triggered() {
    QString filePath = controller->touchPacFile();
    // open file with default editor
//    QDesktopServices::openUrl(QUrl("file://" + filePath, QUrl::TolerantMode));
    // open file browser
//    QDesktopServices::openUrl(QUrl("file://" + QFileInfo(filePath).absolutePath(),  QUrl::TolerantMode));
    Utils::showFileItem(filePath);
}

void MainWindow::on_actionUpdate_Local_PAC_from_GFWList_triggered() {
    controller->updatePacFromGFWList();
}

void MainWindow::on_action_Edit_User_Rule_for_GFWList_triggered() {
    qDebug() << "on_action_Edit_User_Rule_for_GFWList_triggered";
    QString filePath = controller->touchUserRuleFile();
//    QDesktopServices::openUrl(QUrl("file://" + filePath, QUrl::TolerantMode));
    Utils::showFileItem(filePath);
}

void MainWindow::pacUrlChanged(QString pac) {
    controller->savePacUrl(pac);
    emit ui->actionOnline_PAC->activate(QAction::Trigger);
}

void MainWindow::on_actionStart_on_Boot_triggered(bool checked) {
    controller->autoStart(checked);
}

void MainWindow::on_actionQuit_triggered() {
    qApp->exit();
}

void MainWindow::on_actionCopy_Local_PAC_URL_triggered() {
    QClipboard *clipboard = QApplication::clipboard();
//    QString originalText = clipboard->text();
    clipboard->setText(controller->getLocalPacUrl());
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
        BaseResult baseResult = controller->addServerBySSURL(uri);
        if (baseResult.isOk()) {
            showDialog<ConfigDialog>(configDialog);
        } else {
            Utils::info(tr("URL is invalid"));
        }
    }
}

void MainWindow::on_actionImport_URL_from_Clipboard_triggered() {
    QString uri = QApplication::clipboard()->text();
    BaseResult baseResult = controller->addServerBySSURL(uri);
    if (baseResult.isOk()) {
        showDialog<ConfigDialog>(configDialog);
    } else {
        Utils::info(tr("URL is invalid"));
    }
}

void MainWindow::on_actionShare_Server_Config_triggered() {
    ShareDialog *d = new ShareDialog();
    d->setAttribute(Qt::WA_DeleteOnClose);
    d->exec();
}

void MainWindow::on_actionImport_from_gui_config_json_triggered() {
    QString selfilter = tr("JSON (*.json)");
    QString fileName = QFileDialog::getOpenFileName(this,
                                                    tr("choose gui-config.json file"),
                                                    QDir::homePath(),
                                                    "All files (*.*);;JSON (*.json)",
                                                    &selfilter);
    if (fileName.isEmpty()) {
        return;
    }
    controller->importFrom(fileName);
    showDialog<ConfigDialog>(configDialog);
}

void MainWindow::on_actionExport_as_gui_config_json_triggered() {
    QString path = QFileDialog::getExistingDirectory(this,
                                                     tr("Save gui-config.json"),
                                                     QStandardPaths::standardLocations(
                                                             QStandardPaths::DocumentsLocation).first());
    if (path.isEmpty()) {
        return;
    }
    QString fileName = path + "/gui-config.json";
    controller->exportAs(fileName);
    Utils::showFileItem(fileName);
}

void MainWindow::serverConfigChanged() {
    loadMenuServers();
    const Configuration &configuration = controller->getConfiguration();
    menuServerGroup->actions().at(3 + configuration.getIndex())->activate(QAction::Trigger);
}

bool MainWindow::event(QEvent *event) {
    int res = QWidget::event(event);

    if (event->type() == QEvent::Polish) {
        qDebug() << "MainWindow polish event";
        // show config dialog if there is no server
        // don't check the acton if there is no server available
        if (controller->getCurrentServer().getServer().isEmpty()) {
            showDialog<ConfigDialog>(configDialog);
        }
    }

    return res;
}

