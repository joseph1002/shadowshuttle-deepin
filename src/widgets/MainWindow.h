/**
 * Copyright (C) 2017 ~ 2018 PikachuHy
 *
 * Author:     PikachuHy <pikachuhy@163.com>
 * Maintainer: PikachuHy <pikachuhy@163.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtCore>
#include "ProxyManager.h"
#include "Settings.h"
#include "Toolbar.h"
//#include "widget/ProfileView.h"

namespace Ui {
    class MainWindow;
}

class SystemTrayManager;
class Configuration;
class ConfigDialog;
class PACUrlDialog;
class ProxyDialog;
class ShadowsocksController;

class MainWindow : public QMainWindow {
Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

    ~MainWindow() override;

private:
    void initSystemTrayIcon();
    void LoadContextMenu();
    void initProxy();
    void loadMenuServers();

private slots:
    void on_actionEdit_Servers_triggered();
    void on_actionEdit_Online_PAC_URL_triggered();
    void on_actionForward_Proxy_triggered();
    void on_actionShow_Logs_triggered();
    void on_actionImport_from_gui_config_json_triggered();
    void on_actionEnable_System_Proxy_triggered(bool flag);
    void on_actionPAC_triggered(bool checked);
    void on_actionGlobal_triggered(bool checked);
    void on_actionStart_on_Boot_triggered(bool checked);
    void on_actionQuit_triggered();
    void on_actionDisconnect_triggered();
    void on_actionScan_QRCode_from_Screen_triggered();
    void on_actionImport_URL_from_Clipboard_triggered();
    void on_actionShare_Server_Config_triggered();
    void on_actionExport_as_gui_config_json_triggered();

    void serverConfigChanged();
    void updateTrayIcon();

protected:
    bool event(QEvent *) override;

private:
    Ui::MainWindow *ui;
    Toolbar *toolbar;
    ProxyManager *proxyManager;
    SystemTrayManager *systemTrayManager;
    QActionGroup *menuServerGroup;
    QActionGroup *menuPacGroup;
    ConfigDialog *configDialog;
    PACUrlDialog *pacUrlDialog;
    ProxyDialog *proxyDialog;
    ShadowsocksController *controller;
    quint64 in;
    quint64 out;
    quint64 term_usage_in;
    quint64 term_usage_out;
};

#endif // MAINWINDOW_H
