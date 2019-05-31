//
// Created by wallestar on 5/29/19.
//

#ifndef AUTOSTARTUP_H
#define AUTOSTARTUP_H

#include "common/BaseResult.h"

class AutoStartup {
public:
    static bool isAutoStart();
    static BaseResult autoStart();
    static BaseResult removeAutoStart();
};


#endif //AUTOSTARTUP_H
