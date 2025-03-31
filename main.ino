#include "main.h"
#include <PubSubClient.h>

const char* ssid = "TP-LINK-MAISON"; // Définition du nom du réseau WiFi
const char* password = "maison123"; // Définition du mot de passe WiFi

const char* mqtt_server = "10.194.177.44"; // Définition de l'adresse IP ou du nom de domaine du serveur MQTT
const int mqtt_port = 1883; // Définition du port du serveur MQTT
const char* mqtt_user = "maison"; // Définition du nom d'utilisateur MQTT
const char* mqtt_password = "123azerty"; // Définition du mot de passe MQTT

const char* ntpServer = "europe.pool.ntp.org";
const long  gmtOffset_sec = 3600;
const int   daylightOffset_sec = 3600;

const int PIN_XHT_SENSOR = 17;
const int PIN_GAS_SENSOR = 23;
const int PIN_PIR_SENSOR = 14;
const int PIN_LED_YELLOW = 12;
const int PIN_STEAM_SENSOR = 34;
const int PIN_BUZZER = 25;
const int PIN_BUTTON_RIGHT = 27;
const int PIN_BUTTON_LEFT = 16;
const int PIN_SERVO_WINDOW = 5;
const int PIN_SERVO_DOOR = 13;
const int PIN_NEOPIXEL = 26;

unsigned long previousMillis = 0;  // Stocke le dernier temps où une action a été effectuée
const long delayInterval = 1000;    // Intervalle de délai (500 ms)


WiFiManager wifi(ssid, password);

WiFiClient espMaison;
PubSubClient client(espMaison);

JSONManager json;
LCDManager lcd;
RFIDManager rfid;
NEOPIXELManager neopixel(PIN_NEOPIXEL);
YELLOWLEDManager yellowled(PIN_LED_YELLOW);
SERVOManager servoWindow(PIN_SERVO_WINDOW);
SERVOManager servoDoor(PIN_SERVO_DOOR);
XHTManager xht(PIN_XHT_SENSOR);
GazPirSteamManager gaz(PIN_GAS_SENSOR);
GazPirSteamManager pir(PIN_PIR_SENSOR);
GazPirSteamManager steam(PIN_STEAM_SENSOR);
BUTTONManager btnRight(PIN_BUTTON_RIGHT);
BUTTONManager btnLeft(PIN_BUTTON_LEFT);

void reconnect() {
  // Boucle tant que le client MQTT n'est pas connecté
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection..."); // Message de tentative de connexion MQTT
    // Tentative de connexion au serveur MQTT avec les identifiants spécifiés
    if (client.connect("ESP32Client", mqtt_user, mqtt_password)) {

      Serial.println("connected"); // Affichage de la réussite de la connexion sur le moniteur série

      // Abonnement aux différents topics MQTT pour recevoir les messages
      client.subscribe("temperature");
      client.subscribe("humidity");
      client.subscribe("lcd");
      client.subscribe("lcd_status");
      client.subscribe("gas");
      client.subscribe("steam");
      client.subscribe("pir");
      client.subscribe("rfid");
      client.subscribe("door");
      client.subscribe("window");
      client.subscribe("bouton");

    } else {
      // Affichage de l'échec de la connexion avec le code de retour
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds"); // Message pour réessayer la connexion après un délai de 5 secondes
      delay(5000); // Attente de 5 secondes avant de retenter la connexion
    }
  }
}

void setup() {

  Serial.begin(9600);
  wifi.setup();
  Serial.print("[WiFi] Adresse IP locale: ");
  // Affichage de l'adresse IP locale obtenue sur le moniteur série
  Serial.println(wifi.getLocalIP());
  
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);

  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);

  lcd.setup();
  lcd.clear();

  rfid.setup();

  pir.init(INPUT);
  steam.init(INPUT);
  gaz.init(INPUT);

  servoDoor.init();
  servoWindow.init();

  btnLeft.init();
  btnRight.init();

  //neopixel.setup();
}

void loop() {

  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= delayInterval) {
    previousMillis = currentMillis;

    if (btnRight.value() == 0) {
      client.publish("bouton", "bouton_right_detection");
    }
    if (btnLeft.value() == 0) {
      client.publish("bouton", "bouton_left_detection");
    }

    //if (pirSensor.readPyroelectric() == 1) {
      //neopixel.setColorNeoPixel(0xFFFFFF);
      //client.publish("pir", "pir_detection");
    //}

    if (gaz.readGas() == 0) {
      //neopixel.setColorNeoPixel(0xFF0000);
      servoWindow.move(180);
      client.publish("gas", "gas_detection");
    }

    if (steam.readSteam() >= 500) {
      //neopixel.setColorNeoPixel(0x0000FF);
      servoWindow.move(-180);
      client.publish("steam", "steam_detection");
    }

    // Vérifie si une carte RFID est détectée
    if(rfid.detectCard()) {
      // Si oui, obtient l'UID de la carte et le publie via MQTT, puis attend pour rechercher la carte suivante
      const char* cardUIDStr = rfid.getCardUID();
      client.publish("rfid", cardUIDStr);
    }
  }

  if (!client.connected()) {
    reconnect();
  }
  client.loop(); // Gère les messages MQTT entrants et sortants
}

String getDateTime() {
  struct tm timeinfo; // Structure pour stocker les informations sur le temps
  // Obtenir l'heure locale et la stocker dans la structure timeinfo
  if (!getLocalTime(&timeinfo)) {
    return String("Erreur Date"); // Retourner une chaîne de caractères "Erreur Date" en cas d'échec de l'obtention de l'heure locale
  } else {
    char buffer[20]; // Déclaration d'un tampon pour stocker la date et l'heure formatées
    // Formater la date et l'heure dans le tampon selon le format spécifié (%Y-%m-%d %H:%M:%S)
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", &timeinfo);
    return String(buffer); // Retourner la date et l'heure formatées sous forme de chaîne de caractères
  }
}

void callback(char* topic, byte* payload, unsigned int length) {
  
  // Conversion du payload en une chaîne de caractères
  String message = "";
  for (int i = 0; i < length; i++) {
    message += (char)payload[i]; // Ajout de chaque caractère du payload à la chaîne message
  }

  // Création d'un message de log pour afficher le topic et le message reçus via MQTT
  String log_mqtt = "[MQTT] Message reçu: (topic: " + String(topic) + " ; message: " + message + ")";
  Serial.println(log_mqtt); // Affichage du message reçu dans le moniteur série

  yellowled.set(HIGH);
  yellowled.set(LOW);

  // Gestion des messages en fonction du topic
  if (strcmp(topic, "temperature") == 0) { // Si le topic est "temperature"
    if (message.equals("getTemperature")) { // Et le message est "gettemperature"
      // Création d'un message JSON avec les données de température
      String json_message_temperature = json.createJSON("temperature", xht.readTemperature(), "XHT11", getDateTime());
      const char* json_message_temperature_char = json_message_temperature.c_str();
      client.publish("temperature", json_message_temperature_char); // Publication du message JSON sur le topic "temperature"
      return; // Sortie de la fonction
    }
  }

  if (strcmp(topic, "humidity") == 0) { // Si le topic est "humidity"
    if (message.equals("gethumidity")) { // Et le message est "gethumidity"
      // Création d'un message JSON avec les données d'humidité
      String json_message_humidity = json.createJSON("humidity", xht.readHumidity(), "XHT11", getDateTime());
      const char* json_message_humidity_char = json_message_humidity.c_str();
      client.publish("humidity", json_message_humidity_char); // Publication du message JSON sur le topic "humidity"
      return; // Sortie de la fonction
    }
  }

  if (strcmp(topic, "lcd_status") == 0) { // Si le topic est "lcd_status"
    if (message.equals("set_lcd")) { // Et le message est "set_lcd"
      lcd.clear(); // Effacement du contenu de l'écran LCD
      String temp_lcd = "Temperature: " + String(xht.readTemperature());
      lcd.set(temp_lcd, 0, 0);
      String humi_lcd = "Humidite: " + String(xht.readHumidity());
      lcd.set(humi_lcd, 0, 1);
      return; // Sortie de la fonction
    }
  }

  if (strcmp(topic, "door") == 0) { // Si le topic est "door"
    if (message.equals("close_door")) { // Et le message est "close_door"
      servoDoor.move(-180); // Fermeture de la porte
      client.publish("door", "[ESP32] Porte 'CLOSE'"); // Publication d'un message de confirmation sur le topic "door"
      return; // Sortie de la fonction
    } else if (message.equals("open_door")) { // Si le message est "open_door"
      servoDoor.move(180); // Ouverture de la porte
      client.publish("door", "[ESP32] Porte 'OPEN'"); // Publication d'un message de confirmation sur le topic "door"
      return; // Sortie de la fonction
    }
  }

  if (strcmp(topic, "window") == 0) { // Si le topic est "window"
    if (message.equals("close_window")) { // Et le message est "close_window"
      servoWindow.move(-180); // Fermeture de la fenêtre
      client.publish("window", "[ESP32] Fenêtre 'CLOSE'"); // Publication d'un message de confirmation sur le topic "window"
      return; // Sortie de la fonction
    } else if (message.equals("open_window")) { // Si le message est "open_window"
      servoWindow.move(180); // Ouverture de la fenêtre
      client.publish("window", "[ESP32] Fenêtre 'OPEN'"); // Publication d'un message de confirmation sur le topic "window"
      return; // Sortie de la fonction
    }
  }

  if (strcmp(topic, "lcd") == 0) { // Si le topic est "lcd"
    const char* messageChar = message.c_str();
    if(json.isJson(messageChar)) { // Vérification que le message est bien au format JSON
      // Extraction des messages pour l'affichage sur l'écran LCD
      String* message_lcd_from_json = json.getMessageFromJsonLCD(messageChar);
      lcd.clear(); // Effacement de l'écran LCD
      lcd.set(message_lcd_from_json[0], 0, 0); // Affichage du premier message sur la première ligne de l'écran LCD
      lcd.set(message_lcd_from_json[1], 0, 1); // Affichage du deuxième message sur la deuxième ligne de l'écran LCD
    }
  }

  if (strcmp(topic, "rfid") == 0) {
    if (message.equals("rfid_ok")) {
      servoDoor.move(180);
      return; // Sortie de la fonction
    } else if (message.equals("rfid_no")) {
      return; // Sortie de la fonction
    }
  }

}