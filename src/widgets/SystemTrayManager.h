#ifndef SYSTEMTRAYMANAGER_H
#define SYSTEMTRAYMANAGER_H

#include <QtWidgets>
#include "controller/ShadowsocksController.h"

class SystemTrayManager : public QObject
{
    Q_OBJECT
public:
    explicit SystemTrayManager(QObject *parent, QMenu *menu);

    void updateTrayIcon(bool isIn, bool isOut);
    void clickTrayIcon(QSystemTrayIcon::ActivationReason RW);

Q_SIGNALS:
    void trayIconDoubleClicked();

private:
    void initSystemTrayIcon(QMenu *menu);

    QTimer *trayClickTimer;
    QSystemTrayIcon *systemTrayIcon;
    QString currentIconFile;
    ShadowsocksController* controller;
};

#endif // SYSTEMTRAYMANAGER_H
