# Relay-able transmission

Transmitting files via UDP with STM32F769 (Nucleo 144) acting as a relay

## Build

PC-side build is done via cmake, just create build directory and execute:

```bash
    cd build/ && cmake .. && make
```

STM-side build is handled by ST CubeIDE [WIP]

## Usage

### Setting up receive node

```bash
    ./RelayableTransmitter -l [port]
```

After this command program will block untill it detects transmission or will be terminated by user

### Sending file to listening node

```bash
    ./RelayableTransmitter -p [port] -f [file to transfer] -i [receipient(STM) IP]
```
