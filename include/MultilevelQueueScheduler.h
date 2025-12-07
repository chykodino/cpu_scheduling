#ifndef MULTILEVEL_QUEUE_SCHEDULER_H
#define MULTILEVEL_QUEUE_SCHEDULER_H

#include "Scheduler.h"
#include <queue>
#include <map>

/**
 * @file MultilevelQueueScheduler.h
 * @brief Multilevel Queue scheduling algorithm implementation
 * 
 * Implements a multilevel queue scheduler with multiple ready queues,
 * each with its own scheduling algorithm and priority level.
 * Processes are permanently assigned to one queue based on their priority.
 */

/**
 * @enum QueueSchedulingAlgorithm
 * @brief Scheduling algorithm used within a single queue
 */
enum class QueueSchedulingAlgorithm {
    FCFS,           ///< First Come First Served
    ROUND_ROBIN     ///< Round Robin with configurable quantum
};

/**
 * @struct QueueConfig
 * @brief Configuration for a single queue in the multilevel system
 */
struct QueueConfig {
    int priority;                           ///< Queue priority (0 = highest)
    QueueSchedulingAlgorithm algorithm;     ///< Scheduling algorithm for this queue
    int timeQuantum;                        ///< Time quantum (for RR, ignored for FCFS)
    
    QueueConfig() : priority(0), algorithm(QueueSchedulingAlgorithm::FCFS), timeQuantum(0) {}
    
    QueueConfig(int p, QueueSchedulingAlgorithm alg, int quantum = 4)
        : priority(p), algorithm(alg), timeQuantum(quantum) {}
};

/**
 * @class MultilevelQueueScheduler
 * @brief Implements Multilevel Queue CPU scheduling
 * 
 * This scheduler maintains multiple ready queues, each with different
 * priority and scheduling policy. Processes are assigned to queues based
 * on their priority and cannot move between queues.
 * 
 * Higher priority queues are served before lower priority queues.
 */
class MultilevelQueueScheduler : public Scheduler {
private:
    std::map<int, QueueConfig> queueConfigs;            ///< Configuration for each queue
    std::map<int, std::queue<std::shared_ptr<Process>>> queues;  ///< Multiple ready queues
    std::vector<std::string> ganttChart;                ///< Execution timeline
    
    /**
     * @brief Find the highest priority non-empty queue
     * 
     * @return int Priority level of the highest priority queue with processes, or -1
     */
    int getHighestPriorityQueue();
    
    /**
     * @brief Schedule process from a specific queue using its algorithm
     * 
     * @param queuePriority Priority level of the queue
     * @param process Process to schedule
     */
    void scheduleFromQueue(int queuePriority, std::shared_ptr<Process> process);

public:
    /**
     * @brief Construct a new Multilevel Queue Scheduler
     * 
     * @param contextSwitchOverhead Context switch time cost (default: 0)
     */
    explicit MultilevelQueueScheduler(int contextSwitchOverhead = 0);
    
    /**
     * @brief Add a queue configuration
     * 
     * Defines a new queue level with specific scheduling algorithm and parameters.
     * 
     * @param config Queue configuration (priority, algorithm, quantum)
     */
    void addQueueConfig(const QueueConfig& config);
    
    /**
     * @brief Get the name of this scheduling algorithm
     * 
     * @return std::string "Multilevel Queue"
     */
    std::string getName() const override;
    
    /**
     * @brief Execute multilevel queue scheduling simulation
     * 
     * Processes queues from highest to lowest priority. Within each queue,
     * uses the configured scheduling algorithm.
     */
    void schedule() override;
    
    /**
     * @brief Get visual Gantt chart of execution
     * 
     * @return std::string Formatted timeline showing process execution order
     */
    std::string getGanttChart() const override;
};

#endif // MULTILEVEL_QUEUE_SCHEDULER_H
