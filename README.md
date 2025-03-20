# sysmon - Interactive System Monitor

A lightweight, ncurses-based system monitor (written in C) for Linux that provides real-time information about CPU, memory, network, and processes while consuming minimal system resources.

## Features

Phase 1 (Current):
- Real-time CPU usage monitoring (total and per-core)
- Memory and swap usage monitoring
- Simple, lightweight ncurses-based interface
- Minimal resource footprint

Planned Features:
- Process listing and management
- Network activity monitoring
- Disk I/O statistics
- Customizable layout and colors

## Requirements

- Linux operating system
- ncurses library
- GCC or compatible C compiler
- make

## Building and Running

### Build
```bash
make
```

### Run
```bash
make run
```
or
```bash
bin/sysmon
```

### Install
```bash
sudo make install
```

## Usage

- Press 'q' to quit the application

## Contributing

Contributions are welcome! Please feel free to submit a Pull Request.

## License

"This project is licensed under the MIT License - see the LICENSE file for details."

## Acknowledgments

- Inspired by tools like htop, top system monitors
- Designed for users who need a lightweight monitoring solution