#include <DApplication>
#include <DLog>
#include <DWidgetUtil>
#include "common/utils.h"
#include "widgets/MainWindow.h"

#include <thread>
#include <unistd.h>
#include <csignal>

DWIDGET_USE_NAMESPACE
DCORE_USE_NAMESPACE

static void onSignalRecv(int sig) {
    if (sig == SIGINT || sig == SIGTERM) {
        qApp->quit();
    } else {
        qWarning("Unhandled signal %d", sig);
    }
}

int main(int argc, char *argv[]) {
    DApplication::loadDXcbPlugin();
    DApplication app(argc, argv);
    app.setAttribute(Qt::AA_UseHighDpiPixmaps);
    const QString socket_path(QString("shadowshuttle-deepin_%1").arg(getuid()));
    if (app.setSingleInstance(socket_path)) {
        signal(SIGINT, onSignalRecv);
        signal(SIGTERM, onSignalRecv);
        app.setTheme("light");
        app.loadTranslator();

        const QString descriptionText = QApplication::tr(
                    "If you want to keep a secret, you must also hide it from yourself.");

        const QString acknowledgementLink = "https://github.com/joseph1002/shadowshuttle-deepin";

        app.setOrganizationName("wallestar");
        app.setApplicationName("shadowshuttle-deepin");
        app.setApplicationDisplayName(QObject::tr("Shadowshuttle Deepin"));
        app.setApplicationVersion("1.1.0");

        app.setProductIcon(QIcon(Utils::getIconQrcPath("ssw128.svg")));
        app.setProductName(QApplication::tr("Shadowshuttle Deepin"));
        app.setApplicationDescription(descriptionText);
        app.setApplicationAcknowledgementPage(acknowledgementLink);

        app.setWindowIcon(QIcon(Utils::getIconQrcPath("ssw128.png")));
        app.setQuitOnLastWindowClosed(false);
        DLogManager::registerConsoleAppender();
        DLogManager::registerFileAppender();
        MainWindow window;
        // window is not needed to be shown
//        window.show();

        QObject::connect(&app, &DApplication::newInstanceStarted, &window, &MainWindow::activateWindow);

        Dtk::Widget::moveToCenter(&window);
        return app.exec();
    }
    qDebug() << "\033[32mApp has started";
    return 0;
}
