# Sensor_Temp_V2
ESP32 and DH22 for Home Assistant

- Mesure de la température ambiante via DHT22
- Transmission via MQTT
- Allumage de Leds en fonction du message MQTT reçu (vert : froid, orange : comfort, rouge : chaud)
- Mise à jour du firmware via le mécanisme OTA (On The Air)


## Installation
créer un fichier src/data.h avec le contenu suivant :

 - #define NetworkPassword "My_Password"
 - #define NetworkName "My_networkName"
 - #define MqttUser "My_User"
 - #define MqttPassword "My_MQTT_Password"

en remplaçant ce qui est entre " " par les bonnes valeurs.
