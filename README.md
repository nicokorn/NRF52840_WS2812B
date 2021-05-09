# STM32F1XX_WS2812B

Example code for using Nico's ws2812b library for the nrf52840
Microcontroller. This example code works out of the box with the
IAR workbench, J-Link debugger/programmer and the Adafruit
Featherboard nrf52840 with the integrated ws2812b led.
Amount of leds and the pins on which they are connected to can be
set in the ws2812b library. The ws2812b leds are controlled by 
the pwm peripheral. Depending on the dutycycle the ws2812b leds 
interprets a 1 or 0 bit and holds a 24 bit shiftregister. The 24 
bits are splittet into red (8b), green (8) and blue (8b). 
Bits are being shiftet through all leds connected in serial. 
After finishing the shifting, the line needs
to be set low for at least 50 us, so the leds accept their
shift registers thus emitting their colours.
The library is heavily ram consuming because the pwm sequence
arrays are defined as 16bit uints and 1 ws2812b bit is used as
array entity. A future version will solve this issue.
    
<html>
<body>

<h2>The ws2812b example</h2>
<img src="https://github.com/nicokorn/NRF52840_WS2812B/blob/main/docs/WS2812B.jpg" alt="st32f1xx_ws2812b1">
<h2>WS2812B protocol timing and signal</h2>
<img src="https://github.com/nicokorn/NRF52840_WS2812B/blob/main/docs/WS2812B_Protocol_1.PNG" alt="st32f1xx_ws2812b2">

</body>
</html>
