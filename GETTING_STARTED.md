# Getting Started with CPU Scheduler Simulator

## Quick Start Guide

This guide will help you get the CPU Scheduler Simulator up and running in minutes.

## Prerequisites

Before you begin, ensure you have:
- FreeBSD, Linux, or any Unix-like operating system
- G++ compiler (version 7.0 or higher) with C++17 support
- GNU Make
- Terminal access

## Installation Steps

### Step 1: Extract the Project

```bash
# If you have a zip file
unzip cpu-scheduler-simulator.zip
cd cpu-scheduler-simulator

# Or if you have the directory
cd cpu-scheduler-simulator
```

### Step 2: Verify Your Environment

```bash
# Check g++ version
g++ --version
# Should show version 7.0 or higher

# Check make
make --version
# Should show GNU Make

# List project contents
ls -la
```

You should see directories like: `src/`, `include/`, `test/`, `doc/`, and files like `Makefile`, `README.md`

### Step 3: Build the Project

```bash
# Clean any existing build (optional, safe to run)
make clean

# Build the project with optimization
make build
```

Expected output:
```
Compiling: src/Process.cpp
Compiling: src/Scheduler.cpp
Compiling: src/RoundRobinScheduler.cpp
...
Linking executable: bin/scheduler_sim
✓ Build complete: bin/scheduler_sim
```

### Step 4: Run the Simulator

```bash
# Run the program
./bin/scheduler_sim

# Or use make
make run
```

## Using the Simulator

### Main Menu

When you run the simulator, you'll see:

```
================================================================================
CPU SCHEDULER SIMULATOR
================================================================================

Select a scheduling algorithm:
1. Round Robin
2. Non-Preemptive Priority
3. Preemptive Priority
4. Multilevel Queue
5. Multilevel Feedback Queue
6. Compare All Algorithms
0. Exit

Enter your choice:
```

### Example: Running Round Robin

1. Enter `1` to select Round Robin
2. When prompted, enter a time quantum (try `3`)
3. The simulator will:
   - Create sample processes
   - Run the scheduling algorithm
   - Display detailed results
   - Show a Gantt chart

### Example: Comparing All Algorithms

1. Enter `6` to compare all algorithms
2. The simulator will:
   - Run the same process set through all scheduling algorithms
   - Display individual results for each
   - Show a comparison table

## Understanding the Output

### Individual Process Metrics

```
PID  Name      Arrival  Burst  Priority  Wait  TAT   Response
1    P1        0        10     2         10    20    0
2    P2        1        5      1         11    15    2
```

- **PID**: Process ID
- **Arrival**: When the process enters the system
- **Burst**: Total CPU time needed
- **Priority**: Process priority (lower = higher priority)
- **Wait**: Time spent waiting in ready queue
- **TAT**: Turnaround Time (completion - arrival)
- **Response**: Time to first CPU allocation

### Aggregate Metrics

```
Average Waiting Time:      10.00 time units
Average Turnaround Time:   18.00 time units
Average Response Time:     3.00 time units
CPU Utilization:           95.50 %
Throughput:                0.28 processes/time unit
```

### Gantt Chart

Shows the timeline of process execution:
```
Time |                                                            
     |PPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPP
  0      5    10    15    20    25    30    35    40    45    50
```

## Testing the Project

### Run Unit Tests

```bash
make test
```

This runs comprehensive tests of all components.

### Run with Debug Information

```bash
make debug
./bin/scheduler_sim
```

Debug builds include additional error checking and symbols for debugging.

## Common Tasks

### Clean and Rebuild

```bash
make clean
make build
```

### View All Make Targets

```bash
make help
```

### Build and Run in One Command

```bash
make run
```

## Troubleshooting

### Compilation Errors

**Problem**: "g++: command not found"
```bash
# Install g++ (Ubuntu/Debian)
sudo apt-get install build-essential

# Install g++ (FreeBSD)
sudo pkg install gcc
```

**Problem**: "C++17 features not available"
```bash
# Check g++ version
g++ --version

# Need version 7.0 or higher
# Update if necessary
```

### Runtime Issues

**Problem**: Permission denied when running
```bash
# Make the file executable
chmod +x bin/scheduler_sim
```

**Problem**: "bin/scheduler_sim: No such file or directory"
```bash
# Build first
make build
```

## Next Steps

1. **Modify Process Parameters**: Edit `src/main.cpp` in the `createTestProcesses()` function
2. **Add Custom Schedulers**: See `doc/DESIGN.md` for architecture details
3. **Run Experiments**: Try different time quanta and priority values
4. **Read Documentation**: Check `doc/` directory for detailed design and analysis

## Learning Resources

- **README.md**: Complete project overview
- **doc/DESIGN.md**: Technical design document
- **doc/USER_MANUAL.md**: Detailed usage guide
- **Comments in Code**: All source files are extensively documented

## Example Session

```bash
# Build the project
make build

# Run the simulator
./bin/scheduler_sim

# Choose option 6 (Compare All Algorithms)
> 6

# Review the comparison results

# Try Round Robin with different quanta
> 1
> Enter time quantum: 2

# Compare with larger quantum
> 1
> Enter time quantum: 8

# Exit
> 0
```

## Getting Help

If you encounter issues:

1. Check the error message carefully
2. Review the README.md file
3. Read the relevant documentation in `doc/`
4. Verify all prerequisites are installed
5. Try cleaning and rebuilding: `make clean && make build`

## Customization

### Changing Test Processes

Edit `src/main.cpp`, find the `createTestProcesses()` function:

```cpp
std::vector<std::shared_ptr<Process>> createTestProcesses() {
    std::vector<std::shared_ptr<Process>> processes;
    
    // Modify these or add more
    processes.push_back(std::make_shared<Process>(1, "P1", 0, 10, 2));
    processes.push_back(std::make_shared<Process>(2, "P2", 1, 5, 1));
    // ...
    
    return processes;
}
```

Parameters: `Process(PID, Name, ArrivalTime, BurstTime, Priority)`

### Adjusting Scheduler Parameters

In `src/main.cpp`, modify the scheduler creation:

```cpp
// Change Round Robin quantum
RoundRobinScheduler scheduler(4, 0);  // quantum=4, no context switch overhead

// Enable/disable aging in Priority Scheduler
PriorityScheduler scheduler(true, true, 5, 0);  // preemptive, aging enabled, interval=5

// Configure MLFQ queues
MultilevelFeedbackQueueScheduler scheduler(3, true, 10, 0);  // 3 queues, aging, threshold=10
```

## Success Indicators

You'll know everything is working when:
- ✓ `make build` completes without errors
- ✓ `./bin/scheduler_sim` starts and shows the menu
- ✓ Choosing an option displays results and returns to menu
- ✓ All scheduling algorithms produce output
- ✓ Gantt charts display correctly

## Congratulations!

You're now ready to explore CPU scheduling algorithms. Start with simple cases and gradually experiment with more complex scenarios.

For detailed information about each scheduling algorithm, see the main README.md file.

---

**Questions or Issues?**

- Check the `README.md` for comprehensive information
- Review `doc/DESIGN.md` for technical details
- Examine source code comments for implementation specifics
