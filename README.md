# grbl-MITM-pendant
An arduino sketch that allows you to use an STM32F1 Board (BluePill) as an GRBL jog controller for any GRBL v1.1 machine while also forwarding any recieved serial commands from a PC. 
In this implementation three rotary encoder are used to control steps in X, Y and Z direction while an analog selector with 6 positions sets the step size (0.01mm to 5mm per encoder tick).

Topology:
PC <----> STM32F1 jog controller <----> Arduino Nano running GRBL v1.1

Improvments in this fork: 
- Using proper jog commands for grbl controllers (i.e. $J=G91 F1000 X5). Jogging commands are only accepted when the machine is in idle. So you do not have to worry about disturbing a running job. Also it does not change any machine modes (absolut/relativ, mm/inch, etc).
- Ported to STM32F1 (BluePill) for better communication, better performance and more peripherals.
- Baudrate of 115200 works by using a second hardware serial port of the STM32F1. A software serial on an Arduino Nano is not fast enough for this baudrate.
- Hardware timers are used to count the encoder steps. Basically no software overhead. Using quadratureBluePill.h library
  from Chris Albertson.
- Tested with Candle GRBL Controller and works well.
