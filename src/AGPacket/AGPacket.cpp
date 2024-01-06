#include "AGPacket.h"

String AGPacket::delimiter = "\n\r\n\r";

AGPacket::AGPacket(const String& header, const String& data)
    : header(header), data(data) {}

AGPacket::AGPacket(const String& packet) {
    int index = packet.indexOf(delimiter);
    if(index == -1) {
        header = packet;
        data = "";
    } else {
        header = packet.substring(0, index);
        data = packet.substring(index + delimiter.length());
    }
}

String AGPacket::toString() {
    return header + delimiter + data;
}

bool AGPacket::operator==(const AGPacket& other) const {
    return header == other.header && data == other.data;
}

bool AGPacket::operator!=(const AGPacket& other) const {
    return !(*this == other);
}

AGPacket::operator String() {
    return toString();
}
