#include "PriorityScheduler.h"
#include <iostream>
#include <sstream>
#include <algorithm>
#include <climits>
#include <iomanip>

/**
 * @file PriorityScheduler.cpp
 * @brief Implementation of Priority scheduling algorithm (both preemptive and non-preemptive)
 */

PriorityScheduler::PriorityScheduler(bool preemptive, bool enableAging,
                                     int agingInterval, int contextSwitchOverhead)
    : Scheduler(contextSwitchOverhead), preemptive(preemptive),
      agingEnabled(enableAging), agingInterval(agingInterval) {
}

std::string PriorityScheduler::getName() const {
    std::string mode = preemptive ? "Preemptive" : "Non-Preemptive";
    std::string aging = agingEnabled ? " with Aging" : "";
    return mode + " Priority" + aging;
}

void PriorityScheduler::applyAging() {
    if (!agingEnabled) return;
    
    // Boost priority of processes that have been waiting
    for (auto& process : processes) {
        if (process->getState() == ProcessState::READY) {
            int waitingTime = currentTime - process->getLastScheduledTime();
            if (waitingTime >= agingInterval && process->getPriority() > 0) {
                process->setPriority(process->getPriority() - 1);  // Increase priority
            }
        }
    }
}

void PriorityScheduler::schedule() {
    currentTime = 0;
    ganttChart.clear();
    
    // Find the earliest arrival time
    int earliestArrival = INT_MAX;
    for (const auto& process : processes) {
        earliestArrival = std::min(earliestArrival, process->getArrivalTime());
    }
    currentTime = earliestArrival;
    
    bool allComplete = false;
    std::shared_ptr<Process> runningProcess = nullptr;
    
    while (!allComplete) {
        // Admit any processes that have arrived
        admitArrivingProcesses();
        
        // Apply aging if enabled
        if (currentTime % agingInterval == 0) {
            applyAging();
        }
        
        // Build priority queue with all ready processes
        // Clear the queue first
        while (!readyQueue.empty()) {
            readyQueue.pop();
        }
        
        for (auto& process : processes) {
            if (process->getState() == ProcessState::READY) {
                readyQueue.push(process);
            }
        }
        
        // If ready queue is empty, advance time to next arrival
        if (readyQueue.empty()) {
            if (runningProcess != nullptr && !runningProcess->isComplete()) {
                // Continue running current process in non-preemptive mode
                if (!preemptive) {
                    // Execute one time unit
                    runningProcess->execute(1);
                    ganttChart.push_back(runningProcess->getName());
                    currentTime++;
                    
                    if (runningProcess->isComplete()) {
                        runningProcess->setCompletionTime(currentTime);
                        runningProcess->calculateMetrics();
                        runningProcess->setState(ProcessState::TERMINATED);
                        runningProcess = nullptr;
                    }
                    continue;
                }
            }
            
            // Find next arrival
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
        
        // Get highest priority process
        std::shared_ptr<Process> nextProcess = readyQueue.top();
        
        // In preemptive mode, check if we need to preempt current process
        if (preemptive) {
            if (runningProcess != nullptr && runningProcess != nextProcess) {
                // Preempt if next process has higher priority
                if (nextProcess->getPriority() < runningProcess->getPriority()) {
                    runningProcess->setState(ProcessState::READY);
                    contextSwitch(runningProcess, nextProcess);
                    runningProcess = nextProcess;
                }
            } else if (runningProcess == nullptr) {
                contextSwitch(nullptr, nextProcess);
                runningProcess = nextProcess;
            }
            
            readyQueue.pop();
            
            // Execute one time unit
            runningProcess->execute(1);
            ganttChart.push_back(runningProcess->getName());
            
            // Update waiting times
            for (auto& p : processes) {
                if (p != runningProcess && p->getState() == ProcessState::READY) {
                    p->addWaitingTime(1);
                }
            }
            
            currentTime++;
            
            // Check if process is complete
            if (runningProcess->isComplete()) {
                runningProcess->setCompletionTime(currentTime);
                runningProcess->calculateMetrics();
                runningProcess->setState(ProcessState::TERMINATED);
                runningProcess = nullptr;
            }
            
        } else {
            // Non-preemptive mode: run process to completion
            readyQueue.pop();
            
            contextSwitch(runningProcess, nextProcess);
            runningProcess = nextProcess;
            
            // Execute entire burst
            int burstTime = runningProcess->getRemainingTime();
            runningProcess->execute(burstTime);
            
            for (int i = 0; i < burstTime; i++) {
                ganttChart.push_back(runningProcess->getName());
            }
            
            // Update waiting times
            for (auto& p : processes) {
                if (p != runningProcess && p->getState() == ProcessState::READY) {
                    p->addWaitingTime(burstTime);
                }
            }
            
            currentTime += burstTime;
            
            // Process is complete
            runningProcess->setCompletionTime(currentTime);
            runningProcess->calculateMetrics();
            runningProcess->setState(ProcessState::TERMINATED);
            runningProcess = nullptr;
        }
        
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

std::string PriorityScheduler::getGanttChart() const {
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
