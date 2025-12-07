#include "MultilevelFeedbackQueueScheduler.h"
#include <iostream>
#include <sstream>
#include <algorithm>
#include <climits>
#include <iomanip>

/**
 * @file MultilevelFeedbackQueueScheduler.cpp
 * @brief Implementation of Multilevel Feedback Queue scheduling algorithm
 */

MultilevelFeedbackQueueScheduler::MultilevelFeedbackQueueScheduler(
    int numQueues, bool enableAging, int agingThreshold, int contextSwitchOverhead)
    : Scheduler(contextSwitchOverhead), numQueues(numQueues),
      agingEnabled(enableAging), agingThreshold(agingThreshold) {
    
    // Initialize queues
    queues.resize(numQueues);
    
    // Set default time quanta (increasing for lower priority queues)
    timeQuantums.resize(numQueues);
    for (int i = 0; i < numQueues; i++) {
        timeQuantums[i] = 2 * (i + 1);  // 2, 4, 6, 8, ...
    }
}

void MultilevelFeedbackQueueScheduler::setTimeQuantum(int queueIndex, int quantum) {
    if (queueIndex >= 0 && queueIndex < numQueues) {
        timeQuantums[queueIndex] = quantum;
    }
}

std::string MultilevelFeedbackQueueScheduler::getName() const {
    std::string aging = agingEnabled ? " with Aging" : "";
    return "Multilevel Feedback Queue (" + std::to_string(numQueues) + " levels)" + aging;
}

int MultilevelFeedbackQueueScheduler::getHighestPriorityQueue() {
    for (int i = 0; i < numQueues; i++) {
        if (!queues[i].empty()) {
            return i;
        }
    }
    return -1;
}

void MultilevelFeedbackQueueScheduler::demoteProcess(std::shared_ptr<Process> process) {
    int currentLevel = processQueueLevel[process->getPID()];
    if (currentLevel < numQueues - 1) {
        processQueueLevel[process->getPID()] = currentLevel + 1;
        timeInQueue[process->getPID()] = 0;
    }
}

void MultilevelFeedbackQueueScheduler::promoteProcess(std::shared_ptr<Process> process) {
    int currentLevel = processQueueLevel[process->getPID()];
    if (currentLevel > 0) {
        processQueueLevel[process->getPID()] = currentLevel - 1;
        timeInQueue[process->getPID()] = 0;
    }
}

void MultilevelFeedbackQueueScheduler::applyAging() {
    if (!agingEnabled) return;
    
    for (auto& process : processes) {
        if (process->getState() == ProcessState::READY) {
            int pid = process->getPID();
            timeInQueue[pid]++;
            
            if (timeInQueue[pid] >= agingThreshold) {
                promoteProcess(process);
            }
        }
    }
}

void MultilevelFeedbackQueueScheduler::schedule() {
    currentTime = 0;
    ganttChart.clear();
    processQueueLevel.clear();
    timeInQueue.clear();
    
    // Initialize all processes to highest priority queue (level 0)
    for (const auto& process : processes) {
        processQueueLevel[process->getPID()] = 0;
        timeInQueue[process->getPID()] = 0;
    }
    
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
        
        // Apply aging periodically
        if (currentTime % agingThreshold == 0) {
            applyAging();
        }
        
        // Add ready processes to their appropriate queues
        for (auto& process : processes) {
            if (process->getState() == ProcessState::READY && 
                process->getLastScheduledTime() < currentTime) {
                
                int queueLevel = processQueueLevel[process->getPID()];
                
                // Check if process is not already in queue
                bool inQueue = false;
                std::queue<std::shared_ptr<Process>> tempQueue = queues[queueLevel];
                while (!tempQueue.empty()) {
                    if (tempQueue.front() == process) {
                        inQueue = true;
                        break;
                    }
                    tempQueue.pop();
                }
                
                if (!inQueue) {
                    queues[queueLevel].push(process);
                    process->setLastScheduledTime(currentTime);
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
        
        // Get time quantum for this queue
        int quantum = timeQuantums[queueToSchedule];
        
        // Execute for time quantum or until completion
        int executionTime = process->execute(quantum);
        
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
        
        // Check if process is complete
        if (process->isComplete()) {
            process->setCompletionTime(currentTime);
            process->calculateMetrics();
            process->setState(ProcessState::TERMINATED);
        } else {
            process->setState(ProcessState::READY);
            
            // If process used full quantum, demote it
            if (executionTime == quantum) {
                demoteProcess(process);
            }
            
            // Admit new arrivals before re-queueing
            admitArrivingProcesses();
            for (auto& p : processes) {
                if (p->getState() == ProcessState::READY && 
                    p != process &&
                    p->getLastScheduledTime() < currentTime) {
                    
                    int queueLevel = processQueueLevel[p->getPID()];
                    
                    bool inQueue = false;
                    std::queue<std::shared_ptr<Process>> tempQueue = queues[queueLevel];
                    while (!tempQueue.empty()) {
                        if (tempQueue.front() == p) {
                            inQueue = true;
                            break;
                        }
                        tempQueue.pop();
                    }
                    
                    if (!inQueue) {
                        queues[queueLevel].push(p);
                        p->setLastScheduledTime(currentTime);
                    }
                }
            }
            
            // Re-queue the process at its (possibly new) level
            int newLevel = processQueueLevel[process->getPID()];
            queues[newLevel].push(process);
            process->setLastScheduledTime(currentTime);
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

std::string MultilevelFeedbackQueueScheduler::getGanttChart() const {
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
