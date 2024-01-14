#include "AGServerPacket.h"

AGServerPacket::AGServerPacket() {
    this->responseType = "response";
    this->contents = "";
}

AGServerPacket::AGServerPacket(const AGPacket& packet) {
    this->responseType = "response";
    this->contents = packet.data;
}

String AGServerPacket::toString() const {
    DynamicJsonDocument doc(1024);

    doc["header"]["responseType"] = this->responseType;
    doc["header"]["topic"] = this->topic;

    doc["body"] = this->contents;

    String output;
    serializeJson(doc, output);
    return output;
}

void AGServerPacket::contentsFromModuleVector(const std::vector<AGModule>& vector) {
    DynamicJsonDocument doc(1024);
    JsonArray modules = doc.createNestedArray("modules");

    for(AGModule module : vector) {
        JsonObject moduleObject = modules.createNestedObject();
        moduleObject["name"] = module.name;
        moduleObject["type"] = module.type;
        moduleObject["mac"] = macToString(module.macAddress);
        modules.add(moduleObject);
    }

    String output;
    serializeJson(doc, output);
    this->contents = output;
}

AGServerPacket::operator String() {
    return toString();
}