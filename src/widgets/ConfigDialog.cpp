#include <QtShadowsocks>
#include "ConfigDialog.h"
#include "ui_ConfigDialog.h"
#include "common/utils.h"
#include "controller/ShadowsocksController.h"

ConfigDialog::ConfigDialog(QWidget *parent) :
        QDialog(parent),
        ui(new Ui::ConfigDialog) {
    ui->setupUi(this);

    controller = &ShadowsocksController::Instance();
    configuration = controller->getConfiguration();

    backToPrevious = std::nullopt;

    listClickTimer = new QTimer(this);
    listClickTimer->setSingleShot(true);
    listClickTimer->setInterval(5);

    std::vector<std::string> methodBA = QSS::Cipher::supportedMethods();
    std::sort(methodBA.begin(), methodBA.end());
    QStringList methodList;
    for (const std::string &method : methodBA) {
        methodList.push_back(QString::fromStdString(method));
    }
    ui->comboBoxEncryption->addItems(methodList);

    connect(listClickTimer, &QTimer::timeout, this, &ConfigDialog::backToPreviousItem);
    connect(ui->listWidget, &QListWidget::currentRowChanged, this, &ConfigDialog::listWidgetCurrentRowChanged);
    connect(ui->listWidget, &QListWidget::currentItemChanged, this, &ConfigDialog::listWidgetCurrentItemChanged);

    connect(ui->pushButtonAdd, &QPushButton::clicked, this, &ConfigDialog::buttonAddClicked);
    connect(ui->pushButtonDelete, &QPushButton::clicked, this, &ConfigDialog::buttonDeleteClicked);
    connect(ui->pushButtonMoveUp, &QPushButton::clicked, this, &ConfigDialog::buttonMoveUpClicked);
    connect(ui->pushButtonMoveDown, &QPushButton::clicked, this, &ConfigDialog::buttonMoveDownClicked);
    connect(ui->pushButtonDuplicate, &QPushButton::clicked, this, &ConfigDialog::buttonDuplicateClicked);
    connect(ui->pushButtonOK, &QPushButton::clicked, this, &ConfigDialog::buttonOKClicked);

    connect(ui->checkBoxShowPassword, &QCheckBox::clicked, this, &ConfigDialog::showPasswordChecked);
    connect(ui->checkBoxPortableMode, &QCheckBox::clicked, this, &ConfigDialog::portableModeChecked);

    setFixedSize(size());

    updateListWidget();
}

ConfigDialog::~ConfigDialog() {
    delete ui;
}

void ConfigDialog::updateListWidget() {
    ui->listWidget->clear();

    for (const ServerConfig &serverConfig : configuration.getServerConfigs()) {
        ui->listWidget->addItem(serverConfig.friendlyName());
    }
    ui->listWidget->setCurrentRow(configuration.getIndex());
}

BaseResult ConfigDialog::checkServerConfig(const ServerConfig &serverConfig) {
    BaseResult baseResult = Configuration::checkServerConfig(serverConfig);
    if (!baseResult.isOk()) {
        QMessageBox::information(nullptr, "warning", baseResult.getMsg(),
                                 QMessageBox::Yes);
    }
    return baseResult;
}

void ConfigDialog::listWidgetCurrentRowChanged(int currentRow) {
    qDebug() << "currentRowChanged current row:" << currentRow;

    enableMoveUpDownButtons();
    if (currentRow < 0 || (backToPrevious.has_value() && backToPrevious.value() == currentRow)) {
        return;
    }

    updateServerView(configuration.getServerConfigs()[currentRow]);
    configuration.setIndex(currentRow);
}

void ConfigDialog::listWidgetCurrentItemChanged(QListWidgetItem *current, QListWidgetItem *previous) {
    int previousRow = ui->listWidget->row(previous);
    int currentRow = ui->listWidget->row(current);
    qDebug() << "current row:" << currentRow;
    qDebug() << " previous row:" << previousRow;
    if (previousRow < 0 || currentRow < 0)
        return;

    // if the last row is deleted, value of previousRow may be equal with configuration.getServerConfigs().size()
    if (previousRow >= configuration.getServerConfigs().size())
        return;

    if (backToPrevious.has_value()) {
        backToPrevious = std::nullopt;
        return;
    }

    ServerConfig &serverConfig = configuration.getServerConfigs()[previousRow];
    flushServerConfig(serverConfig);
    if (!checkServerConfig(serverConfig).isOk()) {
        backToPrevious = {previousRow};
        listClickTimer->start();
    } else if (serverConfig.friendlyName() != previous->text()) {
        previous->setText(serverConfig.friendlyName());
    }
}

void ConfigDialog::backToPreviousItem() {
    if (backToPrevious.has_value())
        ui->listWidget->setCurrentRow(backToPrevious.value());
}

void ConfigDialog::flushServerConfig(ServerConfig &serverConfig) {
    serverConfig.setPassword(ui->lineEditPassword->text());
    serverConfig.setRemarks(ui->lineEditRemarks->text());
    serverConfig.setServer(ui->lineEditServerAddr->text());
    serverConfig.setServerPort(ui->spinBoxServerPort->value());
    serverConfig.setTimeout(ui->spinBoxTimeout->value());
    serverConfig.setMethod(ui->comboBoxEncryption->currentText());
}

void ConfigDialog::updateServerView(const ServerConfig &serverConfig) {
    ui->lineEditPassword->setText(serverConfig.getPassword());
    ui->lineEditRemarks->setText(serverConfig.getRemarks());
    ui->lineEditServerAddr->setText(serverConfig.getServer());
    ui->spinBoxServerPort->setValue(serverConfig.getServerPort());
    ui->spinBoxTimeout->setValue(serverConfig.getTimeout());
    ui->comboBoxEncryption->setCurrentText(serverConfig.getMethod());
    ui->spinBoxProxyPort->setValue(configuration.getLocalPort());
}

void ConfigDialog::buttonAddClicked() {
    int idx = ui->listWidget->currentRow();

    qDebug() << "buttonAddClicked current row:" << idx;

    ServerConfig &serverConfig = configuration.getServerConfigs()[idx];
    flushServerConfig(serverConfig);
    if (!checkServerConfig(serverConfig).isOk()) {
        return;
    }

    // If yes create a new row
    ServerConfig newServerConfig = configuration.getDefaultServer();
    configuration.addServerConfig(newServerConfig);
    configuration.setIndex(configuration.getServerConfigs().size() - 1);

    updateListWidget();
}

void ConfigDialog::buttonDeleteClicked() {
    int idx = ui->listWidget->currentRow();

    configuration.getServerConfigs().removeAt(idx);
    // if no server config exists, we still add a default one, that's what shadowsocks-windows do
    if (configuration.getServerConfigs().isEmpty()) {
        ServerConfig newServerConfig = configuration.getDefaultServer();
        configuration.addServerConfig(newServerConfig);
    }

    if (idx >= configuration.getServerConfigs().size()) {
        idx = configuration.getServerConfigs().size() - 1;
    }
    configuration.setIndex(idx);
    // replace server config form's values
    ServerConfig &serverConfig = configuration.getServerConfigs()[idx];
    updateServerView(serverConfig);
    updateListWidget();
}

void ConfigDialog::buttonDuplicateClicked() {
    int idx = ui->listWidget->currentRow();

    ServerConfig &serverConfig = configuration.getServerConfigs()[idx];
    flushServerConfig(serverConfig);
    if (!checkServerConfig(serverConfig).isOk()) {
        return;
    }

    QList<ServerConfig> &serverConfigs = configuration.getServerConfigs();
    serverConfigs.insert(idx + 1, serverConfig);

    configuration.setIndex(idx + 1);

    updateListWidget();
}

void ConfigDialog::buttonMoveUpClicked() {
    int idx = ui->listWidget->currentRow();
    if (idx <= 0)
        return;

    ServerConfig &serverConfig = configuration.getServerConfigs()[idx];
    flushServerConfig(serverConfig);
    if (!checkServerConfig(serverConfig).isOk()) {
        return;
    }

    ServerConfig &lastServerConfig = configuration.getServerConfigs()[idx - 1];

    ServerConfig temp = lastServerConfig;
    lastServerConfig = serverConfig;
    serverConfig = temp;

    configuration.setIndex(idx - 1);
    updateListWidget();
}

void ConfigDialog::buttonMoveDownClicked() {
    int idx = ui->listWidget->currentRow();
    if (idx >= configuration.getServerConfigs().size() - 1)
        return;

    ServerConfig &serverConfig = configuration.getServerConfigs()[idx];
    flushServerConfig(serverConfig);
    if (!checkServerConfig(serverConfig).isOk()) {
        return;
    }

    ServerConfig &nextServerConfig = configuration.getServerConfigs()[idx + 1];

    ServerConfig temp = nextServerConfig;
    nextServerConfig = serverConfig;
    serverConfig = temp;

    configuration.setIndex(idx + 1);

    updateListWidget();
}

void ConfigDialog::buttonOKClicked() {
    int idx = ui->listWidget->currentRow();

    ServerConfig &serverConfig = configuration.getServerConfigs()[idx];
    flushServerConfig(serverConfig);
    if (!checkServerConfig(serverConfig).isOk()) {
        return;
    }

    configuration.setLocalPort(ui->spinBoxProxyPort->value());
    configuration.setIndex(idx);
    controller->saveServers(configuration.getServerConfigs(),
                            configuration.getLocalPort(),
                            configuration.getIndex(),
                            configuration.isPortableMode());

    emit configChanged();

    QDialog::close();
}

void ConfigDialog::showPasswordChecked(bool checked) {
    if (checked) {
        ui->lineEditPassword->setEchoMode(QLineEdit::Normal);
    } else {
        ui->lineEditPassword->setEchoMode(QLineEdit::Password);
    }
}

void ConfigDialog::portableModeChecked(bool checked) {
    configuration.setLocalPort(checked);
}

void ConfigDialog::enableMoveUpDownButtons() {
    int size = configuration.getServerConfigs().size();
    int currentRow = ui->listWidget->currentRow();
    if (size <= 1) {
        ui->pushButtonMoveUp->setEnabled(false);
        ui->pushButtonMoveDown->setEnabled(false);
    } else if (currentRow == size - 1) {
        ui->pushButtonMoveDown->setEnabled(false);
    } else if (currentRow == 0) {
        ui->pushButtonMoveUp->setEnabled(false);
    } else {
        ui->pushButtonMoveUp->setEnabled(true);
        ui->pushButtonMoveDown->setEnabled(true);
    }
}

void ConfigDialog::reject() {
    qDebug() << "reject()";
    configuration = controller->getConfiguration();
    backToPrevious = std::nullopt;
    QDialog::reject();
}

void ConfigDialog::showEvent(QShowEvent *event) {
    qDebug() << "showEvent()";
    if (configuration.getIndex() != controller->getConfiguration().getIndex()) {
        configuration = controller->getConfiguration();
        updateListWidget();
    }
    QDialog::showEvent(event);
}
