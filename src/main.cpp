#include <WiFi.h>
#include <PicoMQTT.h>
#include <movement.h>

const char* ssid = "Wi-FiUnamBots";
const char* password = "fiounambots";

// Cambia este ID antes de subir el código a cada robot
int id_robot = 2;

// Variable para controlar el envío del mensaje inicial
bool mensaje_inicial_enviado = false;

// Configure static IP to be on the same network as the broker
IPAddress local_IP(10, 0, 0, 200 + id_robot);    // IP different from broker
IPAddress gateway(10, 0, 0, 100);                // Gateway
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
    // Init motors (solo una vez al inicio)
    setupMotors();
    
    // Conectar al WiFi
    conectarWiFi();

    mqtt.begin();

    // Crear el topic dinámico basado en el ID del robot
    char topic[32];
    snprintf(topic, sizeof(topic), "robot/%d/action", id_robot);
    
    // Subscribe to a topic pattern and attach a callback
    mqtt.subscribe(topic, [](const char * topic, const char * payload) {
        String command = String(payload);
        if (command == "Off"){
            moveMotors(0, 0);
        } else if (command == "Forward") {
            moveMotors(255, 255);
        }
    });
}

void loop(){
    // Verificar conexión WiFi cada ciclo
    if (WiFi.status() != WL_CONNECTED) {
        conectarWiFi();
        mensaje_inicial_enviado = false; // Resetear si se pierde la conexión
    }

    mqtt.loop();
    
    // Enviar mensaje "ready" solo una vez cuando se conecta al broker
    if (mqtt.connected() && !mensaje_inicial_enviado) {
        char topic[32];
        snprintf(topic, sizeof(topic), "robot/%d/status", id_robot);
        mqtt.publish(topic, "ready");
        mensaje_inicial_enviado = true;
    }
    
    delay(200); // Dar tiempo al sistema para procesar mensajes
}
