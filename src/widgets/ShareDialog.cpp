#include "ShareDialog.h"
#include <QHBoxLayout>
#include "common/utils.h"
#include "controller/ShadowsocksController.h"

ShareDialog::ShareDialog(QWidget *parent) :
        QDialog(parent) {
    controller = &ShadowsocksController::Instance();

    QVBoxLayout *vlayout = new QVBoxLayout(this);
    QHBoxLayout *hlayout = new QHBoxLayout(this);

    qrWidget = new QRWidget(this);
    listWidget = new QListWidget(this);
    uriText = new QLineEdit(this);
    uriText->setReadOnly(true);

    hlayout->addWidget(qrWidget, 0, Qt::AlignLeft | Qt::AlignTop);
    hlayout->addWidget(listWidget, 0, Qt::AlignLeft | Qt::AlignTop);
    vlayout->addLayout(hlayout);
    vlayout->addWidget(uriText);

    this->setLayout(vlayout);

    qrWidget->setMinimumSize(200, 200);

    connect(listWidget, &QListWidget::currentRowChanged, this, &ShareDialog::listWidgetCurrentRowChanged);
    connect(listWidget, &QListWidget::currentItemChanged, this, &ShareDialog::listWidgetCurrentItemChanged);

    QSize sz = size();
    sz.setHeight(260);
    sz.setWidth(460);
    setFixedSize(sz);
}

ShareDialog::~ShareDialog() {
    qDebug() << "ShareDialog::~ShareDialog()";
}

void ShareDialog::closeEvent(QCloseEvent *) {
    qDebug() << "set list view to nullptr";
//    listView->setParent(nullptr);
}

void ShareDialog::updateListWidget() {
    listWidget->clear();

    const Configuration &configuration = controller->getConfiguration();
    for (const ServerConfig &serverConfig : configuration.getServerConfigs()) {
//        listWidget->addItem(serverConfig.friendlyName());
        QListWidgetItem* item = new QListWidgetItem();
        QVariant data(controller->getServerURL(serverConfig));
        item->setData(Qt::UserRole, data);
        item->setText(serverConfig.friendlyName());
        listWidget->addItem(item);
    }
    listWidget->setCurrentRow(configuration.getIndex());
}

void ShareDialog::listWidgetCurrentRowChanged(int currentRow) {
    if (currentRow < 0)
        return;

//    const Configuration &configuration = controller->getConfiguration();
//    const ServerConfig &serverConfig = configuration.getServerConfigs()[currentRow];
//    QString uri = controller->getServerURL(serverConfig);
    QListWidgetItem* currentItem = listWidget->item(currentRow);
    QString uri = currentItem->data(Qt::UserRole).toString();

    setCurrentURI(uri);
    uriText->setText(uri);
}

void ShareDialog::listWidgetCurrentItemChanged(QListWidgetItem *current, QListWidgetItem *previous) {

}

void ShareDialog::onSaveQRCodeImage() {
    QString filename = QFileDialog::getSaveFileName(nullptr, tr("Save QRCode Image"),
                                                    QStandardPaths::standardLocations(
                                                            QStandardPaths::PicturesLocation).first(),
                                                    tr("Images (*.png *.xpm *.jpg)"));
    if (filename.isEmpty()) {
        return;
    }
    bool flag = qrWidget->getQRImage().save(filename, "png");
    if (flag) {
        Utils::info(tr("Save QRCode Image Success"));
        Utils::showFileItem(filename);
    } else {
        Utils::critical(tr("Save QRCode Image Error"));
    }
}

void ShareDialog::setCurrentURI(QString uri) {
    currentURI = uri;
    qrWidget->setQRData(uri.toLocal8Bit());
    qrWidget->update();
    // 自己做一下换行
    QStringList l;
    int width = 20;
    while (uri.length() > width) {
        l << uri.left(width);
        uri = uri.remove(0, width);
    }
    l << uri;
}

void ShareDialog::showEvent(QShowEvent *event) {
    qDebug() << "showEvent()";
    updateListWidget();
    QDialog::showEvent(event);
}
