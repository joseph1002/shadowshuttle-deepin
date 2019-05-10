#ifndef SHAREDIALOG_H
#define SHAREDIALOG_H
#include "stdafx.h"
#include "widgets/QRWidget.h"
#include "widgets/SingleListView.h"

class ShareDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ShareDialog(QWidget *parent = nullptr);
    virtual ~ShareDialog();
    void closeEvent(QCloseEvent *);
private slots:
    void onItemSelected();
    void onSaveQRCodeImage();
private:
    SingleListView* listView;
    QRWidget* qrWidget;
    QLabel* uriLabel;
    QPlainTextEdit* uriText;
    QPushButton* saveButton;

    QString currentURI;

    void setCurrentURI(QString uri);
};

#endif // SHAREDIALOG_H
