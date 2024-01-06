#ifndef AG_PACKET_H
#define AG_PACKET_H

#include <Arduino.h>

class AGPacket {
public:
    static String delimiter;
    AGPacket(const String& header, const String& data);
    AGPacket(const String& packet);
    String header;
    String data;
    String toString();
    bool operator==(const AGPacket& other) const;
    bool operator!=(const AGPacket& other) const;
    operator String();
};

#endif // AG_PACKET_H