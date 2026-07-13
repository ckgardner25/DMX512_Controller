# DMX512_Controller

Custom DMX512 lighting controller built around an STM32F411 BlackPill microcontroller. Completed schematic capture, two-layer PCB layout, DRC verification, and manufacturing file generation, integrating power distribution, analog input circuitry, an MP2393 buck converter, and an RS-485 transceiver for DMX512 communication.

The project combines embedded firmware development, custom PCB design, hardware validation, power electronics, and DMX512 protocol implementation, providing experience from system architecture through production-ready hardware.

## Target Features
- STM32F411 ARM Cortex-M4 microcontroller
- FreeRTOS-based firmware
- RS-485 / DMX512 communication
- Seven analog fader inputs
- Custom two-layer KiCad PCB
- 3.3 V buck power supply
- Oscilloscope-based hardware validation
- Complete GitHub documentation

## Ver 1.0
- Prototype PCBs submitted for fabrication.
- Developing FreeRTOS firmware for DMX512 communication, analog input processing, and system control.

## Ver 1.1 
- Add 12 pushbutton switches for programmed options
- Fix orientation errors of ver 1.0
- Finishing remaining FreeRTOS tasks
