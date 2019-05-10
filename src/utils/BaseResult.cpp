//
// Created by wallestar on 4/19/19.
//

#include "BaseResult.h"

BaseResult BaseResult::success() {
    return {true, ""};
}

BaseResult BaseResult::success(QString msg) {
    return {true, msg};
}

BaseResult BaseResult::fail() {
    return {false, ""};
}

BaseResult BaseResult::fail(QString msg) {
    return {false, msg};
}

BaseResult::BaseResult(bool ok, const QString &msg) : ok(ok), msg(msg) {}

bool BaseResult::isOk() const {
    return ok;
}

void BaseResult::setOk(bool ok) {
    BaseResult::ok = ok;
}

const QString &BaseResult::getMsg() const {
    return msg;
}

void BaseResult::setMsg(const QString &msg) {
    BaseResult::msg = msg;
}


