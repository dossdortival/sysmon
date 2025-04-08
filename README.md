# sysmon - Interactive System Monitor

A lightweight, ncurses-based system monitor (written in C) for Linux that provides real-time information about CPU, memory, network, disk, and processes while consuming minimal system resources.

#### Video Demo:  https://www.youtube.com/watch?v=oO2lu8_9tVw

#### Description:

Modern Linux users and system administrators need a lightweight, efficient, and interactive tool to monitor real-time system performance (CPU, memory, network, disk, and process usage). Existing tools like htop and top are widely used, but they are either too complex, resource-intensive, lack customizability, or are not minimalistic enough for specific workflows.
sysmon aims to solve this by providing a lightweight, ncurses-based system monitor that is both efficient and user-friendly while consuming minimal system resources.

##### Use Cases & Acceptance Criteria
- **Use Case 1**: Viewing System Performance
   - As a system administrator,
 I want to view CPU, memory, disk, and network usage in real-time,
 so that I can monitor system health without running multiple commands.

- **✅ Acceptance Criteria:**
  - CPU, memory, disk, and network stats must update every second
  - Data should be displayed in a structured and readable format
  - UI should remain responsive without lag

- **Use Case 2**: Monitoring Running Processes
  - As a Linux user,
 I want to see a list of active processes sorted by resource usage,
 so that I can identify which processes are consuming the most CPU and memory.

- **✅ Acceptance Criteria:**
  - A table of processes should be displayed, showing PID, CPU%, MEM%, and Command
  - Users can sort by CPU or memory usage
  - The table should update dynamically without flickering

- **Use Case 3**: Killing a Process
  - As a developer,
 I want to be able to kill an unresponsive process directly from sysmon,
 so that I don’t have to manually find the PID and run kill in another terminal.

- **✅ Acceptance Criteria:**
  - Users can select a process and press k to kill it
  - A confirmation prompt should appear before killing the process
  - If successful, the process should disappear from the list; otherwise, an error message should be displayed

- **Use Case 4**: Exiting the Application
  - As a user,
 I want to easily exit sysmon, so that I can return to my normal terminal usage.

- **✅ Acceptance Criteria:**
  - Pressing q should immediately quit the application
  - UI should cleanly exit and restore the terminal to its original state

sysmon provides a minimalistic, real-time, and interactive system monitoring experience for Linux users. With efficient resource handling, intuitive keyboard navigation, and real-time data updates, it ensures a smooth and informative monitoring experience. 

## Features

### Current Features:
- **Real-time CPU Monitoring**:
  - Total and per-core CPU usage.
- **Memory and Swap Monitoring**:
  - Displays memory and swap usage with progress bars.
- **Network Activity Monitoring**:
  - Tracks download and upload rates.
  - Displays total data transferred.
- **Disk I/O Monitoring**:
  - Displays read and write rates.
  - Tracks total data read and written.
- **Process Monitoring**:
  - Lists active processes with their PID, CPU%, memory%, and name.
  - Supports scrolling to view all processes.(incoming)
- **Lightweight ncurses-based Interface**:
  - Minimal resource footprint.
  - Simple and intuitive design.

### Planned Features:
- Process management (e.g., killing processes).
- Customizable layout and colors.
- Use the arrow keys to scroll through the process list.

## Requirements

- Linux operating system
- ncurses library
- GCC or compatible C compiler
- make

## Installation From Source

```bash
git clone https://github.com/dossdortival/sysmon.git
cd sysmon
make
sudo make install
```

## Usage

- Quick Run
```bash
./bin/sysmon
```
- Press 'q' to quit the application

## Contributing

Contributions are welcome! Please feel free to submit a Pull Request.

## License

This project is licensed under the **MIT License**. See the [LICENSE](LICENSE) file for details.

## Acknowledgments

- Inspired by tools like `top` system monitor
- Designed for system administrators, developers, and Linux users who need a lightweight monitoring solution.