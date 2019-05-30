#include "SystemTrayManager.h"
#include "common/utils.h"
#include "controller/ShadowsocksController.h"

SystemTrayManager::SystemTrayManager(QObject *parent, QMenu *menu) : QObject(parent) {
    controller = &ShadowsocksController::Instance();
    initSystemTrayIcon(menu);
}

void SystemTrayManager::initSystemTrayIcon(QMenu *menu) {
    currentIconFile = "ss_none.svg";
    systemTrayIcon = new QSystemTrayIcon(this);
    systemTrayIcon->setContextMenu(menu);
    systemTrayIcon->setIcon(QIcon(Utils::getQrcTrayIconPath(currentIconFile)));

    connect(systemTrayIcon, &QSystemTrayIcon::activated, this, &SystemTrayManager::clickTrayIcon);

    trayClickTimer = new QTimer(this);
    trayClickTimer->setSingleShot(true);
    trayClickTimer->setInterval(QApplication::doubleClickInterval());

    systemTrayIcon->show();
}

void SystemTrayManager::updateTrayIcon(bool isIn, bool isOut) {
    QString icon = "ss";
    const Configuration& configuration = controller->getConfiguration();
    if (!configuration.isEnabled()) {
        icon.append("_none");
    } else if (configuration.isGlobal()) {
        icon.append("_global");
    } else {
        icon.append("_pac");
    }
    if (isIn) {
        icon.append("_in");
    }
    if (isOut) {
        icon.append("_out");
    }
    icon.append(".svg");

    if (icon == currentIconFile) {
        return;
    }
    systemTrayIcon->setIcon(QIcon(Utils::getQrcTrayIconPath(icon)));
    currentIconFile = icon;
}

void SystemTrayManager::clickTrayIcon(QSystemTrayIcon::ActivationReason reason) {
    qDebug() << reason;

    if (reason == QSystemTrayIcon::Trigger) {
        if (trayClickTimer->remainingTime() <= 0) {
            //timer is not active or finished (first click)
            trayClickTimer->start();
        } else {
            //timer is active (second click)
            trayClickTimer->stop();
            emit systemTrayIcon->activated(QSystemTrayIcon::DoubleClick);
        }
    } else if (reason == QSystemTrayIcon::DoubleClick) {
        emit trayIconDoubleClicked();
    }
}
