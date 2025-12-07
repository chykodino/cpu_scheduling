#include "RoundRobinScheduler.h"
#include <iostream>
#include <sstream>
#include <climits>
#include <iomanip>

/**
 * @file RoundRobinScheduler.cpp
 * @brief Implementation of Round Robin scheduling algorithm
 */

RoundRobinScheduler::RoundRobinScheduler(int quantum, int contextSwitchOverhead)
    : Scheduler(contextSwitchOverhead), timeQuantum(quantum) {
}

std::string RoundRobinScheduler::getName() const {
    return "Round Robin (Quantum=" + std::to_string(timeQuantum) + ")";
}

void RoundRobinScheduler::schedule() {
    currentTime = 0;
    ganttChart.clear();
    
    // Find the earliest arrival time
    int earliestArrival = INT_MAX;
    for (const auto& process : processes) {
        earliestArrival = std::min(earliestArrival, process->getArrivalTime());
    }
    currentTime = earliestArrival;
    
    // Continue until all processes are complete
    bool allComplete = false;
    
    while (!allComplete) {
        // Admit any processes that have arrived
        admitArrivingProcesses();
        
        // Add newly ready processes to the queue
        for (auto& process : processes) {
            if (process->getState() == ProcessState::READY && 
                process->getLastScheduledTime() < currentTime) {
                
                // Check if process is not already in queue
                bool inQueue = false;
                std::queue<std::shared_ptr<Process>> tempQueue = readyQueue;
                while (!tempQueue.empty()) {
                    if (tempQueue.front() == process) {
                        inQueue = true;
                        break;
                    }
                    tempQueue.pop();
                }
                
                if (!inQueue) {
                    readyQueue.push(process);
                    process->setLastScheduledTime(currentTime);
                }
            }
        }
        
        // If ready queue is empty, advance time to next arrival
        if (readyQueue.empty()) {
            int nextArrival = INT_MAX;
            for (const auto& process : processes) {
                if (process->getState() == ProcessState::NEW) {
                    nextArrival = std::min(nextArrival, process->getArrivalTime());
                }
            }
            
            if (nextArrival != INT_MAX) {
                // Update waiting times for time skipped
                updateWaitingTimes(nextArrival - currentTime);
                currentTime = nextArrival;
                ganttChart.push_back("IDLE");
                continue;
            } else {
                // All processes complete
                allComplete = true;
                break;
            }
        }
        
        // Get next process from ready queue
        std::shared_ptr<Process> process = readyQueue.front();
        readyQueue.pop();
        
        // Context switch to this process
        contextSwitch(currentProcess, process);
        
        // Execute for time quantum or until completion
        int executionTime = process->execute(timeQuantum);
        
        // Record in Gantt chart
        for (int i = 0; i < executionTime; i++) {
            ganttChart.push_back(process->getName());
        }
        
        // Update current time
        currentTime += executionTime;
        
        // Update waiting times for other processes
        for (auto& p : processes) {
            if (p != process && p->getState() == ProcessState::READY) {
                p->addWaitingTime(executionTime);
            }
        }
        
        // Check if process is complete
        if (process->isComplete()) {
            process->setCompletionTime(currentTime);
            process->calculateMetrics();
            process->setState(ProcessState::TERMINATED);
        } else {
            // Process not complete, add back to ready queue
            process->setState(ProcessState::READY);
            
            // Admit any new arrivals before re-queueing
            admitArrivingProcesses();
            for (auto& p : processes) {
                if (p->getState() == ProcessState::READY && 
                    p != process &&
                    p->getLastScheduledTime() < currentTime) {
                    
                    bool inQueue = false;
                    std::queue<std::shared_ptr<Process>> tempQueue = readyQueue;
                    while (!tempQueue.empty()) {
                        if (tempQueue.front() == p) {
                            inQueue = true;
                            break;
                        }
                        tempQueue.pop();
                    }
                    
                    if (!inQueue) {
                        readyQueue.push(p);
                        p->setLastScheduledTime(currentTime);
                    }
                }
            }
            
            // Add current process back to queue
            readyQueue.push(process);
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

std::string RoundRobinScheduler::getGanttChart() const {
    std::stringstream ss;
    ss << "\nGantt Chart:\n";
    ss << std::string(80, '-') << "\n";
    
    if (ganttChart.empty()) {
        ss << "No execution recorded\n";
        return ss.str();
    }
    
    // Display compact Gantt chart
    ss << "Time |";
    for (size_t i = 0; i < ganttChart.size() && i < 60; i++) {
        ss << " ";
    }
    ss << "\n     |";
    
    for (size_t i = 0; i < ganttChart.size() && i < 60; i++) {
        if (ganttChart[i] == "IDLE") {
            ss << "-";
        } else {
            ss << ganttChart[i][0];  // First letter of process name
        }
    }
    ss << "\n";
    
    // Display timeline
    ss << "  0  ";
    for (size_t i = 0; i < ganttChart.size() && i < 60; i += 5) {
        ss << std::setw(5) << (i + 5);
    }
    ss << "\n";
    
    ss << std::string(80, '-') << "\n";
    return ss.str();
}
