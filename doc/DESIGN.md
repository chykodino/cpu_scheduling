# CPU Scheduler Simulator - Design Document

## 1. Executive Summary

This document describes the architecture, design patterns, and implementation details of the CPU Scheduler Simulator project. The system demonstrates various CPU scheduling algorithms used in modern operating systems through simulation and performance analysis.

## 2. System Architecture

### 2.1 High-Level Architecture

The system follows a hierarchical object-oriented design:

```
┌─────────────────────────────────────────────┐
│          Main Application Layer             │
│         (User Interface & Control)          │
└──────────────┬──────────────────────────────┘
               │
┌──────────────┴──────────────────────────────┐
│         Scheduler Abstract Layer            │
│      (Common Scheduling Interface)          │
└──────────────┬──────────────────────────────┘
               │
       ┌───────┴────────┬──────────────┬──────────────┐
       │                │              │              │
┌──────┴───────┐ ┌─────┴──────┐ ┌────┴─────┐ ┌──────┴────────┐
│ Round Robin  │ │ Priority   │ │   MLQ    │ │     MLFQ      │
│  Scheduler   │ │ Scheduler  │ │Scheduler │ │  Scheduler    │
└──────┬───────┘ └─────┬──────┘ └────┬─────┘ └──────┬────────┘
       │               │              │              │
       └───────────────┴──────────────┴──────────────┘
                       │
            ┌──────────┴──────────┐
            │   Process Manager   │
            │  (Process Lifecycle)│
            └─────────────────────┘
```

### 2.2 Component Diagram

```
┌─────────────┐
│   Process   │ ◄─────────────────┐
└─────────────┘                   │
       │ manages                  │ contains
       │                          │
┌──────┴──────────┐      ┌────────┴────────┐
│   Scheduler     │◄─────│  Main Program   │
│  (Abstract)     │      └─────────────────┘
└──────┬──────────┘
       │ inherits
       │
   ┌───┴────┬────────┬──────────┬────────┐
   │        │        │          │        │
┌──┴──┐ ┌──┴──┐ ┌───┴───┐ ┌────┴────┐ ┌─┴──┐
│ RR  │ │ PP  │ │ MLQ   │ │ MLFQ    │ │... │
└─────┘ └─────┘ └───────┘ └─────────┘ └────┘
```

## 3. Class Design

### 3.1 Process Class

**Responsibility**: Represents a single process with its attributes and state.

**Key Attributes**:
- `pid`: Unique process identifier
- `arrivalTime`: When process enters system
- `burstTime`: Total CPU time required
- `remainingTime`: CPU time left to execute
- `priority`: Process priority level
- `state`: Current process state (enum)

**Key Methods**:
- `execute(quantum)`: Execute for given time
- `calculateMetrics()`: Compute performance metrics
- `reset()`: Reset to initial state

**Design Pattern**: Value Object with mutable state

### 3.2 Scheduler Base Class

**Responsibility**: Defines common interface and shared functionality for all schedulers.

**Key Attributes**:
- `processes`: Collection of all processes
- `currentTime`: Current simulation time
- `contextSwitchOverhead`: Time cost per context switch

**Key Methods**:
- `schedule()`: Pure virtual - algorithm implementation
- `calculateMetrics()`: Compute aggregate statistics
- `displayResults()`: Output formatted results
- `admitArrivingProcesses()`: Handle new arrivals

**Design Pattern**: Template Method + Strategy

### 3.3 Concrete Scheduler Classes

Each algorithm-specific scheduler inherits from base class and implements:
- Custom scheduling logic in `schedule()`
- Algorithm-specific data structures
- Gantt chart generation

## 4. Data Structures

### 4.1 Process Storage
- **Type**: `std::vector<std::shared_ptr<Process>>`
- **Rationale**: Dynamic sizing, shared ownership, cache-friendly
- **Complexity**: O(1) access, O(n) iteration

### 4.2 Ready Queue (Round Robin)
- **Type**: `std::queue<std::shared_ptr<Process>>`
- **Rationale**: FIFO semantics match RR algorithm
- **Complexity**: O(1) enqueue/dequeue

### 4.3 Priority Queue (Priority Scheduling)
- **Type**: `std::priority_queue` with custom comparator
- **Rationale**: Efficient priority-based selection
- **Complexity**: O(log n) insert, O(log n) extract

### 4.4 Multiple Queues (MLQ, MLFQ)
- **Type**: `std::map<int, std::queue<...>>` or `std::vector<std::queue<...>>`
- **Rationale**: Organized by priority level
- **Complexity**: O(1) queue selection, O(1) per-queue operations

## 5. Algorithm Implementations

### 5.1 Round Robin

**Pseudocode**:
```
while not all processes complete:
    admit arriving processes to ready queue
    if ready queue empty:
        advance time to next arrival
        continue
    
    process = dequeue from ready queue
    context switch to process
    execute for min(quantum, remaining time)
    
    if process complete:
        mark as terminated
        calculate metrics
    else:
        enqueue back to ready queue
```

**Time Complexity**: O(n × burst_time / quantum)
**Space Complexity**: O(n) for ready queue

### 5.2 Priority Scheduling

**Preemptive Mode**:
```
while not all processes complete:
    admit arriving processes
    select highest priority ready process
    
    if current process has lower priority:
        preempt current process
    
    execute selected process for 1 time unit
    
    if process complete:
        mark as terminated
```

**Non-Preemptive Mode**:
```
while not all processes complete:
    admit arriving processes
    select highest priority ready process
    execute to completion
    mark as terminated
```

**Time Complexity**: O(n log n) with priority queue
**Space Complexity**: O(n)

### 5.3 Multilevel Queue

**Implementation**:
```
configure multiple queues with policies
assign processes to queues by priority

while not all processes complete:
    find highest priority non-empty queue
    if queue uses RR:
        execute process for quantum
        re-enqueue if not complete
    else if queue uses FCFS:
        execute process to completion
```

**Time Complexity**: O(n × m) where m is number of queues
**Space Complexity**: O(n)

### 5.4 Multilevel Feedback Queue

**Implementation**:
```
all processes start in highest priority queue

while not all processes complete:
    select from highest priority non-empty queue
    execute for queue's quantum
    
    if process used full quantum:
        demote to next lower queue
    
    if aging enabled and threshold reached:
        promote waiting processes
```

**Time Complexity**: O(n × m × log n)
**Space Complexity**: O(n × m)

## 6. Performance Metrics

### 6.1 Calculated Metrics

**Waiting Time**:
```
WT = Turnaround Time - Burst Time
   = Time spent in ready queue
```

**Turnaround Time**:
```
TAT = Completion Time - Arrival Time
    = Total time in system
```

**Response Time**:
```
RT = First CPU Allocation Time - Arrival Time
   = Initial wait time
```

**CPU Utilization**:
```
CPU% = (Total Burst Time / Total Simulation Time) × 100
```

**Throughput**:
```
Throughput = Completed Processes / Total Time
```

### 6.2 Metric Tracking

Metrics are tracked in two ways:
1. **Per-Process**: Incremental updates during simulation
2. **Aggregate**: Calculated from completed processes

## 7. Context Switching

### 7.1 Simulation Model

Context switches are modeled with configurable overhead:

```cpp
void contextSwitch(Process* from, Process* to) {
    if (from != to && from != nullptr && to != nullptr) {
        contextSwitchCount++;
        currentTime += contextSwitchOverhead;
    }
    updateProcessStates(from, to);
}
```

### 7.2 Impact Analysis

Context switch overhead affects:
- Total execution time
- CPU utilization
- Turnaround time
- Throughput

Typical values: 0-1 time units per switch

## 8. State Machine

### 8.1 Process State Transitions

```
     NEW
      │
      ├──► READY ◄──┐
      │      │      │
      │      ▼      │
      └──► RUNNING ─┘
             │
             ▼
        TERMINATED
```

**Valid Transitions**:
1. NEW → READY (admission)
2. READY → RUNNING (dispatch)
3. RUNNING → READY (preemption/quantum expiry)
4. RUNNING → TERMINATED (completion)

## 9. Memory Management

### 9.1 Smart Pointers

Use `std::shared_ptr<Process>` for:
- Automatic memory management
- Shared ownership across schedulers
- Prevention of memory leaks
- Exception safety

### 9.2 Container Strategy

```cpp
// Processes owned by scheduler
std::vector<std::shared_ptr<Process>> processes;

// Ready queue holds references
std::queue<std::shared_ptr<Process>> readyQueue;
```

## 10. Error Handling

### 10.1 Input Validation

- Validate process parameters (positive burst times, etc.)
- Check scheduler configuration
- Verify queue configurations

### 10.2 Defensive Programming

```cpp
// Example: Safe process execution
int execute(int quantum) {
    int actual = std::min(quantum, remainingTime);
    remainingTime -= actual;
    return actual;
}
```

## 11. Testing Strategy

### 11.1 Unit Tests

- Process class methods
- Individual scheduler algorithms
- Metric calculations
- State transitions

### 11.2 Integration Tests

- Complete scheduling cycles
- Multi-process scenarios
- Edge cases (empty queues, single process)

### 11.3 System Tests

- Performance benchmarks
- Stress tests (many processes)
- Comparison tests (all algorithms)

## 12. Build System

### 12.1 Makefile Structure

```makefile
# Separation of concerns
SOURCES = $(wildcard src/*.cpp)
OBJECTS = $(SOURCES:src/%.cpp=build/%.o)

# Multiple targets
build: optimized compilation
debug: debug symbols included
test: compile and run tests
clean: remove artifacts
```

### 12.2 Compilation Flags

- **Optimization**: `-O2` for release builds
- **Warnings**: `-Wall -Wextra -Wpedantic`
- **Standard**: `-std=c++17`
- **Debug**: `-g -O0` for debug builds

## 13. Extensibility

### 13.1 Adding New Algorithms

1. Create new class inheriting from `Scheduler`
2. Implement `schedule()` method
3. Override `getName()` and `getGanttChart()`
4. Add to main menu

### 13.2 Adding New Metrics

1. Add metric field to `SchedulingMetrics` struct
2. Update `calculateMetrics()` in base class
3. Update `displayResults()` output

## 14. Performance Optimizations

### 14.1 Algorithmic Optimizations

- Priority queue for O(log n) priority selection
- Early termination when all processes complete
- Efficient state checking

### 14.2 Implementation Optimizations

- Reserve vector capacity when size known
- Minimize string allocations
- Cache frequently accessed values

## 15. Documentation Standards

### 15.1 Code Documentation

- Doxygen-style comments for all public interfaces
- Inline comments for complex logic
- Example usage in documentation

### 15.2 File Headers

Every file includes:
- @file tag with filename
- @brief description
- Detailed explanation if needed

## 16. Conclusion

This design provides a robust, extensible framework for CPU scheduling simulation. Key strengths:

- **Modularity**: Clear separation of concerns
- **Extensibility**: Easy to add new algorithms
- **Performance**: Efficient data structures and algorithms
- **Maintainability**: Well-documented and tested
- **Educational Value**: Clear demonstration of OS concepts

The architecture supports the project requirements while maintaining code quality and professional standards.

---

**Document Version**: 1.0  
**Last Updated**: December 2025  
**Authors**: [Team Members]
