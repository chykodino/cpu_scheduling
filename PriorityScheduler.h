#ifndef PRIORITY_SCHEDULER_H
#define PRIORITY_SCHEDULER_H

#include "Scheduler.h"
#include <queue>
#include <functional>

/**
 * @file PriorityScheduler.h
 * @brief Priority-based scheduling algorithm implementations
 * 
 * Implements both preemptive and non-preemptive priority scheduling.
 * Processes are selected based on priority values (lower number = higher priority).
 */

/**
 * @class PriorityScheduler
 * @brief Implements priority-based CPU scheduling
 * 
 * This scheduler selects processes based on priority values. Supports both
 * preemptive mode (higher priority process can interrupt running process)
 * and non-preemptive mode (running process completes its burst).
 * 
 * Includes aging mechanism to prevent starvation of low-priority processes.
 */
class PriorityScheduler : public Scheduler {
private:
    bool preemptive;                                    ///< True for preemptive, false for non-preemptive
    bool agingEnabled;                                  ///< Enable priority aging to prevent starvation
    int agingInterval;                                  ///< Time units between priority boosts
    std::vector<std::string> ganttChart;                ///< Execution timeline
    
    /**
     * @brief Custom comparator for priority queue
     * 
     * Compares processes based on priority (lower value = higher priority).
     * Breaks ties using arrival time (earlier arrival = higher priority).
     */
    struct PriorityComparator {
        bool operator()(const std::shared_ptr<Process>& a, 
                       const std::shared_ptr<Process>& b) const {
            // Lower priority number = higher priority
            if (a->getPriority() != b->getPriority()) {
                return a->getPriority() > b->getPriority();
            }
            // Break ties with arrival time (earlier = higher priority)
            return a->getArrivalTime() > b->getArrivalTime();
        }
    };
    
    std::priority_queue<std::shared_ptr<Process>, 
                       std::vector<std::shared_ptr<Process>>,
                       PriorityComparator> readyQueue;  ///< Priority-ordered ready queue
    
    /**
     * @brief Apply priority aging to prevent starvation
     * 
     * Increases priority (decreases priority number) of waiting processes
     * to ensure they eventually get CPU time.
     */
    void applyAging();

public:
    /**
     * @brief Construct a new Priority Scheduler
     * 
     * @param preemptive True for preemptive priority scheduling
     * @param enableAging Enable priority aging mechanism (default: true)
     * @param agingInterval Time between priority boosts (default: 5)
     * @param contextSwitchOverhead Context switch time cost (default: 0)
     */
    explicit PriorityScheduler(bool preemptive, bool enableAging = true,
                              int agingInterval = 5, int contextSwitchOverhead = 0);
    
    /**
     * @brief Get the name of this scheduling algorithm
     * 
     * @return std::string "Preemptive Priority" or "Non-Preemptive Priority"
     */
    std::string getName() const override;
    
    /**
     * @brief Execute priority scheduling simulation
     * 
     * Schedules processes based on priority values. In preemptive mode,
     * checks for higher priority processes after each time unit.
     */
    void schedule() override;
    
    /**
     * @brief Get visual Gantt chart of execution
     * 
     * @return std::string Formatted timeline showing process execution order
     */
    std::string getGanttChart() const override;
};

#endif // PRIORITY_SCHEDULER_H
