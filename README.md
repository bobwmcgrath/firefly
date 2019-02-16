# firefly
This code has two loops, sense and shine.  In shine, the LED blinks on and off really really fast.  In sense the led is of and the sensor is read at twice the rate of LED blinking.  Then there is a debouncing algorythem.. or a more apropriatly rebouncing algorythem that makes sure each sensor value alternates being higher or lower than the previouse sensor value.  When this is the case some number of times in a row another led blinking has been detected.  When blnking is detected the counter that defines the lenget of the loop is multiplied by 1.1 casuin the shine loop to start sooner untill the LEDs are in sync.

requires the sensor library from adafruit

Things to fix:
I do not know how well this will work with more than 2 fireflys
The code is messy and uncommented
more percise timing may be accomplished with interupts