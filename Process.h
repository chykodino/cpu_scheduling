#ifndef PROCESS_H
#define PROCESS_H

#include <string>

/**
 * @file Process.h
 * @brief Defines the Process class representing a single process in the scheduler simulation
 * 
 * This class encapsulates all attributes and behaviors of a simulated process,
 * including timing information, priority, and state management.
 */

/**
 * @enum ProcessState
 * @brief Represents the current state of a process in the scheduler
 * 
 * Processes transition between these states during their lifecycle:
 * - NEW: Process has been created but not yet admitted to the ready queue
 * - READY: Process is waiting to be assigned to CPU
 * - RUNNING: Process is currently executing on CPU
 * - WAITING: Process is blocked waiting for I/O or other event
 * - TERMINATED: Process has completed execution
 */
enum class ProcessState {
    NEW,
    READY,
    RUNNING,
    WAITING,
    TERMINATED
};

/**
 * @class Process
 * @brief Represents a single process in the CPU scheduling simulation
 * 
 * The Process class maintains all timing information, priority, and state
 * for a simulated process. It tracks metrics like waiting time, turnaround time,
 * and response time for performance analysis.
 */
class Process {
private:
    int pid;                    ///< Process ID (unique identifier)
    std::string name;           ///< Process name for display purposes
    int arrivalTime;            ///< Time when process arrives in the system
    int burstTime;              ///< Total CPU time required by the process
    int remainingTime;          ///< Remaining CPU time (for preemptive scheduling)
    int priority;               ///< Process priority (lower number = higher priority)
    ProcessState state;         ///< Current state of the process
    
    // Timing metrics
    int startTime;              ///< Time when process first gets CPU (for response time)
    int completionTime;         ///< Time when process finishes execution
    int waitingTime;            ///< Total time spent waiting in ready queue
    int turnaroundTime;         ///< Total time from arrival to completion
    int responseTime;           ///< Time from arrival to first CPU allocation
    
    // Additional tracking
    int lastScheduledTime;      ///< Last time process was scheduled (for calculating waiting)
    bool firstSchedule;         ///< Flag to track if process has been scheduled before

public:
    /**
     * @brief Construct a new Process object
     * 
     * @param pid Process ID
     * @param name Process name
     * @param arrivalTime Time when process arrives
     * @param burstTime Total CPU time required
     * @param priority Process priority (default: 0)
     */
    Process(int pid, const std::string& name, int arrivalTime, 
            int burstTime, int priority = 0);
    
    // Getters
    int getPID() const { return pid; }
    std::string getName() const { return name; }
    int getArrivalTime() const { return arrivalTime; }
    int getBurstTime() const { return burstTime; }
    int getRemainingTime() const { return remainingTime; }
    int getPriority() const { return priority; }
    ProcessState getState() const { return state; }
    int getStartTime() const { return startTime; }
    int getCompletionTime() const { return completionTime; }
    int getWaitingTime() const { return waitingTime; }
    int getTurnaroundTime() const { return turnaroundTime; }
    int getResponseTime() const { return responseTime; }
    int getLastScheduledTime() const { return lastScheduledTime; }
    bool isFirstSchedule() const { return firstSchedule; }
    
    // Setters
    void setState(ProcessState newState) { state = newState; }
    void setPriority(int newPriority) { priority = newPriority; }
    void setStartTime(int time) { startTime = time; }
    void setCompletionTime(int time) { completionTime = time; }
    void setLastScheduledTime(int time) { lastScheduledTime = time; }
    void setFirstSchedule(bool value) { firstSchedule = value; }
    
    /**
     * @brief Execute the process for a given time quantum
     * 
     * Decrements the remaining time by the specified quantum.
     * Used in preemptive scheduling algorithms.
     * 
     * @param quantum Time units to execute
     * @return int Actual time executed (may be less than quantum if process finishes)
     */
    int execute(int quantum);
    
    /**
     * @brief Add waiting time to the process
     * 
     * Increments the total waiting time counter. Called when
     * calculating time spent in ready queue.
     * 
     * @param time Amount of time to add to waiting time
     */
    void addWaitingTime(int time) { waitingTime += time; }
    
    /**
     * @brief Calculate and update all timing metrics
     * 
     * Computes turnaround time and response time based on
     * arrival, start, and completion times. Should be called
     * when process completes execution.
     */
    void calculateMetrics();
    
    /**
     * @brief Check if process has completed execution
     * 
     * @return true if remaining time is 0
     * @return false otherwise
     */
    bool isComplete() const { return remainingTime == 0; }
    
    /**
     * @brief Reset the process to initial state
     * 
     * Useful for running multiple simulations with the same process set.
     * Resets remaining time, state, and all metrics.
     */
    void reset();
    
    /**
     * @brief Get string representation of current process state
     * 
     * @return std::string Human-readable state name
     */
    std::string getStateString() const;
    
    /**
     * @brief Display process information
     * 
     * Prints process details including PID, times, priority, and state
     * to standard output for debugging and visualization.
     */
    void display() const;
};

#endif // PROCESS_H
