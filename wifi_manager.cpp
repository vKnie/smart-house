#include "main.h"

WiFiManager::WiFiManager(const char* ssid, const char* password) { // Définition du constructeur
  _ssid = ssid; // Initialisation du nom du réseau WiFi
  _password = password; // Initialisation du mot de passe WiFi
}

void WiFiManager::setup() { // Définition de la fonction d'initialisation du WiFi
  WiFi.begin(_ssid, _password); // Connexion au réseau WiFi avec le nom et le mot de passe fournis
  Serial.print("[WiFi] Connection au WiFi..."); // Affichage du message de connexion au WiFi
  while (WiFi.status() != WL_CONNECTED) { // Attente de la connexion au WiFi
    delay(1000); // Pause d'une seconde
    Serial.print("."); // Affichage d'un point pour indiquer la progression de la connexion
  }
  Serial.println(" "); // Saut de ligne après la boucle d'attente
  Serial.println("[WiFi] Connectee au WiFi"); // Affichage du message de connexion réussie au WiFi
}

IPAddress WiFiManager::getLocalIP() { // Définition de la fonction pour obtenir l'adresse IP locale
  return WiFi.localIP(); // Renvoie l'adresse IP locale obtenue par le module WiFi
}
