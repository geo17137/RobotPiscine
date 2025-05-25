#ifndef TIMER_TASK_H
#define TIMER_TASK_H
#include <Arduino.h>

// Monostables et Timers
// 
// Les monostables sont utilisés ici pour déclencher des actions
// après un temps programmé (monostable à action retardée). Ils facilitent grandement la
// programmation de cette application basée sur l'exécution d'actions
// régies par le temps
// Les monostatbles sont construit sur un mécanisme des tâches
// Les tâches sont identifiées par un entier
// et sont mémorisées dans un tableau.
// L'ordonnancement est circulaire et l'ordonnanceur
// est lancé touté les secondes dans loop
// Les timers sont utilisés pour lancer des fonctions à intervalle 
// régulier

#define MAX_TASK  4

#define N_CREE    0
#define CREE      1
#define PRET      2
#define EXEC      4
#define SUSP      3

typedef int task_id;

/**
 * @class Task
 * @brief Represents a scheduled task.
 *
 * The Task class encapsulates the attributes and methods to manipulate a task, including creating,
 * starting, stopping, scheduling, and deleting tasks. Each task holds a function pointer to be executed,
 * its current status, and timing details.
 */
class Task {
private:
  void (*fonc)(void);
  unsigned status;
  unsigned currentTime;
  unsigned startTime;
  boolean timerTask;
public:
  Task();
  Task(void (*fonc)(void), unsigned startTime) {
    this->fonc = fonc;
    this->startTime = startTime;
    this->currentTime = 0;
    this->status = CREE;
  }
  task_id t_creer(void (*fonc)(void), unsigned stopTime, boolean isTimer);
  void t_start(int taskId);
  void t_stop(int taskId);
  void t_suspend(int taskId);
  void t_resume(int taskid);
  void t_delete(int taskId);
  void setInterval(int taskId, unsigned interval);
  void schedule();
  void printStatus(int taskId);
  void printStatusAll();
  int  getStatus(int taskId);
  unsigned getCurrentTime(int taskId);
  unsigned getStartTime(int taskId);
  void setCurrentTime(int taskId, unsigned time);
  void setStartTime(int taskId, unsigned time);
};
#endif