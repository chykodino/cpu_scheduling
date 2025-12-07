#ifndef ROUND_ROBIN_SCHEDULER_H
#define ROUND_ROBIN_SCHEDULER_H

#include "Scheduler.h"
#include <queue>

/**
 * @file RoundRobinScheduler.h
 * @brief Round Robin scheduling algorithm implementation
 * 
 * Round Robin is a preemptive scheduling algorithm where each process
 * gets a fixed time quantum in circular order. When a process's quantum
 * expires, it moves to the back of the ready queue.
 */

/**
 * @class RoundRobinScheduler
 * @brief Implements the Round Robin CPU scheduling algorithm
 * 
 * This scheduler maintains a FIFO queue of ready processes and allocates
 * CPU time in fixed-size quanta. Provides fair CPU distribution and good
 * response time for interactive processes.
 */
class RoundRobinScheduler : public Scheduler {
private:
    int timeQuantum;                                    ///< Time quantum for each process
    std::queue<std::shared_ptr<Process>> readyQueue;    ///< FIFO queue of ready processes
    std::vector<std::string> ganttChart;                ///< Execution timeline for visualization

public:
    /**
     * @brief Construct a new Round Robin Scheduler
     * 
     * @param quantum Time quantum for each process (default: 4)
     * @param contextSwitchOverhead Time cost for context switches (default: 0)
     */
    explicit RoundRobinScheduler(int quantum = 4, int contextSwitchOverhead = 0);
    
    /**
     * @brief Get the name of this scheduling algorithm
     * 
     * @return std::string "Round Robin"
     */
    std::string getName() const override;
    
    /**
     * @brief Execute Round Robin scheduling simulation
     * 
     * Processes are scheduled in FIFO order, each getting exactly timeQuantum
     * units of CPU time before being preempted (unless they complete earlier).
     */
    void schedule() override;
    
    /**
     * @brief Get visual Gantt chart of execution
     * 
     * @return std::string Formatted timeline showing process execution order
     */
    std::string getGanttChart() const override;
};

#endif // ROUND_ROBIN_SCHEDULER_H
