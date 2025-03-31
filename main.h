#ifndef MAIN_H
#define MAIN_H

#include <Arduino.h>
#include <ArduinoJson.h>
#include <WiFi.h>
#include <LiquidCrystal_I2C.h>
#include <MFRC522_I2C.h>
#include <Adafruit_NeoPixel.h>
#include <ESP32_Servo.h>
#include "xht11.h"
#include <Wire.h>
#ifdef __AVR__
 #include <avr/power.h> // Required for 16 MHz Adafruit Trinket
#endif

class WiFiManager {
public:
  WiFiManager(const char* ssid, const char* password); // Déclaration du constructeur prenant le nom du réseau et le mot de passe WiFi
  void setup(); // Déclaration de la fonction d'initialisation du WiFi
  IPAddress getLocalIP(); // Déclaration de la fonction pour obtenir l'adresse IP locale

private:
  const char* _ssid; // Déclaration de la variable pour le nom du réseau WiFi
  const char* _password; // Déclaration de la variable pour le mot de passe WiFi
};

class JSONManager {
public:
  // Déclaration des fonctions membres
  String createJSON(const char* name_value, unsigned char value, const char* sensor, String date); // Fonction pour créer un document JSON
  String* getMessageFromJsonLCD(const char* jsonString); // Fonction pour extraire les messages pour l'écran LCD
  bool isJson(const char* message); // Fonction pour vérifier si une chaîne est un JSON valide
};

class LCDManager {
public:
  void setup(); // Déclaration de la fonction d'initialisation de l'écran LCD
  void set(String val, int x, int y); // Déclaration de la fonction pour définir le texte sur l'écran LCD
  void clear(); // Déclaration de la fonction pour effacer le contenu de l'écran LCD
};

class RFIDManager {
public:
  RFIDManager(); // Constructeur de la classe RFID
  void setup(); // Déclaration de la fonction d'initialisation du capteur RFID
  bool detectCard(); // Déclaration de la fonction pour détecter une carte RFID
  const char* getCardUID(); // Déclaration de la fonction pour obtenir l'UID de la carte RFID
private:
  MFRC522 mfrc522; // Instance du capteur RFID
};

class NEOPIXELManager {
public:
  NEOPIXELManager(int pin);
  void setup(); // Déclaration de la fonction pour initialiser les LEDs NeoPixel
  void turnOff(); // Déclaration de la fonction pour éteindre les LEDs NeoPixel
  void setBrightness(uint8_t brightness); // Déclaration de la fonction pour régler la luminosité des LEDs NeoPixel
  void setColor(uint32_t color); // Déclaration de la fonction pour définir la couleur des LEDs NeoPixel
private:
  int _pin; // Pin associé au capteur
  Adafruit_NeoPixel _strip; // Instance pour contrôler les LEDs NeoPixel
};

class YELLOWLEDManager {
public:
  YELLOWLEDManager(int pin);
  void init(uint8_t mode);
  void set(int val);
private:
  int _pin;
};

class SERVOManager {
public:
  SERVOManager(int pin);
  void init(); // Déclaration de la fonction d'initialisation du servo
  void move(int freq); // Déclaration de la fonction pour déplacer le servo
private:
  int _pin;
  Servo myservo;
};

class XHTManager {
public:
  XHTManager(int pin);
  int readTemperature(); // Déclaration de la fonction pour lire la température
  int readHumidity(); // Déclaration de la fonction pour lire l'humidité
private:
  int _pin;
};

class BUTTONManager {
public:
  BUTTONManager(int pin);
  void init();
  boolean value();
private:
  int _pin;
};

class GazPirSteamManager {
public:
  GazPirSteamManager(int pin);
  int init(uint8_t mode);
  int readGas(); // Déclaration de la fonction pour lire la présence de gaz
  int readPyroelectric(); // Déclaration de la fonction pour lire la détection pyroelectrique
  int readSteam(); // Déclaration de la fonction pour lire la vapeur
private:
  int _pin;
};

#endif