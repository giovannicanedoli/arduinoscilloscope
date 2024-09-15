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

Project for os class.
Professor Giorgio Grisetti
Year 2023-24