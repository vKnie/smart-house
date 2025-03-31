#include "main.h" // Inclusion du fichier d'en-tête contenant les définitions de la classe JSONManager

// Définition de la fonction pour créer un document JSON
String JSONManager::createJSON(const char* name_value, unsigned char value, const char* sensor, String date) {
  const size_t capacity = JSON_OBJECT_SIZE(4); // Définition de la taille maximale du document JSON
  DynamicJsonDocument doc(capacity); // Création d'un document JSON dynamique avec la capacité spécifiée
  // Ajout des données au document JSON
  doc["name_value"] = name_value; // Ajout du nom de la mesure
  doc["value"] = value; // Ajout de la valeur mesurée
  doc["sensor"] = sensor; // Ajout du type de capteur
  doc["date"] = date; // Ajout de la date de la mesure
  String jsonString; // Déclaration d'une chaîne de caractères pour stocker le JSON
  // Sérialisation du document JSON en une chaîne JSON
  serializeJson(doc, jsonString); // Cette fonction prend le document JSON et le convertit en une chaîne JSON
  return jsonString; // Renvoie la chaîne JSON créée
}

// Fonction pour extraire les messages pour l'écran LCD à partir d'un document JSON
String* JSONManager::getMessageFromJsonLCD(const char* jsonString) {
  StaticJsonDocument<200> doc; // Crée un document JSON statique avec une taille fixe
  DeserializationError error = deserializeJson(doc, jsonString); // Désérialise le message JSON
  // Vérifie les erreurs de désérialisation
  if (error) {
    Serial.print("deserializeJson() failed: ");
    Serial.println(error.c_str());
    return nullptr; // Retourne nullptr en cas d'erreur
  }
  // Extrait les valeurs des champs "message1" et "message2"
  const char* message1 = doc["message1"];
  const char* message2 = doc["message2"];
  // Crée un tableau dynamique pour stocker les messages
  String* messages = new String[2];
  messages[0] = message1;
  messages[1] = message2;
  // Retourne le tableau contenant les messages
  return messages;
}

// Fonction pour vérifier si une chaîne est un JSON valide
bool JSONManager::isJson(const char* message) {
  StaticJsonDocument<200> doc; // Crée un document JSON statique avec une taille fixe
  DeserializationError error = deserializeJson(doc, message); // Désérialise le message JSON
  return !error; // Retourne true si la désérialisation est réussie, false sinon
}
