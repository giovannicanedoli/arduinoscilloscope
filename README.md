# Oscilloscope Using Arduino

This project allows you to use an Arduino as an oscilloscope.

## Prerequisites

Before running the script, ensure you have installed:

- **avr-gcc**

You can install these dependencies using the following command:

```bash
sudo apt-get install arduino arduino-mk
```

This will install the arduino ide and the necessary dependencies.

You'll also need to:
- **Add your group to dialup**
- **Make plot.sh executable**

You can do this with the commands:

```bash
sudo addgroup <your-username> dialout
sudo chmod +x ./serial/plot.sh
```

## Usage

To use this software, follow these steps:

1. Inside the `src` folder, run:
    ```bash
    make server.hex
    ```

2. Inside the `serial` folder, run:
    ```bash
    make
    ```

3. You have two options to execute the program:

    - Run with all the parameters provided:
        ```bash
        ./serial [channels] [mode] [frequency] [serial_port]
        ```
        - `channels`: Pin from A0 to A7
        - `mode`: Mode of operation, either **continuous** (1) or **buffered** (2)
        - `frequency`: Time required to trigger an interrupt on the timer
        - `serial_port`: Path to the serial device (optional, default: `/dev/ttyACM0`)

    - Alternatively, you can run without parameters:
        ```bash
        ./serial
        ```
        In this case, all the parameters will be asked interactively.

4. To get more information about the available parameters, you can run:
    ```bash
    ./serial --help
    ```
    This will show a list of the meaning of each parameter:

    - `channels`: A number between 1 and 8 (corresponding to pins A0 to A7)
    - `mode`: A number between 1 and 2 (1 = continuous, 2 = buffered)
    - `frequency`: A non-negative number representing the timer interrupt time
    - `device_path`: The path to the device (optional, default is `/dev/ttyACM0`)

### Help Command
The `--help` option provides more detailed usage information:

```c
void print_help() {
    printf("Usage: program [channels] [mode] [frequency] [device_path]\n");
    printf("    channels: a number between 1 and 8\n");
    printf("    mode: a number between 1 and 2 (1 = continuous, 2 = buffered)\n");
    printf("    frequency: a non-negative number representing timer interrupt time\n");
    printf("    device_path: path to the serial device (optional, default: /dev/ttyACM0)\n");
    printf("If no arguments are provided, interactive input will be requested.\n");
    printf("Options:\n");
    printf("    --help       Show this help message and exit\n");
}
```



Project for os class.
Professor Giorgio Grisetti
Year 2023-24
