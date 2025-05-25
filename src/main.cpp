// Author : Daniel Tschirhart
// Date première version  : 15/3/2023
// Date vestion actuelle  : 27/02/2025
/*
 * @file main.cpp
 *
 * @brief Main program for the RobotPiscine project.
 *
 * This file implements the core functionality to manage the autonomous operation of a pool-cleaning robot.
 * It provides OTA updates, WiFi and MQTT connectivity, file-based parameter and log management, and scheduled
 * tasks to control robot movement.
 *
 * Key Functionalities:
 *  - OTA Initialization:
 *      - initOTA(): Configures and starts the OTA update service using a given hostname.
 *
 *  - Reset Reason Handling:
 *      - bootRaison(): Returns a human-readable string that explains the reason for the last system reset.
 *
 *  - File Parameters Management:
 *      - initFileParam(): Initializes the parameters file (using FileLittleFS), writes default parameters if needed,
 *                         and loads parameters into the system.
 *
 *  - WiFi and MQTT Configuration:
 *      - initWifiStation(): Sets WiFi mode, attempts to connect to the specified SSID, and configures auto-reconnect.
 *      - initMQTTClient(): Configures the MQTT client, connects to the broker, subscribes to specific topics, and
 *                          handles reconnection if needed.
 *
 *  - Logging:
 *      - logsWrite(): Writes a log message with a timestamp to the log file, used primarily for recording boot reasons.
 *      - writeLogs(): Conditionally writes log messages depending on the log status.
 *
 *  - Parameter Handling:
 *      - split(): Tokenizes a parameter string into an array of items based on a given delimiter.
 *      - setParam(): Parses the parameter string and updates configuration variables (including cycle parameters,
 *                    scheduled clean time, and logging status).
 *
 *  - Time Management:
 *      - getDate(), getHour(), and getMinutes(): Retrieve and format the current date and time using an NTP client.
 *
 *  - Robot Cleaning Cycle Tasks:
 *      - robotTask(): Alternates robot motion (forward/reverse) with randomized timing intervals,
 *                     updates cycle counts, and publishes the current cycle state.
 *      - robotEndTask(): Ends the cleaning cycle, stops timers, powers off the robot, and logs the cycle end.
 *      - endCycle(): A helper to conclude a cycle, reset timers and state, and publish a cycle reset.
 *
 *  - Scheduled Operations:
 *      - scheduleCleanTask(): Checks at regular intervals whether the scheduled clean time has been reached
 *                             and starts the cleaning cycle if conditions are met.
 *
 *  - MQTT Callback:
 *      - PubSubCallback(): Processes incoming MQTT messages to update parameters, start/stop robot tasks,
 *                          retrieve logs, and reset the system.
 *
 *  - Main Application Flow:
 *      - setup(): Initializes Serial communication, pin modes, file systems, network connections, MQTT client,
 *                 NTP client, logging, parameter setup, and task scheduling.
 *      - loop(): Main execution loop that handles WiFi/MQTT reconnections, feeds the watchdog timer, processes OTA updates,
 *                and repeatedly triggers scheduled task execution.
 *
 * Notes:
 *  - The code includes conditional compilation for debugging and timing adjustments (DEBUG_TIME).
 *  - Task scheduling and timed operations are managed by the timerTask module.
 *  - All topics for MQTT communications (parameters, status, logs, etc.) are defined within the project context.
 *
 * Usage:
 *  - Deploy the firmware to an ESP8266/ESP device configured with PlatformIO.
 *  - Ensure that dependencies such as FileLittleFS and timerTask libraries are properly included.
 */

#include "main.h"
#include "files.h"


inline void initOTA() {
  ArduinoOTA.setHostname(HOSTNAME);
  ArduinoOTA.begin();
}


const char* bootRaison() {
  rst_info* resetInfo;
  resetInfo = ESP.getResetInfoPtr();
  switch (resetInfo->reason) {
  case REASON_DEFAULT_RST:
    return "Startup power on";
  case REASON_WDT_RST:
    return "Watch dog reset ";
  case REASON_EXCEPTION_RST:
    return "Exception reset";
  case REASON_SOFT_WDT_RST:
    return "Software watch dog reset";
  case REASON_SOFT_RESTART:
    return "Software restart";
  case REASON_DEEP_SLEEP_AWAKE:
    return "Wake from deep-sleep";
  case REASON_EXT_SYS_RST:
    return "Watch dog reset (ext)";
  default:
    return "Unknown reset cause";
  };
}

FileLittleFS* initFileParam(boolean force) {
  fileParam = new FileLittleFS(PARAM_FILE_NAME);
  if (!fileParam->exist() || force) {
    fileParam->writeFile(PARAM, "w");
    strcpy(tabParam, PARAM);
  }
  else {
    strcpy(tabParam, fileParam->readFile().c_str());
    fileParam->close();
  }
  return fileParam;
}

void initWifiStation() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.printf("Wifi %s not connected! Rebooting...", ssid);
    delay(5000);
    ESP.restart();
  }
  WiFi.setHostname(HOSTNAME);
  WiFi.setAutoReconnect(true);
  WiFi.persistent(true);
  initOTA();
}

void initMQTTClient() {
  // Connecting to MQTT server
  mqttClient.setServer(mqttServer, mqttPort);
  mqttClient.setCallback(PubSubCallback);
  while (!mqttClient.connected()) {
    Serial.println(String("Connecting to MQTT (") + mqttServer + ")...");
    // Pour un même courtier les clients doivent avoir un id différent
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);

    if (mqttClient.connect(clientId.c_str(), mqttUser, mqttPassword)) {
      Serial.println("MQTT client connected");
      // digitalWrite(GPIO2_LED, LOW);
    }
    else {
      Serial.print("\nFailed with state ");
      Serial.println(mqttClient.state());
      // digitalWrite(GPIO2_LED, HIGH);
      if (WiFi.status() != WL_CONNECTED) {
        initWifiStation();
      }
      delay(500);
    }
  }
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  // Abonne le client aux messages 
  mqttClient.subscribe(TOPIC_SET_PARAM);
  mqttClient.subscribe(TOPIC_GET_PARAM);
  mqttClient.subscribe(TOPIC_GET_VERSION);
  mqttClient.subscribe(TOPIC_GET_LOGS);
  mqttClient.subscribe(TOPIC_GET_STATUS);
  mqttClient.subscribe(TOPIC_START);
  mqttClient.subscribe(TOPIC_MANUAL);
  mqttClient.subscribe(TOPIC_DELETE_LOGS);
  mqttClient.subscribe(TOPIC_RESET);
}

// Ecrire systématique d'un log
// Utilisé par bootRaison
void logsWrite(const char* msg) {
  char buffer[80];
  sprintf(buffer, "%s - %s\n", getDate(), msg);
  fileLog->writeFile(buffer, "a");
}

// Ecrire conditionnelle d'un log 'fonction du champ LOG_STATUS
void writeLogs(const char* log) {
  if (!logStatus)
    return;
  char buffer[80];
  sprintf(buffer, "%s - %s\n", getDate(), log);
  fileLog->writeFile(buffer, "a");
}

/*
 * Retourne un tableau de PARAM_LEN chaines splittées par motif
 * Attention str est modifié par la fonction
 */
Item* split(char* str, const char* motif) {
  static Item items[PARAM_LEN + 1];
  char* pch;
  pch = strtok(str, motif);
  int i = 0;
  while (pch != NULL) {
    strcpy(items[i++].item, pch);
    pch = strtok(NULL, motif);
  }
  return items;
}

char* getDate() {
  static char date[80];
  ntpTime->update();
  time_t epochTime = ntpTime->getEpochTime();
  struct tm* ptm = gmtime((time_t*)&epochTime);
  sprintf(date, "%02d/%02d/%4d %s",
    ptm->tm_mday,
    (ptm->tm_mon + 1),
    (ptm->tm_year + 1900),
    ntpTime->getFormattedTime().c_str());
  return date;
}

inline int getHour() {
  return ntpTime->getHours();
}

inline int getMinutes() {
  return ntpTime->getMinutes();
}

// Met à jour les variables du cycle à partir
// De la chaine param lue dans le fichier PARAM_FILE_NAME
// et mis à jour par le message TOPIC_GET_PARAM
// DEBUG_TIME est soit vide soit égal à  /10 (DEBUG défini)
void setParam(char* tabParam) {
  char temp[50];
  strcpy(temp, tabParam);
  Item* paramItem = split(temp, ":");
  scheduleEnabled = atoi(get(paramItem, SCHEDULED_ENABLE));
  scheduleH = atoi(get(paramItem, SCHEDULED_TIME_H));
  scheduleM = atoi(get(paramItem, SCHEDULED_TIME_M));
  minRandom = atoi(get(paramItem, MIN_RANDOM))  DEBUG_TIME;
  maxRandom = atoi(get(paramItem, MAX_RAMDOM))  DEBUG_TIME;
  nbCycles  = atoi(get(paramItem, N_CYCLES))    DEBUG_TIME;
  activeTime= atoi(get(paramItem, ACTIVE_TIME)) DEBUG_TIME;
  logStatus = atoi(get(paramItem, LOG_STATUS));
}

// Pour le moment non utilisé
// char* modifyParam(const char* tabParam, const char* theParam, int pos) {
//   static char temp[50];
//   strcpy(temp, tabParam);
//   Item* paramItem = split(temp, ":");
//   strcpy(get(paramItem, pos), theParam);
//   strcpy(temp, "");
//   int i = 0;
//   for (; i < PARAM_LEN - 1; i++) {
//     strcat(temp, get(paramItem, i));
//     strcat(temp, ":");
//   }
//   strcat(temp, get(paramItem, i));
//   return temp;
// }


void endCycle() {
  timerTask.t_stop(idRobotTask);
  timerTask.t_stop(idEndRobotTask);
  currentCycle = 0;
  powerOff();
  mqttClient.publish(TOPIC_RESET_CYCLE, "");
  activeScheduledTask = false;
}

// Timer
// Appelé par le scheduler toute les currentRandomValue secondes
// Avance-recule
void robotTask() {
  powerOff();
  delay(750);
  static unsigned randomValue = 0;
  // Nouveau temps d'avance ou de recul
  // currentRandomValue = random(minRandom, maxRandom);
  if (!direction) {
    currentRandomValue = random(minRandom, maxRandom);
    randomValue = currentRandomValue;
    timerTask.setInterval(idRobotTask, currentRandomValue);

    robotForward();
  }
  else {
    // if (currentRandomValue > randomValue && currentRandomValue > 25)
    //    currentRandomValue = randomValue -  15;
    if (randomValue > 30) {
      currentRandomValue = randomValue -  20;
    }
    timerTask.setInterval(idRobotTask, currentRandomValue);
    robotReturn();
  }
  mqttClient.publish(TOPIC_CYCLE_TIME, randomBuffer);
  direction = !direction;
  if (++currentCycle >= nbCycles) {
    endCycle();
    writeLogs("End count cycle");
  }
}

// Monostable appelé à la fin du temps de nettoyage
void robotEndTask() {
  endCycle();
  writeLogs("End time cycle");
}

// Cycle programmé, appelé toute les minutes
void scheduleCleanTask() {
  if (scheduleEnabled) {
    if (scheduleM == getMinutes() && scheduleH == getHour()) {
      timerTask.t_start(idRobotTask);
      timerTask.t_start(idEndRobotTask);
      // Serial.println("scheduleCleanTask()");
      writeLogs("Start scheduled clean cycle");
      sprintf(bufferTime, "%02d:%02d\r", getHour(), getMinutes());
      mqttClient.publish(TOPIC_SCHEDULED, bufferTime);
      activeScheduledTask = true;
    }
  }
}

// Executé au boot
void setup() {
  Serial.begin(115200);

  pinMode(GPIO2_FORWARD, OUTPUT);
  pinMode(GPIO0_RETURN, OUTPUT);
  digitalWrite(GPIO2_FORWARD, HIGH);
  digitalWrite(GPIO0_RETURN, HIGH);
  // Permet de vérifier que le serveur ntp fourni l'heure
  strcpy(date, "00/00/00 00:00:00");
  fileParam = initFileParam(FORCE);
  fileLog = new FileLittleFS(LOG_FILE_NAME);

  initWifiStation();
  initMQTTClient();

  ntpTime = new NTPClient(ntpUDP, "pool.ntp.org", 3600*2, 6000);
  ntpTime->begin();
  Serial.println(getDate());
  logsWrite(bootRaison());

  setParam(tabParam);
  // debugPrintParam();
  // Effacer les logs si supérieur à 4K octets
  fileLog->purge(4048);
  
  // Création des tâches
  // Tache rythmée de nettoyage
  randomSeed(analogRead(A0));
  idRobotTask = timerTask.t_creer(robotTask, random(minRandom, maxRandom), true);

  // Monostable déclenchant la fin du nettoyage après activeTime * 60 secondes
  idEndRobotTask = timerTask.t_creer(robotEndTask, activeTime * 60, false);

  // Tache rytmmée toute les 60 secondes, utilisée pour déclancher le nettoyage programmé 
  idScheduleCleanTask = timerTask.t_creer(scheduleCleanTask, 60, true);
  timerTask.t_start(idScheduleCleanTask);

  mqttClient.publish(TOPIC_RESET_CYCLE, "");
  Serial.println("Robot piscine V" + version);
  Serial.println(getDate());
  currentRandomValue = random(minRandom, maxRandom);
}

void deleteLogs() {
  fileLog->deleteFile();
}

/*
 * Publier régulièrement les états afin de réfléter l'opération
 * en cours sur le smartphone même en cas connexion/reconnexion
 * de ce dernier.
*/
void publishState() {
  static char buffer[60];
  char randomBuffer[60];
  sprintf(buffer, "Cycle %d/%d, t=%d/%d mn#%d",
    currentCycle,
    nbCycles,
    timerTask.getCurrentTime(idEndRobotTask) / 60,
    timerTask.getStartTime(idEndRobotTask) / 60,
    timerTask.getStatus(idRobotTask) != CREE);
  mqttClient.publish(TOPIC_STATUS, buffer);  
  if (!direction) {
    sprintf(randomBuffer, "AV %d/%d", timerTask.getCurrentTime(idRobotTask), currentRandomValue);
  }
  else {
    sprintf(randomBuffer, "AR %d/%d", timerTask.getCurrentTime(idRobotTask), currentRandomValue);
  }  
  mqttClient.publish(TOPIC_CYCLE_TIME, randomBuffer);
  if (activeScheduledTask)
    mqttClient.publish(TOPIC_SCHEDULED, bufferTime);
}

// Boucle de scrutation
void loop() {
  static  long tps = 0;
  static  int wifiTest = 0;
  // Vérifier la connexion WiFI et reconnecter le cas échéant
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    WiFi.reconnect();
    delay(5000);
    if (wifiTest++ == 6) {
      ESP.restart();
    }
  }
  // Reset du chien de garde  
  ESP.wdtFeed();

  if (!mqttClient.connected()) {
    initMQTTClient();
  }
  // Alimenter les boucles de messages
  ArduinoOTA.handle();
  mqttClient.loop();

  // Mettre à jour le scheduler toutes les s
  if (millis() - tps > TIMER_TIC) {
    tps = millis();
    timerTask.schedule();
  }
}

// Fonction de rappel MQTT
// Appelé à la réception d'un message abonné
void PubSubCallback(char* topic, byte* payload, unsigned int length) {
  String strPayload = "";
  static String strON = "ON";
  static String strOFF = "OFF";
  static String strSTOP = "STOP";

  for (unsigned int i = 0; i < length; i++) {
    strPayload += (char)payload[i];
  }

  // Serial.print("Topic:");
  // Serial.println(topic);
  // Serial.print("Message:");

  //------------------- TOPIC_SET_PARAM -----------------
  if (strcmp(topic, TOPIC_SET_PARAM) == 0) {
    strcpy(tabParam, strPayload.c_str());
    fileParam->writeFile(tabParam, "w");
    setParam(tabParam);
    // Serial.println(tabParam);
    // setParam met à jour activeTime
    // Réactualiser le temps de fonctionnement du robot si modifié
    timerTask.setStartTime(idEndRobotTask, activeTime * 60);
    return;
  }
  //------------------- TOPIC_GET_PARAM ----------------
  if (strcmp(topic, TOPIC_GET_PARAM) == 0) {
    // Serial.println(tabParam);
    mqttClient.publish(TOPIC_PARAM, tabParam);
    if (currentCycle!=0)
      mqttClient.publish(TOPIC_CYCLE_TIME, randomBuffer);
    else
      mqttClient.publish(TOPIC_CYCLE_TIME, "0");
    return;
  }
  //------------------ TOPIC_GET_VERSION ----------------
  else if (strcmp(topic, TOPIC_GET_VERSION) == 0) {
    static char buffer[50];
    sprintf(buffer, "%s;%s", version.c_str(), WiFi.localIP().toString().c_str());
    mqttClient.publish(TOPIC_READ_VERSION, buffer);
    return;
  }
  //------------------ TOPIC_GET_LOGS ----------------
  else if (strcmp(topic, TOPIC_GET_LOGS) == 0) {
    // L'implémentation MQTT pour l'esp8266 limite la taille
    // des messages. Il faut les envoyer ligne par ligne
    String logs = fileLog->readFile();
    fileLog->close();
    unsigned i = 0;
    while (i < logs.length()) {
      String partial = logs.substring(i, logs.indexOf("\n", i) + 1);
      mqttClient.publish(TOPIC_READ_LOGS, partial.c_str());
      i = logs.indexOf("\n", i) + 1;
    }
    // Message de fin
    mqttClient.publish(TOPIC_READ_LOGS, "#####");
    return;
  }
  //------------------ TOPIC_GET_STATUS ----------------
  else if (strcmp(topic, TOPIC_GET_STATUS) == 0) {
    publishState();
    return;
  }
  //------------------ TOPIC_START ----------------
  else if (strcmp(topic, TOPIC_START) == 0) {
    if (strPayload == strON) {
      timerTask.t_start(idRobotTask);
      timerTask.t_start(idEndRobotTask);
      writeLogs("Start manual cycle");

    }
    else {
      robotEndTask();
      mqttClient.publish(TOPIC_CYCLE_TIME, "0");
    }
    return;
  }
  //------------------ TOPIC_MANUAL ----------------
  else if (strcmp(topic, TOPIC_MANUAL) == 0) {
    static boolean b;
    // timerTask.printStatus(idRobotTask);
    if (currentCycle==0) {
      timerTask.t_stop(idRobotTask);
      timerTask.t_stop(idEndRobotTask);
      if (strPayload == strON) {
        robotForward();
        delay(2000);
        }
      else if (strPayload == strOFF) {
        robotReturn();
        delay(2000);
      }
      else {
        powerOff();
      }
    }
    else {
      if (strPayload == strSTOP) {
        if (!b) {
          timerTask.t_suspend(idRobotTask);
          powerOff();
        }
        else {
          // Serial.println("resume");
          timerTask.t_resume(idRobotTask);
          if (!direction) {
            robotForward();
          }
          else {
            robotReturn();
          }
        }
        b = !b;
      }
    }
    return;
  }
  //------------------ TOPIC_DELETE_LOGS ----------------
  else if (strcmp(topic, TOPIC_DELETE_LOGS) == 0) {
    deleteLogs();
  }
  //------------------  TOPIC_RESET ----------------------
  if (cmp(topic, TOPIC_RESET)) {
    ESP.restart();
    return;
  }
}