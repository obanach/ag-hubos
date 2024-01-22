#include "AGServerPacket.h"

AGServerPacket::AGServerPacket() {
    this->responseType = "response";
    this->contents = "";
}

AGServerPacket::AGServerPacket(const AGPacket& packet) {
    this->responseType = "response";
    this->contents = packet.data;
}

AGServerPacket::AGServerPacket(const String& serializedPacket) {
    DynamicJsonDocument doc(1024);
    deserializeJson(doc, serializedPacket);

    String deserializedResponse = doc["header"]["type"];
    this->responseType = deserializedResponse;
    String deserializedContents = doc["body"];
    this->contents = deserializedContents;
}

String AGServerPacket::toString() const {
    DynamicJsonDocument doc(1024);

    doc["header"]["type"] = this->responseType;

    DynamicJsonDocument contentDoc(1024);
    DeserializationError error = deserializeJson(contentDoc, this->contents);
    if (!error) {
        doc["body"] = contentDoc.as<JsonObject>();
    } else {
        doc["body"] = this->contents;
    }

    String output;
    serializeJson(doc, output);
    return output;
}

void AGServerPacket::contentsFromModuleVector(const std::vector<AGModule>& vector) {
    DynamicJsonDocument doc(1024);
    JsonArray modules = doc.createNestedArray("modules");

    for(int i = 0; i < vector.size(); i++) {
        JsonObject moduleObject = modules.createNestedObject();
        moduleObject["name"] = vector[i].name;
        moduleObject["type"] = vector[i].type;
        moduleObject["macAddress"] = macToString(vector[i].macAddress);
    }

    String output;
    serializeJson(doc, output);
    this->contents = output;
}

AGServerPacket::operator String() {
    return toString();
}