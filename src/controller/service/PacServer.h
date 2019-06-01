//
// Created by wallestar on 5/29/19.
//

#ifndef PACSERVER_H
#define PACSERVER_H

#include <QtCore>

class Configuration;

class PacServer {
public:
  QString touchPacFile();
  QString getPacUrl();

private:
  const QString PAC_FILE = "pac.txt";
  const QString USER_RULE_FILE = "user-rule.txt";
  const QString USER_ABP_FILE = "abp.txt";
};


#endif //PACSERVER_H
