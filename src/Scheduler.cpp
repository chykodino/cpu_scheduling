#include "Scheduler.h"
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <climits>

/**
 * @file Scheduler.cpp
 * @brief Implementation of the base Scheduler class
 */

Scheduler::Scheduler(int contextSwitchOverhead)
    : currentTime(0), contextSwitchOverhead(contextSwitchOverhead),
      totalContextSwitches(0), currentProcess(nullptr) {
}

void Scheduler::addProcess(std::shared_ptr<Process> process) {
    processes.push_back(process);
}

void Scheduler::contextSwitch(std::shared_ptr<Process> from, 
                              std::shared_ptr<Process> to) {
    // Only count as context switch if actually switching between different processes
    if (from != to && from != nullptr && to != nullptr) {
        totalContextSwitches++;
        currentTime += contextSwitchOverhead;
    }
    
    // Update states
    if (from != nullptr && from->getState() == ProcessState::RUNNING) {
        from->setState(ProcessState::READY);
    }
    
    if (to != nullptr) {
        to->setState(ProcessState::RUNNING);
        
        // Record start time if first time being scheduled
        if (to->isFirstSchedule()) {
            to->setStartTime(currentTime);
            to->setFirstSchedule(false);
        }
    }
    
    currentProcess = to;
}

void Scheduler::updateWaitingTimes(int elapsedTime) {
    for (auto& process : processes) {
        // Only update waiting time for processes in READY state
        if (process->getState() == ProcessState::READY) {
            process->addWaitingTime(elapsedTime);
        }
    }
}

int Scheduler::admitArrivingProcesses() {
    int admitted = 0;
    for (auto& process : processes) {
        if (process->getState() == ProcessState::NEW && 
            process->getArrivalTime() <= currentTime) {
            process->setState(ProcessState::READY);
            admitted++;
        }
    }
    return admitted;
}

SchedulingMetrics Scheduler::calculateMetrics() const {
    SchedulingMetrics metrics;
    
    int totalWaiting = 0;
    int totalTurnaround = 0;
    int totalResponse = 0;
    int completedProcesses = 0;
    int maxCompletionTime = 0;
    int minArrivalTime = INT_MAX;
    int totalBurstTime = 0;
    
    for (const auto& process : processes) {
        if (process->getState() == ProcessState::TERMINATED) {
            totalWaiting += process->getWaitingTime();
            totalTurnaround += process->getTurnaroundTime();
            totalResponse += process->getResponseTime();
            completedProcesses++;
            
            maxCompletionTime = std::max(maxCompletionTime, process->getCompletionTime());
            minArrivalTime = std::min(minArrivalTime, process->getArrivalTime());
            totalBurstTime += process->getBurstTime();
        }
    }
    
    if (completedProcesses > 0) {
        metrics.averageWaitingTime = static_cast<double>(totalWaiting) / completedProcesses;
        metrics.averageTurnaroundTime = static_cast<double>(totalTurnaround) / completedProcesses;
        metrics.averageResponseTime = static_cast<double>(totalResponse) / completedProcesses;
    } else {
        metrics.averageWaitingTime = 0;
        metrics.averageTurnaroundTime = 0;
        metrics.averageResponseTime = 0;
    }
    
    // CPU Utilization = (Total Burst Time) / (Total Time) * 100
    int totalTime = maxCompletionTime - minArrivalTime;
    if (totalTime > 0) {
        metrics.cpuUtilization = (static_cast<double>(totalBurstTime) / totalTime) * 100.0;
    } else {
        metrics.cpuUtilization = 0;
    }
    
    // Throughput = Completed Processes / Total Time
    if (totalTime > 0) {
        metrics.throughput = static_cast<double>(completedProcesses) / totalTime;
    } else {
        metrics.throughput = 0;
    }
    
    metrics.totalContextSwitches = totalContextSwitches;
    metrics.totalTime = totalTime;
    
    return metrics;
}

void Scheduler::displayResults() const {
    std::cout << "\n" << std::string(80, '=') << "\n";
    std::cout << "SCHEDULING RESULTS: " << getName() << "\n";
    std::cout << std::string(80, '=') << "\n\n";
    
    // Display individual process results
    std::cout << "Individual Process Metrics:\n";
    std::cout << std::string(80, '-') << "\n";
    std::cout << std::left << std::setw(5) << "PID"
              << std::setw(12) << "Name"
              << std::setw(10) << "Arrival"
              << std::setw(10) << "Burst"
              << std::setw(10) << "Priority"
              << std::setw(10) << "Wait"
              << std::setw(10) << "TAT"
              << std::setw(10) << "Response"
              << "\n";
    std::cout << std::string(80, '-') << "\n";
    
    for (const auto& process : processes) {
        if (process->getState() == ProcessState::TERMINATED) {
            std::cout << std::left << std::setw(5) << process->getPID()
                      << std::setw(12) << process->getName()
                      << std::setw(10) << process->getArrivalTime()
                      << std::setw(10) << process->getBurstTime()
                      << std::setw(10) << process->getPriority()
                      << std::setw(10) << process->getWaitingTime()
                      << std::setw(10) << process->getTurnaroundTime()
                      << std::setw(10) << process->getResponseTime()
                      << "\n";
        }
    }
    
    // Display aggregate metrics
    SchedulingMetrics metrics = calculateMetrics();
    
    std::cout << "\n" << std::string(80, '-') << "\n";
    std::cout << "Aggregate Performance Metrics:\n";
    std::cout << std::string(80, '-') << "\n";
    std::cout << std::fixed << std::setprecision(2);
    std::cout << "Average Waiting Time:      " << std::setw(10) << metrics.averageWaitingTime << " time units\n";
    std::cout << "Average Turnaround Time:   " << std::setw(10) << metrics.averageTurnaroundTime << " time units\n";
    std::cout << "Average Response Time:     " << std::setw(10) << metrics.averageResponseTime << " time units\n";
    std::cout << "CPU Utilization:           " << std::setw(10) << metrics.cpuUtilization << " %\n";
    std::cout << "Throughput:                " << std::setw(10) << metrics.throughput << " processes/time unit\n";
    std::cout << "Total Context Switches:    " << std::setw(10) << metrics.totalContextSwitches << "\n";
    std::cout << "Total Simulation Time:     " << std::setw(10) << metrics.totalTime << " time units\n";
    std::cout << std::string(80, '=') << "\n\n";
}

std::string Scheduler::getGanttChart() const {
    // Base implementation - derived classes should override
    return "Gantt chart not implemented for this scheduler\n";
}

void Scheduler::reset() {
    currentTime = 0;
    totalContextSwitches = 0;
    currentProcess = nullptr;
    
    for (auto& process : processes) {
        process->reset();
    }
}
