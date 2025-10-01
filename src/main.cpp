#include <WiFi.h>
#include <PicoMQTT.h>

const char* ssid = "Wi-FiUnamBots";
const char* password = "fiounambots";

// Configure static IP to be on the same network as the broker
IPAddress local_IP(10, 0, 0, 200);    // IP different from broker (99)
IPAddress gateway(10, 0, 0, 100);      // Gateway
IPAddress subnet(255, 0, 0, 0);

void conectarWiFi() {
    WiFi.config(local_IP, gateway, subnet);
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
}

PicoMQTT::Client mqtt(
  "10.0.0.100",   // Broker
  1883,             // Port
  "Robot1",   // Client ID
  "user1",          // Username
  "1234"            // Password
);

void setup(){
    // Conectar al WiFi
    conectarWiFi();

    mqtt.begin();
}

void loop(){
    // Verificar conexión WiFi cada ciclo
    if (WiFi.status() != WL_CONNECTED) {
        conectarWiFi();
    }

    mqtt.loop();
    mqtt.publish("picomqtt/welcome", "Hello from PicoMQTT!");
    
    delay(5000); // Verificar cada 5 segundos
}
