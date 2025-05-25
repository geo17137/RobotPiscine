/**
 * @file timerTask.cpp
 * @brief Implementation of a timer-based task scheduler.
 *
 * This file implements a simple task scheduler that manages tasks with timing functionality.
 * Each task can be either a one-shot (monostable) or a repeating timer. Tasks are stored in a
 * fixed-size array (tabTask) and are executed based on their timing parameters.
 */
#include "timerTask.h"

Task tabTask[MAX_TASK];
Task tabLastStatusTask[MAX_TASK];
const char* textStatus[4] = {"N_CREE", "CREE", "PRET", "SUSP"};

/**
 * @brief Default constructor.
 *
 * Initializes a Task object by setting its status to N_CREE and initializing timing parameters (currentTime and stopTime) to 0.
 */
Task::Task() {
  // fonc = 0;
  status = N_CREE;
  currentTime = 0;
  startTime = 0;
}

/**
 * @brief Creates a new task.
 *
 * This function allocates a new Task with the provided function pointer and stop time,
 * then searches for a free entry in the task table (tabTask) to store the new task.
 *
 * @param fonc Pointer to the function that defines the task's behavior.
 * @param stopTime The time interval (in ticks or seconds) after which the task should be executed.
 * @param isTimer Boolean flag indicating whether the task is a recurring timer (true) or a one-shot task (false).
 * @return The index of the task in tabTask if successfully created; returns -1 if no free slot is found.
 */
int Task::t_creer(void (*fonc)(void),unsigned stopTime, boolean isTimer) {
  // Créer une tâche
  Task* task = new Task(fonc,stopTime);
  // Chercher un emplacement libre dans le tableau de tâches
  for (int taskId = 0; taskId < MAX_TASK; taskId++) {
    if (tabTask[taskId].status == N_CREE) {
      // Y copier la tâche 
      tabTask[taskId] = *task;
      tabTask[taskId].timerTask = isTimer;
      return taskId;
    }
  }
  return -1;
}
/**
 * @brief Schedules tasks for execution.
 *
 * This function iterates through all tasks in the task table and checks if they are in the PRET (ready) state.
 * For each ready task, it increments the currentTime count. When currentTime matches stopTime,
 * the associated function (fonc) is executed. Post execution, the task status is updated depending on
 * whether it is a timer task (recurring) or a one-shot task.
 */
void Task::schedule() {
  for (int taskId = 0; taskId < MAX_TASK; taskId++) {
      // printStatus(taskId);
    if (tabTask[taskId].status == PRET) {
      tabTask[taskId].currentTime++;
      if (tabTask[taskId].currentTime == tabTask[taskId].startTime) {
        tabTask[taskId].status = EXEC;
        tabTask[taskId].fonc();
      }
      if (tabTask[taskId].status == EXEC) {
        if (!tabTask[taskId].timerTask)
          tabTask[taskId].status = CREE;
        else {
          tabTask[taskId].status = PRET;
          tabTask[taskId].currentTime = 0;
        }
      }
    }
  }
}
/**
 * @brief Prints the status of all tasks.
 *
 * Iterates over all tasks in the task table and prints their task ID, status, currentTime, and stopTime.
 */
void  Task::printStatusAll() {
  for (int taskId = 0; taskId < MAX_TASK; taskId++)
    printStatus(taskId);
}
/**
 * @brief Prints the status of a specific task.
 *
 * Prints detailed information (task ID, status, currentTime, stopTime) for the task identified by taskId.
 *
 * @param taskId The index of the task in the task table.
 */
void  Task::printStatus(int taskId) {
  Serial.printf("task=%d, status=%s, currentTime=%2d, stopTime=%2d\n",
    taskId, textStatus[tabTask[taskId].status], tabTask[taskId].currentTime, tabTask[taskId].startTime);
}
/**
 * @brief Retrieves the current status of a task.
 *
 * @param taskId The index of the task in the task table.
 * @return The status of the specified task.
 */
int Task::getStatus(int taskId) {
  return tabTask[taskId].status;
}
/**
 * @brief Starts a task.
 *
 * Sets the task status to PRET (ready) and resets its currentTime counter.
 * For timer tasks (recurring), the associated function (fonc) is executed immediately.
 *
 * @param taskId The index of the task in the task table.
 */
void Task::t_start(int taskId) {
  // Serial.println(taskId);
  if (taskId == -1)
    return;
  tabTask[taskId].status = PRET;
  // if (tabTask[itask].stopTime == tabTask[itask].currentTime)
  tabTask[taskId].currentTime = 0;
  if (tabTask[taskId].timerTask) {
    tabTask[taskId].fonc();
  }
}
/**
 * @brief Stops a task.
 *
 * Resets the task by setting its status to CREE and currentTime to 0.
 *
 * @param taskId The index of the task in the task table.
 */
void Task::t_stop(int taskId) {
  if (taskId == -1)
    return;
  tabTask[taskId].status = CREE;
  tabTask[taskId].currentTime = 0;
}
/**
 * @brief Updates the interval (stopTime) for a task.
 *
 * Sets the stopTime parameter that determines how long the task should wait before executing.
 *
 * @param taskId The index of the task in the task table.
 * @param interval The time interval after which the task should execute.
 */
void Task::setInterval(int taskId, unsigned interval) {
  tabTask[taskId].startTime = interval;
}
/**
 * @brief Deletes a task.
 *
 * Marks the task as not created (N_CREE) in the task table, effectively removing it from scheduling.
 *
 * @param taskId The index of the task in the task table.
 */
void Task::t_delete(int taskId) {
  // Serial.println(itask);
  if (taskId == -1)
    return;
  tabTask[taskId].status = N_CREE;
}
/**
 * @brief Retrieves the current time counter for a task.
 *
 * @param taskId The index of the task in the task table.
 * @return The currentTime value of the task.
 */
unsigned Task::getCurrentTime(int taskId) {
  return tabTask[taskId].currentTime;
}
/**
 * @brief Sets the current time counter for a task.
 *
 * Updates the currentTime value used to track the elapsed time for the task.
 *
 * @param taskId The index of the task in the task table.
 * @param time The new count value to set for currentTime.
 */
void Task::setCurrentTime(int taskId , unsigned time) {
  tabTask[taskId].currentTime = time;
}
/**
 * @brief Retrieves the stop time (interval) of a task.
 *
 * @param taskId The index of the task in the task table.
 * @return The stopTime value of the task.
 */
unsigned Task::getStartTime(int taskId) {
  return tabTask[taskId].startTime;
}

/**
 * @brief Sets the stop time (interval) for a task.
 *
 * Updates the stopTime value, which determines when the task should be executed.
 *
 * @param taskId The index of the task in the task table.
 * @param time The new stopTime value to set.
 */
void Task::setStartTime(int taskId, unsigned time) {
  tabTask[taskId].startTime = time;
}

/**
 * @brief Supspend a task.
 *
 * @param taskId The index of the task in the task table.
 */
void Task::t_suspend(int taskId) {
  // Serial.println(itask);
  if (taskId == -1)
    return;
  // if (tabTask[taskId].status == EXEC)
  tabLastStatusTask[taskId].status = tabTask[taskId].status;  
  tabTask[taskId].status = SUSP;
}
/**
 * @brief Resume a task.
 *
 * @param taskId The index of the task in the task table.
 */
void Task::t_resume(int taskId) {
  // Serial.println(itask);
  if (taskId == -1)
    return;
  tabTask[taskId].status = tabLastStatusTask[taskId].status;  
}
