#include "PACUrlDialog.h"
#include "ui_PACUrlDialog.h"
#include "common/utils.h"
#include "controller/ShadowsocksController.h"

PACUrlDialog::PACUrlDialog(QWidget *parent) :
        QDialog(parent),
        ui(new Ui::PACUrlDialog) {
    ui->setupUi(this);
    setFixedSize(size());

    ui->lineEdit->setText(ShadowsocksController::Instance().getConfiguration().getPacUrl());
}

PACUrlDialog::~PACUrlDialog() {
    delete ui;
}

//void PACUrlDialog::on_buttonBox_clicked(QAbstractButton *button) {
//    qDebug() << "on_buttonBox_clicked";
//    if(ui->buttonBox->button(QDialogButtonBox::Ok)  == button) {
//        QString pac = ui->lineEdit->text();
//        emit pacUrlChanged(pac);
//    }
//}

void PACUrlDialog::accept() {
    qDebug() << "accept";
    QString pacUrl = ui->lineEdit->text();
    if (!pacUrl.startsWith("file://", Qt::CaseInsensitive)
            && !pacUrl.startsWith("http://", Qt::CaseInsensitive)
            && !pacUrl.startsWith("https://", Qt::CaseInsensitive)) {
        Utils::critical(tr("not valid url"));
        return;
    }

    QUrl parsedUrl(pacUrl);
    if (!parsedUrl.isValid()) {
        Utils::critical(tr("not valid url"));
        return;
    }
    emit pacUrlChanged(pacUrl);
    QDialog::accept();
}

void PACUrlDialog::showEvent(QShowEvent *event) {
    qDebug() << "showEvent()";
    ui->lineEdit->setFocus();
    ui->lineEdit->selectAll();
    QDialog::showEvent(event);
}
