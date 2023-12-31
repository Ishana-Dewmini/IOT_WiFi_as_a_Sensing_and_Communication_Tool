#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

const char *SSID = "GA20s";
const char *PASSWORD = "anud1237";
const char *mqttBroker = "broker.hivemq.com";
const int mqttPort = 1883;
const char *mqttClientID = "Esp32_group07_2";
const char *Topic = "Group071";

WiFiClient espClient;
PubSubClient client(espClient);

void WIFIsetup() {
  client.setServer(mqttBroker, mqttPort);
  WiFi.begin(SSID, PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(SSID);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

String bssidToString(uint8_t *bssid) {
  String result = "";
  for (int i = 0; i < 6; i++) {
    result += String(bssid[i], HEX);
    if (i < 5) {
      result += ":";
    }
  }
  return result;
}

void reconnect() {
  while (!client.connected()) {
    if (client.connect("ESP32Client")) {
      Serial.println("Connected to MQTT broker");
    } else {
      Serial.println("Not connected to MQTT broker");
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  Serial.println(F("\nESP8266 WiFi scan example"));

  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);
  WIFIsetup();
  reconnect();
}

void loop() {
  String ssid;
  int32_t rssi;
  uint8_t encryptionType;
  uint8_t *bssid;
  int32_t channel;
  bool hidden;
  int scanResult;

  Serial.println(F("Starting WiFi scan..."));

  scanResult = WiFi.scanNetworks(/*async=*/false, /*hidden=*/true);

  if (scanResult == 0) {
    Serial.println(F("No networks found"));
  } else if (scanResult > 0) {
    Serial.printf(PSTR("%d networks found:\n"), scanResult);

    // Create a JSON array to hold network data
    StaticJsonDocument<2500> doc; // Adjust the size as needed
    JsonArray jsonArray = doc.to<JsonArray>();

    // Iterate through the scanned networks
    //for (int8_t i = 0; i < scanResult; i++) {
    for (int8_t i = 0; i < 4; i++) {
      
      WiFi.getNetworkInfo(i, ssid, encryptionType, rssi, bssid, channel, hidden);

      String rssiString = String(rssi);
      String bssidString = bssidToString(bssid);

      // Create a JSON object for each network
      JsonObject networkObject = jsonArray.createNestedObject();
      networkObject["index"] = i;
      networkObject["bssid"] = bssidString;
      networkObject["rssi"] = rssiString;
    }

    // Serialize the JSON array to a string
    String jsonString;
    serializeJson(doc, jsonString);

    if (!client.connected()) {
      reconnect();
    }
    // Publish the JSON data
    int messageLength = jsonString.length();
    Serial.println(jsonString);    
    Serial.println("Message length: " + String(messageLength) + " bytes");
    client.publish(Topic, jsonString.c_str(), 0);
    client.loop();
    delay(5000);
    
    
                
    

    
    
  } else {
    Serial.printf(PSTR("WiFi scan error %d"), scanResult);
  }

  // Wait a bit before scanning again
  delay(5000);
}
