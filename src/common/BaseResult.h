//
// Created by wallestar on 4/19/19.
//

#ifndef BASERESULT_H
#define BASERESULT_H


#include <QtCore/QString>

class BaseResult {
public:
    BaseResult(bool ok, const QString &msg);

    static BaseResult success();
    static BaseResult success(QString);
    static BaseResult fail();
    static BaseResult fail(QString);

    bool isOk() const;

    void setOk(bool ok);

    const QString &getMsg() const;

    void setMsg(const QString &msg);

private:
    bool ok;
    QString msg;
};


#endif //BASERESULT_H
