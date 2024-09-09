# sketch_joy.ino

### Arduino UNO R4 WIFI project implementing Pong/Snake and FlappyBird controlled by Analog Joystick

ref: [https://arduinogetstarted.com/tutorials/arduino-joystick](https://arduinogetstarted.com/tutorials/arduino-joystick)

#### pinouts

For Pinouts to the Joystick see:

    VRX_PIN  A0 // Arduino pin connected to VRX pin
    VRY_PIN  A1 // Arduino pin connected to VRY pin
    SW_PIN   D2  // Arduino pin connected to SW  pin

![Wiring to Joystick](Img/wiring.jpg?raw=true)

#### required library

![ezButton](Img/library.jpg?raw=true)

### Instructions

While the LED matrix main menu displays P/S or FB in rotation, press the Joystick button to play the corresponding game.

### Game instructions:
   
    snake: up/down/left/right
   
    pong: up/down
   
    flappy bird: button press (shorter than 5 secs or you will exit back to menu)

scores in serial output.

While in game mode, to exit, long press the Joystick button.  Pressing for more than 5secs will bring back the menu.