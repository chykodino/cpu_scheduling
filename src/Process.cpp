#include "Process.h"
#include <iostream>
#include <iomanip>

/**
 * @file Process.cpp
 * @brief Implementation of the Process class
 */

Process::Process(int pid, const std::string& name, int arrivalTime, 
                 int burstTime, int priority)
    : pid(pid), name(name), arrivalTime(arrivalTime), burstTime(burstTime),
      remainingTime(burstTime), priority(priority), state(ProcessState::NEW),
      startTime(-1), completionTime(-1), waitingTime(0), turnaroundTime(0),
      responseTime(0), lastScheduledTime(arrivalTime), firstSchedule(true) {
}

int Process::execute(int quantum) {
    // Calculate actual execution time (may be less than quantum if process finishes)
    int executionTime = std::min(quantum, remainingTime);
    
    // Decrement remaining time
    remainingTime -= executionTime;
    
    return executionTime;
}

void Process::calculateMetrics() {
    // Turnaround Time = Completion Time - Arrival Time
    turnaroundTime = completionTime - arrivalTime;
    
    // Response Time = Start Time - Arrival Time
    responseTime = startTime - arrivalTime;
    
    // Waiting Time is already tracked incrementally during scheduling
    // Alternative calculation: Waiting Time = Turnaround Time - Burst Time
    // We use the tracked value for more accuracy with context switches
}

void Process::reset() {
    remainingTime = burstTime;
    state = ProcessState::NEW;
    startTime = -1;
    completionTime = -1;
    waitingTime = 0;
    turnaroundTime = 0;
    responseTime = 0;
    lastScheduledTime = arrivalTime;
    firstSchedule = true;
}

std::string Process::getStateString() const {
    switch(state) {
        case ProcessState::NEW:
            return "NEW";
        case ProcessState::READY:
            return "READY";
        case ProcessState::RUNNING:
            return "RUNNING";
        case ProcessState::WAITING:
            return "WAITING";
        case ProcessState::TERMINATED:
            return "TERMINATED";
        default:
            return "UNKNOWN";
    }
}

void Process::display() const {
    std::cout << "Process " << std::setw(3) << pid 
              << " (" << std::setw(8) << name << ")"
              << " | Arrival: " << std::setw(3) << arrivalTime
              << " | Burst: " << std::setw(3) << burstTime
              << " | Priority: " << std::setw(2) << priority
              << " | State: " << std::setw(10) << getStateString();
    
    if (state == ProcessState::TERMINATED) {
        std::cout << " | WT: " << std::setw(3) << waitingTime
                  << " | TAT: " << std::setw(3) << turnaroundTime
                  << " | RT: " << std::setw(3) << responseTime;
    }
    
    std::cout << std::endl;
}
