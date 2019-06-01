#include "PACUrlDialog.h"
#include "ui_PACUrlDialog.h"
#include "common/utils.h"


PACUrlDialog::PACUrlDialog(QWidget *parent) :
        QDialog(parent),
        ui(new Ui::PACUrlDialog) {
    ui->setupUi(this);
    setFixedSize(size());
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
