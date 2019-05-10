#ifndef SYSTEMTRAYMANAGER_H
#define SYSTEMTRAYMANAGER_H

#include "stdafx.h"
#include "controller/ShadowsocksController.h"

class SystemTrayManager : public QObject
{
    Q_OBJECT
public:
    explicit SystemTrayManager(QObject *parent, QMenu *menu);

    void updateTrayIcon(bool isIn, bool isOut);

Q_SIGNALS:
    void trayIconDoubleClicked();

public slots:
            void clickTrayIcon(QSystemTrayIcon::ActivationReason RW);

private:
    void initSystemTrayIcon(QMenu *menu);

    QTimer *trayClickTimer;
    QSystemTrayIcon *systemTrayIcon;
    QString currentIconFile;
    const Configuration& configuration;
};

#endif // SYSTEMTRAYMANAGER_H
