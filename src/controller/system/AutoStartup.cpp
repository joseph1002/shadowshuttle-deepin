//
// Created by wallestar on 5/29/19.
//

#include <QDir>
#include <QStandardPaths>
#include "AutoStartup.h"
#include "common/constant.h"

bool AutoStartup::isAutoStart() {
    // https://specifications.freedesktop.org/autostart-spec/autostart-spec-latest.html
    // /etc/xdg/autostart/ or ~/.config/autostart/
    for (auto path : QStandardPaths::standardLocations(QStandardPaths::ConfigLocation)) {
        QFileInfo autoStartFile(path + "/autostart/" + Constant::DESKTOP_FILE);
        if (autoStartFile.exists())
            return true;
    }
    return false;
}

BaseResult AutoStartup::autoStart() {
    for (auto path : QStandardPaths::standardLocations(QStandardPaths::ApplicationsLocation)) {
        QFileInfo desktopFile(path + "/" + Constant::DESKTOP_FILE);
        if (desktopFile.exists()) {
            QString autoStartFile = QStandardPaths::standardLocations(QStandardPaths::ConfigLocation).first()
                    + "/autostart/" + Constant::DESKTOP_FILE;
            bool success = QFile::copy(desktopFile.filePath(), autoStartFile);
            return BaseResult(success, "");
        }
    }
    return BaseResult::fail(QObject::tr("desktop file not exist!"));
}

BaseResult AutoStartup::removeAutoStart() {
    for (auto path : QStandardPaths::standardLocations(QStandardPaths::ConfigLocation)) {
        QString filePath = path + "/autostart/" + Constant::DESKTOP_FILE;
        QFileInfo autoStartFile(filePath);
        if (autoStartFile.exists()) {
            QFile file(filePath);
            file.remove();
        }
    }
    return BaseResult::success();
}
