#ifndef __MAIN_H
#define __MAIN_H

#include <string>
#include <Arduino.h>
#include <ESP8266mDNS.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <PubSubClient.h>
#include <NTPClient.h>
#include <CertStoreBearSSL.h>
#include <time.h>
#include "files.h"
#include "timerTask.h"
#include "const.h"

// Item d'un champ param
struct Item {
  char item[5];
};

char date[20];

// Variables d'un cycle
int scheduleEnabled;
int scheduleH;
int scheduleM;
int minRandom_av;
int maxRandom_av;
int minRandom_ar;
int maxRandom_ar;
int reverse_cycle;
int nbCycles;
int activeTime;
int logStatus;
unsigned currentRandomValue;

int currentCycle;
boolean activeScheduledTask;
boolean direction;

// Id des tâches
task_id idRobotTask;
task_id idEndRobotTask;
task_id idScheduleCleanTask;
task_id idMonoPowerTimeOffTask;

// Buffers
// tabParam doit être de la taille de PARAM+1 
char tabParam[33];
char bufferTime[30];
char randomBuffer[12];

// Objets utilisés
FileLittleFS* fileLog;
FileLittleFS *fileParam;
Task timerTask;
WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);
NTPClient* ntpTime;
WiFiUDP ntpUDP;

void PubSubCallback(char* topic, byte* payload, unsigned int length);
void writeLogs(const char * msg);
void logsWrite(const char *msg);
void deleteLogs();
Item* split(char* str, const char* motif);
char* getDate();
void robotEndTask();

// Fonctions permettant de manipuler les items des paramètres
// Comparer deux items
inline boolean cmp(const char* s1, const char* s2) {
  return strcmp(s1, s2) == 0;
}
// Obtenir un item
inline char* get(Item* paramItem, int PARAM) {
  return (&paramItem[PARAM])->item;
}

inline void debugPrintParam() {
  Serial.println(tabParam);
  Serial.printf("scheduleEnabled  = %d\n", scheduleEnabled);
  Serial.printf("schedule time = %02d:%02d\n", scheduleH, scheduleM);
  Serial.printf("random limits = %d, %d, %d, %d\n", minRandom_av, maxRandom_av,minRandom_ar, maxRandom_ar);
  Serial.printf("Reverse = %d\n", reverse_cycle);
  Serial.printf("nbCycles = %d\n", nbCycles);
  Serial.printf("activeTime = %d mn\n", activeTime);
  Serial.printf("logStatus= %d\n", logStatus);
}

// Commande du moteur de traction
inline void powerOff() {
#ifdef POWER_DEBUG  
  Serial.println("powerOff");
#endif
  digitalWrite(GPIO0_RETURN, HIGH);
  digitalWrite(GPIO2_FORWARD, HIGH);
}

inline void robotForward() {
#ifdef POWER_DEBUG  
  Serial.println("forward");
#endif  
  digitalWrite(GPIO2_FORWARD, LOW);
  digitalWrite(GPIO0_RETURN, HIGH);
}

inline void robotReturn() {
#ifdef POWER_DEBUG  
  Serial.println("robotReturn");
#endif  
  digitalWrite(GPIO2_FORWARD, HIGH);
  digitalWrite(GPIO0_RETURN, LOW);
}

#endif