Current Revision: 2020-11-07 (v3.1)

## Executive Summary / Concept of Operations

All-in-One-Die is an extra-large die that you can shake with your own two hands! Large enough to be bigger than your average d6 or d20, and it ‘rolls’ when you shake it. When a 1 is rolled, an extremely negative sound* will be played. Perfect for those who enjoy tabletop gaming and want a fun interactive novelty item. 

\*Not family friendly, may involve swearing

## Brief Market Analysis

The need for varying value dice is frequently felt by the tabletop gaming community. Games such as _Dungeons and Dragons_ and _Magic the Gathering_ require the use of dice for tracking the number of creatures a player can create, damage caused, and other game effects. In an average game of this type, a player can expect to have at least one set of gaming dice. A typical set will have 1 of each of the following: d4, d6, d8, d10, d12, d20, with each value ‘n’ after ‘d’ representing the possible rolls from 1 to n. Instead of having to carry around a bunch of dice, the All-in-One-Die will provide the customer with a handheld device capable of rolling each type of die.

Looking at what is available in the market for tabletop gaming, no handheld devices that match this design's functionality currently exist. There are some simple boards capable of displaying a roll on LEDs but those don’t have a practical application for gaming. The biggest competition comes from dice rolling apps and websites. While these offer a quick solution to being able to roll dice of different values, it lacks the hands on feel many gamers enjoy. Arguably, there are digital versions of games, but the type of customers this device is for want something they can physically play with. A big selling point of the device is the novelty of it and having a tangible accessory to improve the gaming experience.

Going over early projections of the cost of production, it would cost about $232.00 to manufacture 100 boards, equating to about $2.00 a board, added components and batteries brings the total up to about $20.00 per board. Using this production cost, and considering the limited batch being made, the device would be sold for about $40.00-$50.00. This cost could be brought down if it were being mass produced.


## Requirements
This project MUST:
* Have some input to change the size of the die
* Be fully enclosed in a case with the exception of inputs
* Have an on/off switch
* Have an audio device to play a sound effect upon rolling a 1
* Have a way to detect shaking of the die
* Have a screen that displays the result of the roll.
* Be portable and battery-powered with replaceable batteries
* Battery power lasts at least 1 hour
* Contain a processor
* Be live documented on Github Wiki

This project SHOULD:
* Have rechargeable batteries 
* Be created on a PCB that is as small as possible
* Contain mostly hand solder surface mount components on the board
* Have documentation and issues tracked on Github


## System Architecture
### High-Level Diagram

![High level diagram, because block diagrams are important](https://media.discordapp.net/attachments/758774692181442581/768668638180802580/unknown.png)

### Low-level diagram

![Low level diagram, just y'know ignore the low resolution. ](https://user-images.githubusercontent.com/5107773/96808173-2c74af00-13cd-11eb-9bce-c6de82beec40.png)

## Design Specification
* Processor: ATMega328P
* Actuator: LCD Screen and Speaker
* Sensor: Accelerometer (LIS3L02AQ3)
* USB to Serial: FT232RL
* Power: 3.3V-5V
* Using Arduino IDE
