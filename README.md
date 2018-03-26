# stm32-xbee-s2c-driver
Library that enables STM32F3 family of micro-controllers to interface with Digi XBee S2C radio modules. This driver was orginally
a part of my master thesis work where I created wireless sensor and actuator nodes for a process control system. For this system
I had created a set of wireless nodes that communicated with each other by using Xbee S2C radio modules. On each node there was also an STM32 micro-controller that interfaced with the Xbee radio module, as well as some auxilliary sensors/motor driver IC's. During my master thesis work there were no viable driver options that could enable this Xbee radio based wireless communication for my STM32F303K8 micro-controllers, so I decided to create a "side project" where I made one myself. This github repo is my attempt at generalizing the drivers I wrote so that they can be re-used by someone else.

## What Is The Goal Of This Project?
My goal is to create a C library which contains all of the neccesary functions which would allow any STM32 micro-controller to successfully interface with Xbee S2C radio modules (primarily, legacy support may come later). This library will make use of the STM32 HAL API as much as possible, as to avoid having to introduce any additional unneccesary data structs. During the development of these drivers I will focus on keeping the driver functions as lightweight as possible, to aid in saving power in battery driven applications.

### Development Objectives (subject to change over time)
* [] Enable Digi API mode functionality
* [] Enable Digi Transparent mode functionality
* [] Enable reconfiguration of local Xbee modules via UART uplink
* [] Enable reconfiguration of remote Xbee modules through local Xbee module via UART uplink
* [] Create a sample program showcasing a "Hello World!" message transmission
* [X] Create a terminal program which allows interaction with a local Xbee module
* [] Create useful articles in repo Wiki which explain the basics of how an Xbee network operates, how to configure it etc..

## Useful Links!
* [Xbee S2C product page](https://www.digi.com/products/xbee-rf-solutions/2-4-ghz-modules/xbee-802-15-4)
* [STM32 HAL API user manual](http://www.st.com/content/ccc/resource/technical/document/user_manual/a6/79/73/ae/6e/1c/44/14/DM00122016.pdf/files/DM00122016.pdf/jcr:content/translations/en.DM00122016.pdf)
* [Xbee S2C RF Module user manual](https://www.digi.com/resources/documentation/Digidocs/90001500/Default.htm)
