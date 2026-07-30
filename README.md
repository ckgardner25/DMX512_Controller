# DMX512_Controller
## Overview
Custom DMX512 lighting controller built around an STM32F411 BlackPill microcontroller. Completed schematic capture, four-layer PCB layout, DRC verification, and manufacturing file generation, integrating power distribution, analog input circuitry, an MP2393 buck converter, and an RS-485 transceiver for DMX512 communication.

The project combines embedded firmware development, custom PCB design, hardware validation, power electronics, and DMX512 protocol implementation, providing experience from system architecture through production-ready hardware.

## Specifications
Microcontroller: STM32F411 Cortex-M4 <br>

RTOS: FreeRTOS <br>

Protocol: DMX512 over RS-485 <br>

Inputs: 7 Analog Faders <br>

ADC: 12-bit <br>

Output: 512 DMX Channels <br>

PCB: 4 Layers <br>

Power: 5 V to 3.3 V Buck Converter <br>

Validation: Oscilloscope + Functional Testing <br>

## Block Diagram
<img width="500" height="550" alt="Board" src="Diagrams+Images/Block Diagram.png" />

## Control Strategy
The DMX takes a 5V signal which drives both the RS-485 transceiver and the STM32 Microcontroller. The 5V is brought through a step-down chip that creates 3.3V to power the linear potentiometers that act as the faders which the microcontroller interprets as different signal values for lighting.

## PCB Design
<br><p float="left">
<img width="500" height="550" alt="Board" src="Renderings/Top_View.png" />
<img width="500" height="550" alt="Board" src="Renderings/Bot_View.png" />
</p>

## Measured Results


## Versions
### Ver 1.1 
- Add 12 pushbutton switches for programmed options
- Fix orientation errors of ver 1.0
- Changed to four-layer PCB
- Finished FreeRTOS tasks
<img width="400" height="450" alt="Board" src="Fabricated_board/Unpopulated_Board.jpeg" />

### Ver 1.0
- Prototype PCBs submitted for fabrication.
- Developing FreeRTOS firmware for DMX512 communication, analog input processing, and system control.

