# sysmon - Interactive System Monitor

A lightweight, ncurses-based system monitor (written in C) for Linux that provides real-time information about CPU, memory, network, disk, and processes while consuming minimal system resources.
 
Modern Linux users and system administrators need a lightweight, efficient, and interactive tool to monitor real-time system performance (CPU, memory, network, disk, and process usage). Existing tools like htop and top are widely used, but they are either too complex, resource-intensive, lack customizability, or are not minimalistic enough for specific workflows.
sysmon aims to solve this by providing a lightweight, ncurses-based system monitor that is both efficient and user-friendly while consuming minimal system resources.

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