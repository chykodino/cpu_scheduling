#include "MultilevelQueueScheduler.h"
#include <iostream>
#include <sstream>
#include <algorithm>
#include <climits>
#include <iomanip>

/**
 * @file MultilevelQueueScheduler.cpp
 * @brief Implementation of Multilevel Queue scheduling algorithm
 */

MultilevelQueueScheduler::MultilevelQueueScheduler(int contextSwitchOverhead)
    : Scheduler(contextSwitchOverhead) {
}

void MultilevelQueueScheduler::addQueueConfig(const QueueConfig& config) {
    queueConfigs[config.priority] = config;
    queues[config.priority] = std::queue<std::shared_ptr<Process>>();
}

std::string MultilevelQueueScheduler::getName() const {
    return "Multilevel Queue (" + std::to_string(queueConfigs.size()) + " queues)";
}

int MultilevelQueueScheduler::getHighestPriorityQueue() {
    // Return the lowest priority number (highest priority) that has processes
    for (const auto& pair : queueConfigs) {
        if (!queues[pair.first].empty()) {
            return pair.first;
        }
    }
    return -1;  // No queue has processes
}

void MultilevelQueueScheduler::scheduleFromQueue(int queuePriority, 
                                                  std::shared_ptr<Process> process) {
    const QueueConfig& config = queueConfigs[queuePriority];
    
    if (config.algorithm == QueueSchedulingAlgorithm::FCFS) {
        // Run to completion
        int burstTime = process->getRemainingTime();
        process->execute(burstTime);
        
        for (int i = 0; i < burstTime; i++) {
            ganttChart.push_back(process->getName());
        }
        
        // Update waiting times for other ready processes
        for (auto& p : processes) {
            if (p != process && p->getState() == ProcessState::READY) {
                p->addWaitingTime(burstTime);
            }
        }
        
        currentTime += burstTime;
        
    } else if (config.algorithm == QueueSchedulingAlgorithm::ROUND_ROBIN) {
        // Execute for time quantum
        int executionTime = process->execute(config.timeQuantum);
        
        for (int i = 0; i < executionTime; i++) {
            ganttChart.push_back(process->getName());
        }
        
        // Update waiting times
        for (auto& p : processes) {
            if (p != process && p->getState() == ProcessState::READY) {
                p->addWaitingTime(executionTime);
            }
        }
        
        currentTime += executionTime;
    }
}

void MultilevelQueueScheduler::schedule() {
    currentTime = 0;
    ganttChart.clear();
    
    // Find the earliest arrival time
    int earliestArrival = INT_MAX;
    for (const auto& process : processes) {
        earliestArrival = std::min(earliestArrival, process->getArrivalTime());
    }
    currentTime = earliestArrival;
    
    bool allComplete = false;
    
    while (!allComplete) {
        // Admit any processes that have arrived
        admitArrivingProcesses();
        
        // Add ready processes to their respective queues based on priority
        for (auto& process : processes) {
            if (process->getState() == ProcessState::READY && 
                process->getLastScheduledTime() < currentTime) {
                
                // Find which queue this process belongs to
                int targetQueue = -1;
                for (const auto& pair : queueConfigs) {
                    if (process->getPriority() <= pair.first) {
                        targetQueue = pair.first;
                        break;
                    }
                }
                
                // If no matching queue, use lowest priority queue
                if (targetQueue == -1 && !queueConfigs.empty()) {
                    targetQueue = queueConfigs.rbegin()->first;
                }
                
                if (targetQueue != -1) {
                    // Check if process is not already in queue
                    bool inQueue = false;
                    std::queue<std::shared_ptr<Process>> tempQueue = queues[targetQueue];
                    while (!tempQueue.empty()) {
                        if (tempQueue.front() == process) {
                            inQueue = true;
                            break;
                        }
                        tempQueue.pop();
                    }
                    
                    if (!inQueue) {
                        queues[targetQueue].push(process);
                        process->setLastScheduledTime(currentTime);
                    }
                }
            }
        }
        
        // Get highest priority non-empty queue
        int queueToSchedule = getHighestPriorityQueue();
        
        // If no queue has processes, advance time to next arrival
        if (queueToSchedule == -1) {
            int nextArrival = INT_MAX;
            for (const auto& process : processes) {
                if (process->getState() == ProcessState::NEW) {
                    nextArrival = std::min(nextArrival, process->getArrivalTime());
                }
            }
            
            if (nextArrival != INT_MAX) {
                ganttChart.push_back("IDLE");
                currentTime = nextArrival;
                continue;
            } else {
                allComplete = true;
                break;
            }
        }
        
        // Get next process from the selected queue
        std::shared_ptr<Process> process = queues[queueToSchedule].front();
        queues[queueToSchedule].pop();
        
        // Context switch
        contextSwitch(currentProcess, process);
        
        // Schedule using the queue's algorithm
        scheduleFromQueue(queueToSchedule, process);
        
        // Check if process is complete
        if (process->isComplete()) {
            process->setCompletionTime(currentTime);
            process->calculateMetrics();
            process->setState(ProcessState::TERMINATED);
        } else {
            // If using Round Robin and process not complete, re-add to queue
            const QueueConfig& config = queueConfigs[queueToSchedule];
            if (config.algorithm == QueueSchedulingAlgorithm::ROUND_ROBIN) {
                process->setState(ProcessState::READY);
                
                // Admit new arrivals before re-queueing
                admitArrivingProcesses();
                for (auto& p : processes) {
                    if (p->getState() == ProcessState::READY && 
                        p != process &&
                        p->getLastScheduledTime() < currentTime) {
                        
                        int targetQueue = -1;
                        for (const auto& pair : queueConfigs) {
                            if (p->getPriority() <= pair.first) {
                                targetQueue = pair.first;
                                break;
                            }
                        }
                        
                        if (targetQueue == -1 && !queueConfigs.empty()) {
                            targetQueue = queueConfigs.rbegin()->first;
                        }
                        
                        if (targetQueue != -1) {
                            bool inQueue = false;
                            std::queue<std::shared_ptr<Process>> tempQueue = queues[targetQueue];
                            while (!tempQueue.empty()) {
                                if (tempQueue.front() == p) {
                                    inQueue = true;
                                    break;
                                }
                                tempQueue.pop();
                            }
                            
                            if (!inQueue) {
                                queues[targetQueue].push(p);
                                p->setLastScheduledTime(currentTime);
                            }
                        }
                    }
                }
                
                queues[queueToSchedule].push(process);
                process->setLastScheduledTime(currentTime);
            }
        }
        
        currentProcess = nullptr;
        
        // Check if all processes are complete
        allComplete = true;
        for (const auto& p : processes) {
            if (p->getState() != ProcessState::TERMINATED) {
                allComplete = false;
                break;
            }
        }
    }
}

std::string MultilevelQueueScheduler::getGanttChart() const {
    std::stringstream ss;
    ss << "\nGantt Chart:\n";
    ss << std::string(80, '-') << "\n";
    
    if (ganttChart.empty()) {
        ss << "No execution recorded\n";
        return ss.str();
    }
    
    ss << "Time |";
    for (size_t i = 0; i < ganttChart.size() && i < 60; i++) {
        ss << " ";
    }
    ss << "\n     |";
    
    for (size_t i = 0; i < ganttChart.size() && i < 60; i++) {
        if (ganttChart[i] == "IDLE") {
            ss << "-";
        } else {
            ss << ganttChart[i][0];
        }
    }
    ss << "\n";
    
    ss << "  0  ";
    for (size_t i = 0; i < ganttChart.size() && i < 60; i += 5) {
        ss << std::setw(5) << (i + 5);
    }
    ss << "\n";
    
    ss << std::string(80, '-') << "\n";
    return ss.str();
}
