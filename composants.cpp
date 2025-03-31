#include "main.h"

LiquidCrystal_I2C liquidcrystaldisplay(0x27, 16, 2);

RFIDManager::RFIDManager() : mfrc522(0x28) { }
YELLOWLEDManager::YELLOWLEDManager(int pin) : _pin(pin) { }
NEOPIXELManager::NEOPIXELManager(int pin) :  _pin(pin), _strip(4, pin, NEO_GRB + NEO_KHZ800) { }
SERVOManager::SERVOManager(int pin) : _pin(pin) { }
XHTManager::XHTManager(int pin) : _pin(pin) { }
GazPirSteamManager::GazPirSteamManager(int pin) : _pin(pin) { }
BUTTONManager::BUTTONManager(int pin) : _pin(pin) { }

void LCDManager::setup() {
  liquidcrystaldisplay.init(); // Initialisation de l'écran LCD
}
// Définit le texte à afficher sur l'écran LCD à la position spécifiée
void LCDManager::set(String val, int x, int y) {
  liquidcrystaldisplay.setCursor(x, y); // Déplacement du curseur à la position spécifiée (x, y)
  liquidcrystaldisplay.print(val); // Affichage de la valeur spécifiée sur l'écran LCD
}
// Efface le contenu de l'écran LCD et allume le rétro-éclairage
void LCDManager::clear() {
  liquidcrystaldisplay.backlight(); // Allumage du rétro-éclairage de l'écran LCD
  liquidcrystaldisplay.clear(); // Effacement du contenu de l'écran LCD
}

void RFIDManager::setup() {
  Wire.begin();
  mfrc522.PCD_Init();
}
// Vérifie la présence d'une nouvelle carte et lit son UID
bool RFIDManager::detectCard() {
  return mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial();
}
// Retourne l'UID de la carte détectée sous forme de chaîne de caractères
const char* RFIDManager::getCardUID() {
  static char cardUID[14]; // Pour stocker l'UID (8 octets en hexadécimal avec des espaces et un caractère de terminaison)
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    sprintf(&cardUID[i * 3], "%02X ", mfrc522.uid.uidByte[i]); // Convertit chaque octet de l'UID en hexadécimal et l'ajoute à la chaîne
  }
  return cardUID;
}

void YELLOWLEDManager::init(uint8_t mode) {
  pinMode(_pin, mode);
}
void YELLOWLEDManager::set(int val) {
  digitalWrite(_pin, val);
}

// Initialise les LEDs NeoPixel
void NEOPIXELManager::setup() {
  _strip.begin(); // Initialise la communication avec les LEDs NeoPixel
  _strip.show();  // Met à jour les LEDs avec les données actuelles
  #if defined(__AVR_ATtiny85__) && (F_CPU == 16000000)
    clock_prescale_set(clock_div_1); // Configure la division d'horloge à 1 pour ATtiny85 à 16 MHz
  #endif
}
// Éteint les LEDs NeoPixel
void NEOPIXELManager::turnOff() {
  _strip.clear();
  _strip.show();
}
// Définit la luminosité des LEDs NeoPixel
void NEOPIXELManager::setBrightness(uint8_t brightness) {
  _strip.setBrightness(brightness);
}
// Définit la couleur des LEDs NeoPixel
void NEOPIXELManager::setColor(uint32_t color) {
  for(int i = 0; i < _strip.numPixels(); i++) {
    _strip.setPixelColor(i, color);
  }
  _strip.show();
}

void SERVOManager::init() {
  myservo.attach(_pin);
}
void SERVOManager::move(int freq) {
  myservo.write(freq);
}

// Lit la température du capteur XHT11
int XHTManager::readTemperature() {
  xht11 xht(_pin); // Crée une instance de la classe xht11 en utilisant le pin associé au capteur
  unsigned char dht[4] = {0, 0, 0, 0}; // Tableau pour stocker les données lues du capteur (température et humidité)
  if (xht.receive(dht)) { // Lit les données du capteur XHT11
    return dht[2]; // Si la lecture est réussie, retourne la température (stockée dans dht[2])
  } else {
    Serial.println("[XHT11] Erreur temperature !"); // Si la lecture échoue, affiche un message d'erreur sur le moniteur série
    return 0; // Retourne 0 pour indiquer une erreur de lecture
  }
}
// Lit l'humidité du capteur XHT11 et retourne la valeur en cas de succès, sinon retourne 0
int XHTManager::readHumidity() {
  xht11 xht(_pin);
  unsigned char dht[4] = {0, 0, 0, 0};
  if (xht.receive(dht)) {
    return dht[0];
  } else {
    Serial.println("[XHT11] Erreur humidity !");
    return 0;
  }
}

int GazPirSteamManager::init(uint8_t mode) {
  pinMode(_pin, mode);
  return 0;
}
// Lit l'état du capteur de gaz (HIGH ou LOW)
int GazPirSteamManager::readGas() {
  return digitalRead(_pin);
}
// Lit l'état du capteur pyroelectrique (HIGH ou LOW)
int GazPirSteamManager::readPyroelectric() {
  return digitalRead(_pin);
}
// Lit la valeur analogique du capteur de vapeur
int GazPirSteamManager::readSteam() {
  return analogRead(_pin);
}

void BUTTONManager::init() {
  pinMode(_pin, INPUT);
}
boolean BUTTONManager::value() {
  return digitalRead(_pin);
}