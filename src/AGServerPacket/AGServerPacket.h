#ifndef AG_SERVER_PACKET_H
#define AG_SERVER_PACKET_H

#include "AGPacket/AGPacket.h"
#include "AGModule/AGModule.h"
#include "AGUtil/AGUtil.h"
#include <ArduinoJson.h>
#include <vector>

class AGServerPacket {
public:
    AGServerPacket();
    AGServerPacket(const AGPacket& packet);
    String toString() const;
    String topic;
    String responseType;
    String contents;
    void contentsFromModuleVector(const std::vector<AGModule>& vector);
    operator String();
};

#endif // AG_SERVER_PACKET_H
