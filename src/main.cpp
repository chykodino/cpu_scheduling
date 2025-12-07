#include "Process.h"
#include "RoundRobinScheduler.h"
#include "PriorityScheduler.h"
#include "MultilevelQueueScheduler.h"
#include "MultilevelFeedbackQueueScheduler.h"
#include <iostream>
#include <iomanip>
#include <memory>
#include <vector>

/**
 * @file main.cpp
 * @brief Main program demonstrating all CPU scheduling algorithms
 * 
 * This program creates a set of sample processes and runs them through
 * various scheduling algorithms, displaying performance metrics for comparison.
 */

/**
 * @brief Create a standard set of test processes
 * 
 * Creates 5 processes with varying arrival times, burst times, and priorities
 * for demonstration purposes.
 * 
 * @return std::vector<std::shared_ptr<Process>> Vector of test processes
 */
std::vector<std::shared_ptr<Process>> createTestProcesses() {
    std::vector<std::shared_ptr<Process>> processes;
    
    // Create sample processes
    // Process(PID, Name, ArrivalTime, BurstTime, Priority)
    processes.push_back(std::make_shared<Process>(1, "P1", 0, 10, 2));
    processes.push_back(std::make_shared<Process>(2, "P2", 1, 5, 1));
    processes.push_back(std::make_shared<Process>(3, "P3", 2, 8, 3));
    processes.push_back(std::make_shared<Process>(4, "P4", 3, 4, 2));
    processes.push_back(std::make_shared<Process>(5, "P5", 4, 6, 1));
    
    return processes;
}

/**
 * @brief Display menu and get user choice
 * 
 * @return int User's menu selection
 */
int displayMenu() {
    std::cout << "\n" << std::string(80, '=') << "\n";
    std::cout << "CPU SCHEDULER SIMULATOR\n";
    std::cout << std::string(80, '=') << "\n\n";
    std::cout << "Select a scheduling algorithm:\n";
    std::cout << "1. Round Robin\n";
    std::cout << "2. Non-Preemptive Priority\n";
    std::cout << "3. Preemptive Priority\n";
    std::cout << "4. Multilevel Queue\n";
    std::cout << "5. Multilevel Feedback Queue\n";
    std::cout << "6. Compare All Algorithms\n";
    std::cout << "0. Exit\n";
    std::cout << "\nEnter your choice: ";
    
    int choice;
    std::cin >> choice;
    return choice;
}

/**
 * @brief Run Round Robin scheduler
 */
void runRoundRobin() {
    auto processes = createTestProcesses();
    
    int quantum;
    std::cout << "\nEnter time quantum (recommended: 2-4): ";
    std::cin >> quantum;
    
    RoundRobinScheduler scheduler(quantum, 0);
    
    for (const auto& process : processes) {
        scheduler.addProcess(process);
    }
    
    std::cout << "\nRunning Round Robin Scheduler...\n";
    scheduler.schedule();
    scheduler.displayResults();
    std::cout << scheduler.getGanttChart();
}

/**
 * @brief Run Priority scheduler
 */
void runPriority(bool preemptive) {
    auto processes = createTestProcesses();
    
    PriorityScheduler scheduler(preemptive, true, 5, 0);
    
    for (const auto& process : processes) {
        scheduler.addProcess(process);
    }
    
    std::cout << "\nRunning Priority Scheduler...\n";
    scheduler.schedule();
    scheduler.displayResults();
    std::cout << scheduler.getGanttChart();
}

/**
 * @brief Run Multilevel Queue scheduler
 */
void runMultilevelQueue() {
    auto processes = createTestProcesses();
    
    MultilevelQueueScheduler scheduler(0);
    
    // Configure queues
    // Queue 0: High priority, Round Robin with quantum 2
    scheduler.addQueueConfig(QueueConfig(0, QueueSchedulingAlgorithm::ROUND_ROBIN, 2));
    // Queue 1: Medium priority, Round Robin with quantum 4
    scheduler.addQueueConfig(QueueConfig(1, QueueSchedulingAlgorithm::ROUND_ROBIN, 4));
    // Queue 2: Low priority, FCFS
    scheduler.addQueueConfig(QueueConfig(2, QueueSchedulingAlgorithm::FCFS, 0));
    // Queue 3: Lowest priority, FCFS
    scheduler.addQueueConfig(QueueConfig(3, QueueSchedulingAlgorithm::FCFS, 0));
    
    for (const auto& process : processes) {
        scheduler.addProcess(process);
    }
    
    std::cout << "\nRunning Multilevel Queue Scheduler...\n";
    scheduler.schedule();
    scheduler.displayResults();
    std::cout << scheduler.getGanttChart();
}

/**
 * @brief Run Multilevel Feedback Queue scheduler
 */
void runMLFQ() {
    auto processes = createTestProcesses();
    
    int numQueues;
    std::cout << "\nEnter number of queues (recommended: 3): ";
    std::cin >> numQueues;
    
    MultilevelFeedbackQueueScheduler scheduler(numQueues, true, 10, 0);
    
    // Set custom time quanta if desired
    // Default is 2, 4, 6, 8, ...
    
    for (const auto& process : processes) {
        scheduler.addProcess(process);
    }
    
    std::cout << "\nRunning Multilevel Feedback Queue Scheduler...\n";
    scheduler.schedule();
    scheduler.displayResults();
    std::cout << scheduler.getGanttChart();
}

/**
 * @brief Compare all scheduling algorithms
 */
void compareAll() {
    std::cout << "\n" << std::string(80, '=') << "\n";
    std::cout << "COMPARING ALL SCHEDULING ALGORITHMS\n";
    std::cout << std::string(80, '=') << "\n";
    
    // Test processes (same set for fair comparison)
    auto testProcesses = createTestProcesses();
    
    std::vector<std::shared_ptr<Scheduler>> schedulers;
    
    // 1. Round Robin (quantum = 3)
    auto rr = std::make_shared<RoundRobinScheduler>(3, 0);
    for (const auto& p : testProcesses) {
        rr->addProcess(std::make_shared<Process>(*p));
    }
    schedulers.push_back(rr);
    
    // 2. Non-Preemptive Priority
    auto npp = std::make_shared<PriorityScheduler>(false, true, 5, 0);
    for (const auto& p : testProcesses) {
        npp->addProcess(std::make_shared<Process>(*p));
    }
    schedulers.push_back(npp);
    
    // 3. Preemptive Priority
    auto pp = std::make_shared<PriorityScheduler>(true, true, 5, 0);
    for (const auto& p : testProcesses) {
        pp->addProcess(std::make_shared<Process>(*p));
    }
    schedulers.push_back(pp);
    
    // 4. Multilevel Queue
    auto mlq = std::make_shared<MultilevelQueueScheduler>(0);
    mlq->addQueueConfig(QueueConfig(0, QueueSchedulingAlgorithm::ROUND_ROBIN, 2));
    mlq->addQueueConfig(QueueConfig(1, QueueSchedulingAlgorithm::ROUND_ROBIN, 4));
    mlq->addQueueConfig(QueueConfig(2, QueueSchedulingAlgorithm::FCFS, 0));
    mlq->addQueueConfig(QueueConfig(3, QueueSchedulingAlgorithm::FCFS, 0));
    for (const auto& p : testProcesses) {
        mlq->addProcess(std::make_shared<Process>(*p));
    }
    schedulers.push_back(mlq);
    
    // 5. MLFQ
    auto mlfq = std::make_shared<MultilevelFeedbackQueueScheduler>(3, true, 10, 0);
    for (const auto& p : testProcesses) {
        mlfq->addProcess(std::make_shared<Process>(*p));
    }
    schedulers.push_back(mlfq);
    
    // Run all schedulers
    for (auto& scheduler : schedulers) {
        scheduler->schedule();
        scheduler->displayResults();
    }
    
    // Comparison table
    std::cout << "\n" << std::string(80, '=') << "\n";
    std::cout << "PERFORMANCE COMPARISON\n";
    std::cout << std::string(80, '=') << "\n\n";
    
    std::cout << std::left << std::setw(35) << "Algorithm"
              << std::setw(12) << "Avg Wait"
              << std::setw(12) << "Avg TAT"
              << std::setw(12) << "Avg Resp"
              << std::setw(10) << "CPU %"
              << "\n";
    std::cout << std::string(80, '-') << "\n";
    
    for (const auto& scheduler : schedulers) {
        SchedulingMetrics metrics = scheduler->calculateMetrics();
        std::cout << std::fixed << std::setprecision(2);
        std::cout << std::left << std::setw(35) << scheduler->getName()
                  << std::setw(12) << metrics.averageWaitingTime
                  << std::setw(12) << metrics.averageTurnaroundTime
                  << std::setw(12) << metrics.averageResponseTime
                  << std::setw(10) << metrics.cpuUtilization
                  << "\n";
    }
    
    std::cout << std::string(80, '=') << "\n";
}

/**
 * @brief Main function
 */
int main() {
    while (true) {
        int choice = displayMenu();
        
        switch (choice) {
            case 1:
                runRoundRobin();
                break;
            case 2:
                runPriority(false);
                break;
            case 3:
                runPriority(true);
                break;
            case 4:
                runMultilevelQueue();
                break;
            case 5:
                runMLFQ();
                break;
            case 6:
                compareAll();
                break;
            case 0:
                std::cout << "\nThank you for using the CPU Scheduler Simulator!\n";
                return 0;
            default:
                std::cout << "\nInvalid choice. Please try again.\n";
        }
        
        std::cout << "\nPress Enter to continue...";
        std::cin.ignore();
        std::cin.get();
    }
    
    return 0;
}
