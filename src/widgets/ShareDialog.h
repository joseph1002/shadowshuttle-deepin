#ifndef SHAREDIALOG_H
#define SHAREDIALOG_H

#include <QtWidgets>

#include "widgets/QRWidget.h"

class ShadowsocksController;

class ShareDialog : public QDialog {
Q_OBJECT

public:
    explicit ShareDialog(QWidget *parent = nullptr);

    virtual ~ShareDialog();

    void closeEvent(QCloseEvent *);

private slots:

//    void onItemSelected();
    void onSaveQRCodeImage();

    void listWidgetCurrentRowChanged(int currentRow);

    void listWidgetCurrentItemChanged(QListWidgetItem *current, QListWidgetItem *previous);

protected:
    void showEvent(QShowEvent *) override;

private:
    QListWidget *listWidget;
    QRWidget *qrWidget;
    QLineEdit *uriText;
    ShadowsocksController *controller;

    QString currentURI;

    void setCurrentURI(QString uri);

    void updateListWidget();
};

#endif // SHAREDIALOG_H
