#include "../include/Process.h"
#include "../include/RoundRobinScheduler.h"
#include "../include/PriorityScheduler.h"
#include "../include/MultilevelQueueScheduler.h"
#include "../include/MultilevelFeedbackQueueScheduler.h"
#include <iostream>
#include <cassert>
#include <memory>
#include <cmath>

/**
 * @file test_scheduler.cpp
 * @brief Comprehensive test suite for CPU Scheduler Simulator
 * 
 * This file contains unit tests and system tests for all scheduling algorithms
 * and their components. Tests verify correctness of scheduling logic, metric
 * calculations, and edge cases.
 */

// Test counter
int tests_passed = 0;
int tests_failed = 0;

/**
 * @brief Assert macro with custom message
 */
#define TEST_ASSERT(condition, message) \
    do { \
        if (!(condition)) { \
            std::cerr << "FAIL: " << message << std::endl; \
            std::cerr << "  at " << __FILE__ << ":" << __LINE__ << std::endl; \
            tests_failed++; \
            return false; \
        } \
    } while(0)

/**
 * @brief Run a test function
 */
#define RUN_TEST(test_func) \
    do { \
        std::cout << "Running " << #test_func << "... "; \
        if (test_func()) { \
            std::cout << "PASSED" << std::endl; \
            tests_passed++; \
        } \
    } while(0)

// ============================================================================
// Process Class Tests
// ============================================================================

/**
 * @brief Test Process construction and basic getters
 */
bool test_process_creation() {
    Process p(1, "TestProcess", 0, 10, 2);
    
    TEST_ASSERT(p.getPID() == 1, "PID should be 1");
    TEST_ASSERT(p.getName() == "TestProcess", "Name should be TestProcess");
    TEST_ASSERT(p.getArrivalTime() == 0, "Arrival time should be 0");
    TEST_ASSERT(p.getBurstTime() == 10, "Burst time should be 10");
    TEST_ASSERT(p.getRemainingTime() == 10, "Remaining time should be 10");
    TEST_ASSERT(p.getPriority() == 2, "Priority should be 2");
    TEST_ASSERT(p.getState() == ProcessState::NEW, "Initial state should be NEW");
    
    return true;
}

/**
 * @brief Test Process execution
 */
bool test_process_execution() {
    Process p(1, "TestProcess", 0, 10, 2);
    
    int executed = p.execute(5);
    TEST_ASSERT(executed == 5, "Should execute 5 time units");
    TEST_ASSERT(p.getRemainingTime() == 5, "Remaining time should be 5");
    TEST_ASSERT(!p.isComplete(), "Process should not be complete");
    
    executed = p.execute(5);
    TEST_ASSERT(executed == 5, "Should execute 5 time units");
    TEST_ASSERT(p.getRemainingTime() == 0, "Remaining time should be 0");
    TEST_ASSERT(p.isComplete(), "Process should be complete");
    
    return true;
}

/**
 * @brief Test Process execution with quantum larger than remaining time
 */
bool test_process_execution_overflow() {
    Process p(1, "TestProcess", 0, 5, 2);
    
    int executed = p.execute(10);
    TEST_ASSERT(executed == 5, "Should execute only 5 time units (remaining time)");
    TEST_ASSERT(p.getRemainingTime() == 0, "Remaining time should be 0");
    TEST_ASSERT(p.isComplete(), "Process should be complete");
    
    return true;
}

/**
 * @brief Test metric calculation
 */
bool test_process_metrics() {
    Process p(1, "TestProcess", 5, 10, 2);
    
    p.setStartTime(8);
    p.setCompletionTime(20);
    p.addWaitingTime(5);
    p.calculateMetrics();
    
    TEST_ASSERT(p.getResponseTime() == 3, "Response time should be 3 (8-5)");
    TEST_ASSERT(p.getTurnaroundTime() == 15, "Turnaround time should be 15 (20-5)");
    TEST_ASSERT(p.getWaitingTime() == 5, "Waiting time should be 5");
    
    return true;
}

/**
 * @brief Test process reset
 */
bool test_process_reset() {
    Process p(1, "TestProcess", 5, 10, 2);
    
    p.execute(5);
    p.setCompletionTime(15);
    p.setState(ProcessState::TERMINATED);
    
    p.reset();
    
    TEST_ASSERT(p.getRemainingTime() == 10, "Remaining time should be reset to 10");
    TEST_ASSERT(p.getState() == ProcessState::NEW, "State should be reset to NEW");
    TEST_ASSERT(p.getWaitingTime() == 0, "Waiting time should be reset to 0");
    
    return true;
}

// ============================================================================
// Round Robin Scheduler Tests
// ============================================================================

/**
 * @brief Test Round Robin scheduling with simple workload
 */
bool test_round_robin_basic() {
    RoundRobinScheduler scheduler(4, 0);
    
    scheduler.addProcess(std::make_shared<Process>(1, "P1", 0, 8, 0));
    scheduler.addProcess(std::make_shared<Process>(2, "P2", 0, 4, 0));
    scheduler.addProcess(std::make_shared<Process>(3, "P3", 0, 2, 0));
    
    scheduler.schedule();
    
    SchedulingMetrics metrics = scheduler.calculateMetrics();
    
    // All processes should complete
    for (const auto& p : scheduler.getProcesses()) {
        TEST_ASSERT(p->getState() == ProcessState::TERMINATED, 
                   "All processes should be terminated");
    }
    
    // Check reasonable metrics
    TEST_ASSERT(metrics.averageWaitingTime >= 0, "Average waiting time should be non-negative");
    TEST_ASSERT(metrics.averageTurnaroundTime > 0, "Average turnaround time should be positive");
    TEST_ASSERT(metrics.cpuUtilization > 90, "CPU utilization should be high");
    
    return true;
}

/**
 * @brief Test Round Robin with different arrival times
 */
bool test_round_robin_arrivals() {
    RoundRobinScheduler scheduler(3, 0);
    
    scheduler.addProcess(std::make_shared<Process>(1, "P1", 0, 6, 0));
    scheduler.addProcess(std::make_shared<Process>(2, "P2", 2, 4, 0));
    scheduler.addProcess(std::make_shared<Process>(3, "P3", 4, 2, 0));
    
    scheduler.schedule();
    
    // All processes should complete
    for (const auto& p : scheduler.getProcesses()) {
        TEST_ASSERT(p->getState() == ProcessState::TERMINATED, 
                   "All processes should be terminated");
        TEST_ASSERT(p->getCompletionTime() >= p->getArrivalTime() + p->getBurstTime(),
                   "Completion time should be at least arrival + burst time");
    }
    
    return true;
}

// ============================================================================
// Priority Scheduler Tests
// ============================================================================

/**
 * @brief Test Non-Preemptive Priority Scheduling
 */
bool test_priority_non_preemptive() {
    PriorityScheduler scheduler(false, false, 5, 0);
    
    scheduler.addProcess(std::make_shared<Process>(1, "P1", 0, 5, 3));  // Low priority
    scheduler.addProcess(std::make_shared<Process>(2, "P2", 1, 3, 1));  // High priority
    scheduler.addProcess(std::make_shared<Process>(3, "P3", 2, 2, 2));  // Medium priority
    
    scheduler.schedule();
    
    // All processes should complete
    for (const auto& p : scheduler.getProcesses()) {
        TEST_ASSERT(p->getState() == ProcessState::TERMINATED, 
                   "All processes should be terminated");
    }
    
    // P1 starts first (arrives at 0), then P2 (higher priority), then P3
    auto processes = scheduler.getProcesses();
    TEST_ASSERT(processes[0]->getCompletionTime() == 5, "P1 should complete at time 5");
    
    return true;
}

/**
 * @brief Test Preemptive Priority Scheduling
 */
bool test_priority_preemptive() {
    PriorityScheduler scheduler(true, false, 5, 0);
    
    scheduler.addProcess(std::make_shared<Process>(1, "P1", 0, 8, 3));  // Low priority
    scheduler.addProcess(std::make_shared<Process>(2, "P2", 1, 4, 1));  // High priority
    scheduler.addProcess(std::make_shared<Process>(3, "P3", 5, 2, 2));  // Medium priority
    
    scheduler.schedule();
    
    // All processes should complete
    for (const auto& p : scheduler.getProcesses()) {
        TEST_ASSERT(p->getState() == ProcessState::TERMINATED, 
                   "All processes should be terminated");
    }
    
    return true;
}

/**
 * @brief Test Priority Scheduling with Aging
 */
bool test_priority_aging() {
    PriorityScheduler scheduler(false, true, 3, 0);
    
    scheduler.addProcess(std::make_shared<Process>(1, "P1", 0, 10, 5));  // Very low priority
    scheduler.addProcess(std::make_shared<Process>(2, "P2", 1, 2, 1));   // High priority
    scheduler.addProcess(std::make_shared<Process>(3, "P3", 2, 2, 1));   // High priority
    
    scheduler.schedule();
    
    // All processes should complete (aging prevents starvation)
    for (const auto& p : scheduler.getProcesses()) {
        TEST_ASSERT(p->getState() == ProcessState::TERMINATED, 
                   "All processes should be terminated (aging should prevent starvation)");
    }
    
    return true;
}

// ============================================================================
// Multilevel Queue Scheduler Tests
// ============================================================================

/**
 * @brief Test Multilevel Queue Scheduling
 */
bool test_multilevel_queue() {
    MultilevelQueueScheduler scheduler(0);
    
    scheduler.addQueueConfig(QueueConfig(0, QueueSchedulingAlgorithm::ROUND_ROBIN, 2));
    scheduler.addQueueConfig(QueueConfig(1, QueueSchedulingAlgorithm::ROUND_ROBIN, 4));
    scheduler.addQueueConfig(QueueConfig(2, QueueSchedulingAlgorithm::FCFS, 0));
    
    scheduler.addProcess(std::make_shared<Process>(1, "P1", 0, 6, 0));  // Queue 0
    scheduler.addProcess(std::make_shared<Process>(2, "P2", 0, 4, 1));  // Queue 1
    scheduler.addProcess(std::make_shared<Process>(3, "P3", 0, 2, 2));  // Queue 2
    
    scheduler.schedule();
    
    // All processes should complete
    for (const auto& p : scheduler.getProcesses()) {
        TEST_ASSERT(p->getState() == ProcessState::TERMINATED, 
                   "All processes should be terminated");
    }
    
    return true;
}

// ============================================================================
// Multilevel Feedback Queue Scheduler Tests
// ============================================================================

/**
 * @brief Test MLFQ basic functionality
 */
bool test_mlfq_basic() {
    MultilevelFeedbackQueueScheduler scheduler(3, false, 10, 0);
    
    scheduler.setTimeQuantum(0, 2);
    scheduler.setTimeQuantum(1, 4);
    scheduler.setTimeQuantum(2, 8);
    
    scheduler.addProcess(std::make_shared<Process>(1, "P1", 0, 10, 0));
    scheduler.addProcess(std::make_shared<Process>(2, "P2", 1, 3, 0));
    scheduler.addProcess(std::make_shared<Process>(3, "P3", 2, 6, 0));
    
    scheduler.schedule();
    
    // All processes should complete
    for (const auto& p : scheduler.getProcesses()) {
        TEST_ASSERT(p->getState() == ProcessState::TERMINATED, 
                   "All processes should be terminated");
    }
    
    return true;
}

/**
 * @brief Test MLFQ with aging
 */
bool test_mlfq_aging() {
    MultilevelFeedbackQueueScheduler scheduler(3, true, 5, 0);
    
    scheduler.addProcess(std::make_shared<Process>(1, "P1", 0, 15, 0));
    scheduler.addProcess(std::make_shared<Process>(2, "P2", 1, 2, 0));
    
    scheduler.schedule();
    
    // All processes should complete (aging prevents starvation)
    for (const auto& p : scheduler.getProcesses()) {
        TEST_ASSERT(p->getState() == ProcessState::TERMINATED, 
                   "All processes should be terminated");
    }
    
    return true;
}

// ============================================================================
// Performance and Edge Case Tests
// ============================================================================

/**
 * @brief Test scheduler with single process
 */
bool test_single_process() {
    RoundRobinScheduler scheduler(4, 0);
    scheduler.addProcess(std::make_shared<Process>(1, "P1", 0, 10, 0));
    
    scheduler.schedule();
    
    auto metrics = scheduler.calculateMetrics();
    
    TEST_ASSERT(metrics.averageWaitingTime == 0, "Single process should have 0 waiting time");
    TEST_ASSERT(metrics.averageResponseTime == 0, "Single process should have 0 response time");
    TEST_ASSERT(metrics.averageTurnaroundTime == 10, "Turnaround time should equal burst time");
    
    return true;
}

/**
 * @brief Test scheduler with processes having same arrival time
 */
bool test_same_arrival_time() {
    RoundRobinScheduler scheduler(3, 0);
    
    for (int i = 0; i < 5; i++) {
        scheduler.addProcess(std::make_shared<Process>(i+1, "P" + std::to_string(i+1), 
                                                       0, 6, 0));
    }
    
    scheduler.schedule();
    
    // All processes should complete
    for (const auto& p : scheduler.getProcesses()) {
        TEST_ASSERT(p->getState() == ProcessState::TERMINATED, 
                   "All processes should be terminated");
    }
    
    return true;
}

/**
 * @brief Test context switch overhead
 */
bool test_context_switch_overhead() {
    RoundRobinScheduler scheduler1(2, 0);  // No overhead
    RoundRobinScheduler scheduler2(2, 1);  // 1 unit overhead
    
    scheduler1.addProcess(std::make_shared<Process>(1, "P1", 0, 4, 0));
    scheduler1.addProcess(std::make_shared<Process>(2, "P2", 0, 4, 0));
    
    scheduler2.addProcess(std::make_shared<Process>(1, "P1", 0, 4, 0));
    scheduler2.addProcess(std::make_shared<Process>(2, "P2", 0, 4, 0));
    
    scheduler1.schedule();
    scheduler2.schedule();
    
    auto metrics1 = scheduler1.calculateMetrics();
    auto metrics2 = scheduler2.calculateMetrics();
    
    // Scheduler with overhead should take longer
    TEST_ASSERT(metrics2.totalTime > metrics1.totalTime, 
               "Scheduler with context switch overhead should take longer");
    
    return true;
}

// ============================================================================
// Main Test Runner
// ============================================================================

int main() {
    std::cout << "==========================================\n";
    std::cout << "CPU SCHEDULER SIMULATOR - TEST SUITE\n";
    std::cout << "==========================================\n\n";
    
    // Process tests
    std::cout << "Process Class Tests:\n";
    std::cout << "--------------------\n";
    RUN_TEST(test_process_creation);
    RUN_TEST(test_process_execution);
    RUN_TEST(test_process_execution_overflow);
    RUN_TEST(test_process_metrics);
    RUN_TEST(test_process_reset);
    
    // Round Robin tests
    std::cout << "\nRound Robin Scheduler Tests:\n";
    std::cout << "----------------------------\n";
    RUN_TEST(test_round_robin_basic);
    RUN_TEST(test_round_robin_arrivals);
    
    // Priority Scheduler tests
    std::cout << "\nPriority Scheduler Tests:\n";
    std::cout << "-------------------------\n";
    RUN_TEST(test_priority_non_preemptive);
    RUN_TEST(test_priority_preemptive);
    RUN_TEST(test_priority_aging);
    
    // Multilevel Queue tests
    std::cout << "\nMultilevel Queue Tests:\n";
    std::cout << "----------------------\n";
    RUN_TEST(test_multilevel_queue);
    
    // MLFQ tests
    std::cout << "\nMultilevel Feedback Queue Tests:\n";
    std::cout << "--------------------------------\n";
    RUN_TEST(test_mlfq_basic);
    RUN_TEST(test_mlfq_aging);
    
    // Edge case tests
    std::cout << "\nEdge Case and Performance Tests:\n";
    std::cout << "--------------------------------\n";
    RUN_TEST(test_single_process);
    RUN_TEST(test_same_arrival_time);
    RUN_TEST(test_context_switch_overhead);
    
    // Summary
    std::cout << "\n==========================================\n";
    std::cout << "TEST SUMMARY\n";
    std::cout << "==========================================\n";
    std::cout << "Tests Passed: " << tests_passed << "\n";
    std::cout << "Tests Failed: " << tests_failed << "\n";
    std::cout << "Total Tests:  " << (tests_passed + tests_failed) << "\n";
    
    if (tests_failed == 0) {
        std::cout << "\n✓ ALL TESTS PASSED!\n";
        return 0;
    } else {
        std::cout << "\n✗ SOME TESTS FAILED\n";
        return 1;
    }
}
