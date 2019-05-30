//
// Created by wallestar on 5/29/19.
//

#ifndef AUTOSTARTUP_H
#define AUTOSTARTUP_H

#include "common/BaseResult.h"

class AutoStartup {
    bool isAutoStart();
    BaseResult autoStart();
    BaseResult removeAutoStart();
};


#endif //AUTOSTARTUP_H
