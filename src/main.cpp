/* Projet ESP32_Sensor_Temp_V2
  *
  * @author 2020 Jean-Louis Frucot <frucot.jeanlouis@free.fr>
  * @see The GNU Public License (GPL)
  *
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
  */

 /* Crédits Remerciements
  OTA
    OTAWebUpdater.ino Example from ArduinoOTA Library
    Rui Santos 
    Complete Project Details https://randomnerdtutorials.com
 */

#include <Arduino.h>

#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <ArduinoOTA.h>
#include <ESPmDNS.h>
#include <WebServer.h>
#include <Update.h>
#include <DHTesp.h>
//////////////////////////  Mes libs perso ////////////////////////////////////
#include "JLF_Timer.h"
#include "JLF_Temp_Device.h"
#include "data.h"

WebServer server(8888); // Initialise et lance le serveur web utilisé pour OTA
#include "JLF_ota.h"    // Pour déporter les "pages html" du server OTA

//////////////////////////  Déclaration de certaines constantes ///////////////

#define OUTPUT_SERIAL_PRINT 1 // 0 pas de sortie sur le moniteur série, 1 sortie sur le moniteur série

// Paramètres WIFI
const char *SSID = NetworkName;           // Nom du réseau WIFI
const char *PASSWORD = NetworkPassword; // Mot de passe du réseau WIFI
const char RED[] = "off";
const char GREEN[] = "on";
const char ORANGE[] = "stay";
const char INACTIF[] = "inactif";
const char *HOST = "esp32-01";

// Les broches utilisées par les leds
const int GREEN_PIN = 19;
const int ORANGE_PIN = 21;
const int RED_PIN = 22;

// Allume une led et éteint les autres (0 pour tout éteindre)
void allume_led(const int led)
{
  digitalWrite(GREEN_PIN, LOW);
  digitalWrite(ORANGE_PIN, LOW);
  digitalWrite(RED_PIN, LOW);
  if (led)
  {
    digitalWrite(led, HIGH);
  }
}

#define DHTPIN 25             // Broche à laquelle le DHT sensor est connecté
#define DHTTYPE DHTesp::DHT22 // DHT 22 (AM2302)

//////////////////////////////////////////////// Broker mqtt  ///////////////////////////
const char *MQTT_SERVEUR = "192.168.1.101";
#define MQTT_PORT 1883
#define MQTT_USER MqttUser
#define MQTT_PASSWORD MqttPassword
// Les topics
const char MQTT_SERIAL_PUBLISH_CH[] = "sensors/office/temperature/01";
const char MQTT_SERIAL_RECEIVER_CH[] = "sensors/office/temperature/01_RX";
const char MQTT_CLIENT_ID[] = "ESP32Client-Device01";

// Déclaration d'un client pour le WIFI
WiFiClient wifiClient;
void setup_wifi();

// Création d'un client MQTT
PubSubClient MQTTClient(wifiClient);
void publishData(float temp);
void MQTTReceiver(char *topic, byte *payload, unsigned int length);
void connectMQTT();
StaticJsonDocument<200> jsonDoc; // Un buffer pour les publications MQTT

// Création d'un capteur de température
JLF_Temp_Device *tempDevice = new JLF_Temp_Device(DHTPIN, DHTTYPE); // Initialisation du capteur

////////////////////////////////////   Variables pour compter le temps   ////////////////////////////

JLF_Timer *readTempTimer = new JLF_Timer(3000);     // Création d'un compteur pour lire la température
JLF_Timer *publishTimer = new JLF_Timer(30 * 1000); // Création d'un compteur pour rythmer les publications vers le broker mqtt

/////////////////////////////////////   Setup   /////////////////////////////////////////////////

void setup()
{
  // Initialisation des broches leds et DHT22
  pinMode(DHTPIN, INPUT);

  pinMode(RED_PIN, OUTPUT);
  pinMode(ORANGE_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);

  // Initialisation liaison série
  Serial.begin(9600);
  Serial.setTimeout(500);

  // Initialisaiton WIFI
  setup_wifi();

  // Initialisation MQTT
  MQTTClient.setServer(MQTT_SERVEUR, MQTT_PORT);
  MQTTClient.setCallback(MQTTReceiver); // Pour le traitement des messages reçus
  connectMQTT();

  // Service mDNS pour la résolution de nom
  if (!MDNS.begin(HOST))
  {
#if OUTPUT_SERIAL_PRINT
    Serial.println("Error setting up MDNS responder!");
#endif
    while (1)
    {
      delay(1000);
    }
  }
#if OUTPUT_SERIAL_PRINT
  Serial.println("mDNS responder started");
#endif
  //////////////////////////// OTA
  OTAServerInit(); // Initialise et lance le serveur web pour OTA (voir JLF_OTA.h)

  // init jsonDoc
  jsonDoc["temperature"] = 0.0;

  // On démarre les compteurs de temps
  readTempTimer->start();
  publishTimer->start();
}

/////////////////////////////////////   Loop   /////////////////////////////////////////////////

void loop()
{
  // On teste la connexion Wifi
  if (WiFi.status() == WL_CONNECTED)
  {
    // Si le WIFI est Ok, on teste la connexion au serveur MQTT
    if (!MQTTClient.connected())
    {
      connectMQTT();
    }
  }
  else
  {
    // On reconnecte le Wifi
    setup_wifi();
  }

  MQTTClient.loop(); // Permet l'écoute du broker MQTT

  server.handleClient(); // Permet l'écoute du serveur http pour OTA

  // Publication de la température vers le serveur MQTT
  if (publishTimer->isTimeElapsed())
  {
    float temp = tempDevice->getAverageTemp();
    publishData(temp); // Publication de la température
#if OUTPUT_SERIAL_PRINT
    Serial.print("30 s écoulées   température : ");
    Serial.println(temp);
#endif
  }

  // Lecture périodique de la température
  if (readTempTimer->isTimeElapsed())
  {
    tempDevice->update();
  }
}
///////////////////////////////////////////////  Routines /////////////////////

/////// WIFI /////////////////
void setup_wifi()
{
  // Juste pour moi ip 192.168.1.180 attribuée par le routeur
  delay(10);
  // On essaie de se connecter au WIFI

#if OUTPUT_SERIAL_PRINT
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(SSID);
#endif
  WiFi.begin(SSID, PASSWORD);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
#if OUTPUT_SERIAL_PRINT
    Serial.print(".");
#endif
  }
#if OUTPUT_SERIAL_PRINT
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
#endif
}

/////// MQTT   //////////////

void connectMQTT()
{
  // On boucle tant qu'on n'est pas connecté
  while (!MQTTClient.connected())
  {
    Serial.print("Attente de la connexion MQTT ...");
    if (MQTTClient.connect(MQTT_CLIENT_ID, MQTT_USER, MQTT_PASSWORD))
    {
#if OUTPUT_SERIAL_PRINT
      Serial.println("connecté");
#endif
      MQTTClient.subscribe(MQTT_SERIAL_RECEIVER_CH);
    }
    else
    {
#if OUTPUT_SERIAL_PRINT
      Serial.print("erreur, rc=");
      Serial.print(MQTTClient.state());
      Serial.println(" On recommence dans 5 secondes");
#endif
      // On attend 5 secondes avant de réessayer
      delay(5000);
    }
  }
}

void publishData(float temp)
{
  jsonDoc.clear();
  jsonDoc["temperature"] = temp;

  if (!MQTTClient.connected())
  {
    connectMQTT();
  }
  char data[200]; // Pour accueillir les données du document json
  serializeJson(jsonDoc, data);
#if OUTPUT_SERIAL_PRINT
  Serial.print("Envoi mqtt : ");
  Serial.println(MQTTClient.publish(MQTT_SERIAL_PUBLISH_CH, data, true));
  Serial.println(data);
#else
  client.publish(MQTT_SERIAL_PUBLISH_CH, data, true);
#endif

  yield();
}

void MQTTReceiver(char *topic, byte *payload, unsigned int length)
{
  // Il est nécessaire de copier topic et payload car les buffer
  // peuvent être réutilisés lors d'une éventuelle publication concurrente
  // Allocation de mémoire
  byte *p = (byte *)malloc(length);
  // Copie du payload dans le buffer *p
  memcpy(p, payload, length);

  // Copie la chaine de caractères du Topic Mqtt
  int size = strlen(topic);
  byte *t = (byte *)malloc(size);
  memcpy(t, topic, size);
  // Maintenant que tout est copié, on peut publier...
  // MQTTClient.publish("sensors/office/temperature/01_debug", p, length, true);
#if OUTPUT_SERIAL_PRINT
  Serial.println("-------new message from broker-----");
  Serial.print("channel:");
  Serial.write(t, size);
  Serial.println();
  Serial.print("data:");
  Serial.write(p, length);
  Serial.println();
#endif

  if (!strncmp((char *)t, MQTT_SERIAL_RECEIVER_CH, strlen(MQTT_SERIAL_RECEIVER_CH))) // On utilise strncmp car une des chaines n'est pas "null terminated" (t)
  {
    DeserializationError error = deserializeJson(jsonDoc, p);
    if (error.code() != DeserializationError::Ok)
    {
#if OUTPUT_SERIAL_PRINT
      switch (error.code())
      {
      case DeserializationError::Ok:
        Serial.println(F("Deserialization réussie !"));
        break;
      case DeserializationError::InvalidInput:
        Serial.println(F("Entrée invalide !"));
        break;
      case DeserializationError::NoMemory:
        Serial.println(F("Pas assez de mémoire !"));
        break;
      default:
        Serial.println(F("Deserialization ratée !"));
        break;
      }
#endif
    }
    else
    {
      // On analyse le msg.payload pour savoir quelle action exécuter
      const char *etat = jsonDoc["state"];
#if OUTPUT_SERIAL_PRINT
      Serial.print("status ");
      Serial.println(etat);
#endif
      // On vérifie qu'il y a bien une clé "state"
      JsonObject obj = jsonDoc.as<JsonObject>();
      if (obj.containsKey("state"))
      {
        // On cherche ce qu'il faut faire
        if (!strcmp(etat, RED))
        {
          // On allume la lampe rouge : la prise est en mode Off
          allume_led(RED_PIN);
        }
        else if (!strcmp(etat, GREEN))
        {
          // On allume la lampe verte : la prise est en mode off
          allume_led(GREEN_PIN);
        }
        else if (!strcmp(etat, ORANGE))
        {
          // On allume la lampe orange : on est dans la zone de comfort (on ou off)
          allume_led(ORANGE_PIN);
        }
        else if (!strcmp(etat, INACTIF))
        {
          // tout est éteint : la régulation est inactivée
          allume_led(0);
        }
      }
      else
      {
#if OUTPUT_SERIAL_PRINT
        Serial.println("Pas de \"state\" identifié");
#endif
      }
    }
  }
  free(p);
  free(t);
}
