#ifndef MULTILEVEL_FEEDBACK_QUEUE_SCHEDULER_H
#define MULTILEVEL_FEEDBACK_QUEUE_SCHEDULER_H

#include "Scheduler.h"
#include <queue>
#include <vector>
#include <map>

/**
 * @file MultilevelFeedbackQueueScheduler.h
 * @brief Multilevel Feedback Queue scheduling algorithm implementation
 * 
 * Implements MLFQ scheduling where processes can move between queues based
 * on their behavior. Provides good response time for I/O-bound processes
 * while ensuring CPU-bound processes eventually complete.
 */

/**
 * @class MultilevelFeedbackQueueScheduler
 * @brief Implements Multilevel Feedback Queue CPU scheduling
 * 
 * This scheduler maintains multiple queues with decreasing priority.
 * Processes start in the highest priority queue and move to lower
 * priority queues if they use their full time quantum (CPU-bound behavior).
 * 
 * Features:
 * - Dynamic priority adjustment based on process behavior
 * - Prevents starvation through aging mechanism
 * - Favors I/O-bound and interactive processes
 */
class MultilevelFeedbackQueueScheduler : public Scheduler {
private:
    int numQueues;                                      ///< Number of priority queues
    std::vector<int> timeQuantums;                      ///< Time quantum for each queue
    std::vector<std::queue<std::shared_ptr<Process>>> queues;  ///< Multiple ready queues
    std::map<int, int> processQueueLevel;               ///< Track which queue each process is in
    std::vector<std::string> ganttChart;                ///< Execution timeline
    bool agingEnabled;                                  ///< Enable aging to prevent starvation
    int agingThreshold;                                 ///< Time before promoting process
    std::map<int, int> timeInQueue;                     ///< Track time process spends in queue
    
    /**
     * @brief Find the highest priority non-empty queue
     * 
     * @return int Index of highest priority queue with processes, or -1
     */
    int getHighestPriorityQueue();
    
    /**
     * @brief Move process to lower priority queue (demotion)
     * 
     * Called when a process uses its full time quantum without completing.
     * 
     * @param process Process to demote
     */
    void demoteProcess(std::shared_ptr<Process> process);
    
    /**
     * @brief Move process to higher priority queue (promotion)
     * 
     * Called as part of aging mechanism to prevent starvation.
     * 
     * @param process Process to promote
     */
    void promoteProcess(std::shared_ptr<Process> process);
    
    /**
     * @brief Apply aging mechanism to prevent starvation
     * 
     * Promotes processes that have been waiting too long in lower priority queues.
     */
    void applyAging();

public:
    /**
     * @brief Construct a new Multilevel Feedback Queue Scheduler
     * 
     * Creates a scheduler with configurable number of queues and time quanta.
     * 
     * @param numQueues Number of priority levels (default: 3)
     * @param enableAging Enable aging mechanism (default: true)
     * @param agingThreshold Time units before promoting process (default: 10)
     * @param contextSwitchOverhead Context switch time cost (default: 0)
     */
    explicit MultilevelFeedbackQueueScheduler(int numQueues = 3, bool enableAging = true,
                                             int agingThreshold = 10, 
                                             int contextSwitchOverhead = 0);
    
    /**
     * @brief Set time quantum for a specific queue
     * 
     * Allows customization of quantum values for each priority level.
     * Typically, lower priority queues have larger quanta.
     * 
     * @param queueIndex Queue index (0 = highest priority)
     * @param quantum Time quantum for this queue
     */
    void setTimeQuantum(int queueIndex, int quantum);
    
    /**
     * @brief Get the name of this scheduling algorithm
     * 
     * @return std::string "Multilevel Feedback Queue"
     */
    std::string getName() const override;
    
    /**
     * @brief Execute MLFQ scheduling simulation
     * 
     * Processes start in highest priority queue and are demoted if they
     * use their full quantum. Aging promotes long-waiting processes.
     */
    void schedule() override;
    
    /**
     * @brief Get visual Gantt chart of execution
     * 
     * @return std::string Formatted timeline showing process execution order
     */
    std::string getGanttChart() const override;
};

#endif // MULTILEVEL_FEEDBACK_QUEUE_SCHEDULER_H
