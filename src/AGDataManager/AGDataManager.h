#ifndef AG_DATA_MANAGER_H
#define AG_DATA_MANAGER_H

#include <ArduinoJson.h>
#include "AGModule/AGModule.h"
#include "AGUtil/AGUtil.h"
#include "AGPacket/AGPacket.h"

class AGDataManager {
public:
    AGDataManager();
    void printPackage(AGPacket package);
};

#endif // AG_DATA_MANAGER_H