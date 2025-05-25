#ifndef CONST_H
#define CONST_H
#include <Arduino.h>
#include "../secret/password.h"

/** password.h 
#ifdef PUBLIC_BROKER
// Public MQTT server
#define SSID          "xxxxxx"
#define PASSWORD      "xxxxxxxx"
#define MQTT_SERVER   "test.mosquitto.org"
#define MQTT_USER     ""
#define MQTT_PASSWORD ""
#define PREFIX "_XX"
#define MQTT_PORT xxxxx
#else
// Private MQTT server
#define SSID          "xxxxx"
#define PASSWORD      "xxxxx"
#define MQTT_SERVER   "xxxxx"
#define MQTT_USER     "xxxxxx"
#define MQTT_PASSWORD "xxxxxx"
#define PREFIX "_YY"
#define MQTT_PORT XXXX
#endif
*/

const String version = "2025.05.10 [D.T]";
#define HOSTNAME "ROBOT_ESP"
// FORCE permet de forcer la mise à jour des paramètres
// à partir de la chaine PARAM
#define POWER_DEBUG

#define FORCE  false 
#define WeMos_D1_Mini
// Pour la mise au point
// Définir DEBUG permet de diviser les données du cycle par 10
//#define DEBUG
#ifdef DEBUG
#define DEBUG_TIME / 10
#else
#define DEBUG_TIME
#endif

// Période d'appel du scheduler en ms
#define TIMER_TIC 1000

// Port des relais utilisé par les moteurs du robot
#ifdef WeMos_D1_Mini
#define GPIO2_FORWARD 5
#define GPIO0_RETURN  4   
#else
#define GPIO2_FORWARD 2
#define GPIO0_RETURN  0
#endif

// Messages MQTT
//-----------------Abonnements---------------------
#define TOPIC_SET_PARAM    PREFIX "robot/param_set"
#define TOPIC_GET_PARAM    PREFIX "robot/param_get"
#define TOPIC_GET_VERSION  PREFIX "robot/versionGet"
#define TOPIC_GET_LOGS     PREFIX "robot/logsGet"
#define TOPIC_GET_STATUS   PREFIX "robot/getStatus"
#define TOPIC_START        PREFIX "robot/start"
#define TOPIC_MANUAL       PREFIX "robot/manual"
#define TOPIC_DELETE_LOGS  PREFIX "robot/logsDelete"
#define TOPIC_RESET        PREFIX "robot/reset"

// -------------Publications--------------------
#define TOPIC_PARAM        PREFIX "robot/param"   
#define TOPIC_READ_VERSION PREFIX "robot/readVersion"
#define TOPIC_READ_LOGS    PREFIX "robot/readLogs"
#define TOPIC_LOG_STATUS   PREFIX "robot/log_status"
#define TOPIC_STATUS       PREFIX "robot/status"
#define TOPIC_RESET_CYCLE  PREFIX "robot/reset_cycle"
#define TOPIC_SCHEDULED    PREFIX "robot/scheduled"
#define TOPIC_CYCLE_TIME   PREFIX "robot/cycle_time"


#define LOG_FILE_NAME "logs.txt"
#define PARAM_FILE_NAME "r_param.txt"

const char *ssid = SSID;
const char *password = PASSWORD;
const char *mqttServer = MQTT_SERVER;
const int mqttPort = MQTT_PORT;
const char *mqttUser = MQTT_USER;
const char *mqttPassword = MQTT_PASSWORD;

// Param robot
// Chaine des paramètres de configuration
const char *PARAM = "1:10:30:40:99:150:360:1";
// Signification des champs
#define SCHEDULED_ENABLE 0
#define SCHEDULED_TIME_H 1
#define SCHEDULED_TIME_M 2
#define MIN_RANDOM       3
#define MAX_RAMDOM       4
#define N_CYCLES         5
#define ACTIVE_TIME      6
#define LOG_STATUS       7
#define PARAM_LEN        8
#endif