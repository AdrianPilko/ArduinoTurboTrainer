# ArduinoTurboTrainer
Project to create an arduino controlled turbo trainer upgrade

 control for turbo trainer
     - support for 4 relays, I have opto-isolated block of 4 on same pcb, with seperate relay power supply
     - TM1638 board with 7segment display and buttons increase/decrease number of relays on, this will switch in and out the power resistor loads on which are connected to the turbo trainer generator circuit
     - output for LEDs on TM1638 board to show resistance load setting
    
     future upgrade:
         - measure and display the power being generated ( power = current * voltage) but need way of saefly measuring from arduino (see https://www.instructables.com/ARDUINO-ENERGY-METER/)
         - way to load excercise programs, or even a few presets, selectabel via another button
         - connection to ANT+ to integrate to zwift if possible
         - rear wheel rpm to data log equivallent distance travelled, and view on a display in real time
         - data logging for power and distance, produce graphs, maybe best to use a serial connection to either phone or raspberry pi
