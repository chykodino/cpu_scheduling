#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "Process.h"
#include <vector>
#include <queue>
#include <memory>
#include <string>

/**
 * @file Scheduler.h
 * @brief Defines the abstract base class for all CPU scheduling algorithms
 * 
 * This file provides the interface that all scheduling algorithm implementations
 * must follow, ensuring consistent behavior and metric tracking across different
 * scheduling policies.
 */

/**
 * @struct SchedulingMetrics
 * @brief Contains aggregate performance metrics for a scheduling simulation
 * 
 * These metrics are calculated after a simulation completes and provide
 * insight into the overall performance of the scheduling algorithm.
 */
struct SchedulingMetrics {
    double averageWaitingTime;      ///< Average time processes spend waiting
    double averageTurnaroundTime;   ///< Average time from arrival to completion
    double averageResponseTime;     ///< Average time from arrival to first CPU allocation
    double cpuUtilization;          ///< Percentage of time CPU was busy
    double throughput;              ///< Number of processes completed per time unit
    int totalContextSwitches;       ///< Number of context switches performed
    int totalTime;                  ///< Total simulation time
};

/**
 * @class Scheduler
 * @brief Abstract base class for all CPU scheduling algorithms
 * 
 * This class defines the interface that all scheduling algorithms must implement.
 * It provides common functionality for process management, metric tracking,
 * and simulation execution.
 */
class Scheduler {
protected:
    std::vector<std::shared_ptr<Process>> processes;  ///< All processes to be scheduled
    int currentTime;                                   ///< Current simulation time
    int contextSwitchOverhead;                         ///< Time cost of context switch
    int totalContextSwitches;                          ///< Count of context switches
    std::shared_ptr<Process> currentProcess;           ///< Currently running process
    
    /**
     * @brief Perform a context switch
     * 
     * Simulates the overhead of switching between processes.
     * Increments the context switch counter and advances simulation time.
     * 
     * @param from Process being switched from (can be nullptr)
     * @param to Process being switched to (can be nullptr)
     */
    void contextSwitch(std::shared_ptr<Process> from, std::shared_ptr<Process> to);
    
    /**
     * @brief Update waiting time for all ready processes
     * 
     * Adds elapsed time to the waiting time of all processes currently
     * in the ready state. Should be called each time simulation time advances.
     * 
     * @param elapsedTime Time units that have passed
     */
    void updateWaitingTimes(int elapsedTime);
    
    /**
     * @brief Check for and admit newly arrived processes
     * 
     * Moves processes from NEW state to READY state when their arrival
     * time equals the current simulation time.
     * 
     * @return int Number of processes that arrived
     */
    int admitArrivingProcesses();

public:
    /**
     * @brief Construct a new Scheduler object
     * 
     * @param contextSwitchOverhead Time cost for each context switch (default: 0)
     */
    explicit Scheduler(int contextSwitchOverhead = 0);
    
    /**
     * @brief Virtual destructor for proper cleanup of derived classes
     */
    virtual ~Scheduler() = default;
    
    /**
     * @brief Add a process to the scheduler
     * 
     * @param process Shared pointer to the process to add
     */
    void addProcess(std::shared_ptr<Process> process);
    
    /**
     * @brief Get the name of the scheduling algorithm
     * 
     * Each derived class must implement this to return its algorithm name.
     * 
     * @return std::string Name of the scheduling algorithm
     */
    virtual std::string getName() const = 0;
    
    /**
     * @brief Execute the scheduling simulation
     * 
     * Pure virtual function that must be implemented by each scheduling algorithm.
     * Runs the complete simulation from start to finish.
     */
    virtual void schedule() = 0;
    
    /**
     * @brief Calculate aggregate performance metrics
     * 
     * Computes average waiting time, turnaround time, response time,
     * CPU utilization, and throughput based on completed processes.
     * 
     * @return SchedulingMetrics Structure containing all calculated metrics
     */
    SchedulingMetrics calculateMetrics() const;
    
    /**
     * @brief Display detailed results of the simulation
     * 
     * Prints individual process metrics and aggregate statistics
     * in a formatted table to standard output.
     */
    void displayResults() const;
    
    /**
     * @brief Get Gantt chart representation of execution
     * 
     * Creates a visual timeline showing which process ran at each time unit.
     * Useful for understanding the execution order and identifying idle time.
     * 
     * @return std::string Gantt chart as a formatted string
     */
    virtual std::string getGanttChart() const;
    
    /**
     * @brief Reset the scheduler to initial state
     * 
     * Resets all processes and timing information, allowing the same
     * scheduler to run multiple simulations.
     */
    void reset();
    
    /**
     * @brief Get all processes
     * 
     * @return const std::vector<std::shared_ptr<Process>>& Reference to process vector
     */
    const std::vector<std::shared_ptr<Process>>& getProcesses() const { 
        return processes; 
    }
};

#endif // SCHEDULER_H
