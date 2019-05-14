#include "SystemTrayManager.h"
#include "common/utils.h"
#include "controller/ShadowsocksController.h"

SystemTrayManager::SystemTrayManager(QObject *parent, QMenu *menu) : QObject(parent),
    configuration(ShadowsocksController::Instance().getCurrentConfiguration()) {
    initSystemTrayIcon(menu);
}

void SystemTrayManager::initSystemTrayIcon(QMenu *menu) {
    currentIconFile = "ss16.png";
    systemTrayIcon = new QSystemTrayIcon(this);
    systemTrayIcon->setContextMenu(menu);
    systemTrayIcon->setIcon(QIcon(Utils::getIconQrcPath(currentIconFile)));

    connect(systemTrayIcon, &QSystemTrayIcon::activated, this, &SystemTrayManager::clickTrayIcon);

    trayClickTimer = new QTimer(this);
    trayClickTimer->setSingleShot(true);
    trayClickTimer->setInterval(QApplication::doubleClickInterval());

    systemTrayIcon->show();
}

void SystemTrayManager::updateTrayIcon(bool isIn, bool isOut) {
    QString icon = "ssw";
    if (isIn) {
        icon.append("_in");
    }
    if (isOut) {
        icon.append("_out");
    }
    if (!configuration.isEnabled()) {
        icon.append("_none");
    } else if (configuration.isGlobal()) {
        icon.append("_manual");
    } else {
        icon.append("_auto");
    }
    icon.append("128.svg");

    if (icon == currentIconFile) {
        return;
    }
    systemTrayIcon->setIcon(QIcon(Utils::getIconQrcPath(icon)));
    currentIconFile = icon;
}

void SystemTrayManager::clickTrayIcon(QSystemTrayIcon::ActivationReason reason) {
    qDebug() << reason;

    if (reason == QSystemTrayIcon::Trigger) {
        if (trayClickTimer->remainingTime() <= 0) {
            //timer is not active or finished (first click)
            trayClickTimer->start();
        } else { //timer is active (second click)
            trayClickTimer->stop();
            emit systemTrayIcon->activated(QSystemTrayIcon::DoubleClick);
        }
    } else if (reason == QSystemTrayIcon::DoubleClick) {
//        emit ui->actionEdit_Servers->trigger();
        emit trayIconDoubleClicked();
    }
}
