/**
 * Copyright (C) 2017 ~ 2018 PikachuHy
 *
 * Author:     PikachuHy <pikachuhy@163.com>
 * Maintainer: PikachuHy <pikachuhy@163.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef CONFIGDIALOG_H
#define CONFIGDIALOG_H

#include <QtCore>
#include <QtWidgets>
#include <QDialog>
#include "common/BaseResult.h"
#include "model/Configuration.h"

class ShadowsocksController;
class ServerConfig;

namespace Ui {
class ConfigDialog;
}

class ConfigDialog : public QDialog {
    Q_OBJECT
public:
    explicit ConfigDialog(QWidget *parent = nullptr);

    ~ConfigDialog() override;

Q_SIGNALS:
    void configChanged();

private:
    BaseResult checkServerConfig(const ServerConfig& serverConfig);

    void updateListWidget();
    void flushServerConfig(ServerConfig& serverConfig);
    void updateServerView(const ServerConfig& serverConfig);
    void enableMoveUpDownButtons();

private slots:
    virtual void reject() override;

    void buttonDuplicateClicked();
    void buttonMoveUpClicked();
    void buttonMoveDownClicked();
    void buttonDeleteClicked();
    void buttonAddClicked();
    void buttonOKClicked();

    void listWidgetCurrentRowChanged(int currentRow);
    void listWidgetCurrentItemChanged(QListWidgetItem *current, QListWidgetItem *previous);
    void backToPreviousItem();
    void showPasswordChecked(bool checked);
    void portableModeChecked(bool checked);

protected:
    void showEvent(QShowEvent *) override;

private:
    Ui::ConfigDialog *ui;
    QTimer* listClickTimer;
    Configuration configuration;
    std::optional<int> backToPrevious;
    ShadowsocksController *controller;
};

#endif // CONFIGDIALOG_H
