# Sistema de Comunicación MQTT para Robots ESP32

Este proyecto implementa un sistema de comunicación MQTT para controlar múltiples robots ESP32 desde una computadora que actúa como broker MQTT.

## Arquitectura del Sistema

### Componentes:
1. **Computadora (Servidor)**: Actúa como punto de acceso WiFi y broker MQTT
2. **Robots ESP32**: Clientes que se conectan al broker para recibir comandos y enviar estados

### Flujo de Comunicación:
```
[Computadora] <-- WiFi --> [ESP32 Robot 1]
    |                       [ESP32 Robot 2]
    |                       [ESP32 Robot N]
    +-- Broker MQTT (Mosquitto)
```

## Configuración del Servidor (Computadora)

### Paso 1: Crear Red WiFi Local

**Concepto**: La computadora debe crear o conectarse a una red WiFi donde los robots puedan conectarse. Puede ser:
- Un punto de acceso (hotspot) creado por la computadora
- Un router WiFi existente
- Una red ad-hoc

**Requisitos**:
- Definir credenciales de red (SSID y contraseña)
- Asignar una IP fija a la computadora dentro de la red
- La computadora actuará como gateway de la red

**Ejemplo de configuración de red**:
```
Red WiFi: "Wi-FiUnamBots"
Contraseña: "fiounambots"
IP del servidor: 10.0.0.100
Máscara de subred: 255.0.0.0
Gateway: 10.0.0.100 (misma IP del servidor)
```

### Paso 2: Instalar y Configurar Broker MQTT (Mosquitto)

**Concepto**: El broker MQTT es el intermediario que recibe y distribuye mensajes entre el servidor y los robots.

#### En Linux (Ubuntu/Debian/Arch):
```bash
# Instalar mosquitto
sudo apt install mosquitto mosquitto-clients  # Ubuntu/Debian
sudo pacman -S mosquitto                      # Arch Linux

# Crear usuario y contraseña
sudo mosquitto_passwd -c /etc/mosquitto/passwd user1
# Ingresar contraseña cuando se solicite (ej: 1234)
```

#### En Windows:
1. Descargar Mosquitto desde: https://mosquitto.org/download/
2. Instalar el ejecutable
3. Crear archivo de contraseñas usando `mosquitto_passwd.exe`

### Paso 3: Configurar Mosquitto

**Concepto**: Configurar el broker para que:
- Escuche solo en la IP de la red local (seguridad)
- Requiera autenticación de usuarios
- Permita conexiones desde los robots

**Archivo de configuración** (`/etc/mosquitto/mosquitto.conf` o `mosquitto.conf`):
```conf
# Escuchar solo en la IP fija del servidor
listener 1883 10.0.0.100

# Requerir autenticación
allow_anonymous false
password_file /etc/mosquitto/passwd
```

**Iniciar el broker**:
```bash
# Linux (systemd)
sudo systemctl start mosquitto
sudo systemctl enable mosquitto

# Manual (cualquier SO)
mosquitto -c /ruta/a/mosquitto.conf
```

### Paso 4: Verificar Funcionamiento del Broker

**Concepto**: Probar que el broker está funcionando correctamente antes de conectar los robots.

```bash
# Terminal 1: Suscribirse a un tópico de prueba
mosquitto_sub -h 10.0.0.100 -t "test" -u user1 -P 1234

# Terminal 2: Publicar en el mismo tópico
mosquitto_pub -h 10.0.0.100 -t "test" -m "Hola mundo" -u user1 -P 1234
```

Si ves el mensaje "Hola mundo" en la Terminal 1, ¡el broker está funcionando!

## Configuración de los Robots ESP32

### Paso 1: Configurar Credenciales en el Código

Editar el archivo `src/main.cpp`:

```cpp
// Configuración de WiFi
const char* ssid = "Wi-FiUnamBots";      // Nombre de tu red WiFi
const char* password = "fiounambots";     // Contraseña de tu red

// ID único para cada robot (CAMBIAR antes de subir a cada robot)
int id_robot = 1;  // Robot 1: id=1, Robot 2: id=2, etc.

// IP del broker MQTT (IP fija de la computadora)
PicoMQTT::Client mqtt(
  "10.0.0.100",   // IP del servidor/broker
  1883,           // Puerto MQTT
  "Robot1",       // Client ID (opcional: cambiar por robot)
  "user1",        // Usuario creado en Mosquitto
  "1234"          // Contraseña del usuario
);
```

### Paso 2: Compilar y Subir el Código

**Concepto**: El código se compila y se sube a cada ESP32. Antes de subir a cada robot, cambiar el `id_robot`.

```bash
# Compilar para ESP32-C3
platformio run -e esp32-c3-devkitm-1

# Subir al ESP32 conectado
platformio run -e esp32-c3-devkitm-1 --target upload
```

**Para otros boards**:
- ESP32 normal: `platformio run -e esp32dev`
- ESP32-CAM: `platformio run -e esp32cam`

### Paso 3: Asignación de IPs

**Concepto**: Cada robot obtiene una IP única basada en su ID, evitando conflictos.

```
Robot con id_robot = 1 → IP: 10.0.0.201
Robot con id_robot = 2 → IP: 10.0.0.202
Robot con id_robot = 3 → IP: 10.0.0.203
...
```

Esto se hace automáticamente en el código:
```cpp
IPAddress local_IP(10, 0, 0, 200 + id_robot);
```

## Uso del Sistema

### Tópicos MQTT

Cada robot tiene tópicos específicos basados en su ID:

**Tópicos de entrada (comandos al robot)**:
- `robot/1/action` - Comandos para el robot 1
- `robot/2/action` - Comandos para el robot 2
- `robot/N/action` - Comandos para el robot N

**Tópicos de salida (estado del robot)**:
- `status` - Estado de conexión del robot

## Comandos Disponibles

Los comandos actuales implementados son:
- `Forward` - Mover motores hacia adelante (velocidad 255)
- `Off` - Detener motores (velocidad 0)

**Agregar más comandos**: Editar la función callback en `setup()`:

```cpp
mqtt.subscribe(topic, [](const char * topic, const char * payload) {
    String command = String(payload);
    if (command == "Off"){
        moveMotors(0, 0);
    } else if (command == "Forward") {
        moveMotors(255, 255);
    } else if (command == "Backward") {  // Nuevo comando
        moveMotors(-255, -255);
    }
    // Agregar más comandos aquí...
});
```

## Solución de Problemas

### El robot no se conecta al WiFi
- Verificar que las credenciales (SSID y contraseña) sean correctas
- Verificar que el robot esté en rango de la red WiFi
- Verificar que la IP del robot no esté en uso por otro dispositivo

### El robot no se conecta al broker MQTT
- Verificar que el broker esté ejecutándose: `sudo systemctl status mosquitto`
- Verificar que la IP del broker sea correcta en el código
- Verificar usuario y contraseña en el código
- Verificar que el broker esté escuchando en la IP correcta

### Los comandos no llegan al robot
- Verificar que el robot esté publicando en el tópico `status` (indica que está conectado)
- Verificar que el tópico usado al publicar coincida con el ID del robot
- Agregar un `delay()` en el loop si es necesario para dar tiempo a procesar mensajes

## Estructura del Proyecto

```
robot_comunication/
├── platformio.ini          # Configuración de boards y librerías
├── src/
│   ├── main.cpp           # Código principal de conexión y MQTT
│   └── movement.cpp       # Control de motores
├── include/
│   └── movement.h         # Declaraciones para control de motores
└── README.md              # Este archivo
```

## Dependencias

- **PlatformIO**: Plataforma de desarrollo
- **Framework Arduino**: Para ESP32
- **PicoMQTT**: Librería MQTT ligera para ESP32
- **WiFi**: Librería de conectividad (incluida en Arduino ESP32)

## Licencia

Este proyecto es de código abierto y puede ser modificado libremente.