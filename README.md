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

## Licence
Ce projet est sous GPL :

  * this program is free software you can redistribute it and/or modify
  * it under the terms of the GNU General Public License as published by
  * the Free Software Foundation either version 3 of the License, or
  * (at your option) any later version.
  *
  * This program is distributed in the hope that it will be useful, but
  * WITHOUT ANY WARRANTY, without even the implied warranty of MERCHANTABILITY
  * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
  * for more details.
  *
  * You should have received a copy of the GNU General Public License along
  * with this program. If not, see <http://www.gnu.org/licenses/>.
  
## Credits

 OTAWebUpdater.ino Example from ArduinoOTA Library
 
 Rui Santos 
 
 Complete Project Details https://randomnerdtutorials.com
