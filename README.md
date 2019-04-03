# firefly
This code has two loops, sense and shine.  In shine, the IR LED blinks on and off really really fast.  In sense the led is off and the sensor (an IR photo transistor) is read at the rate of LED blinking.  Then there is a debouncing algorythem.. or a more apropriatly rebouncing algorythem that makes sure each sensor value alternates being higher or lower than the previouse sensor value.  When this is the case some number of times in a row, another led blinking has been detected.  When blnking is detected the counter that defines the lengeth of the loop is multiplied by 1.1 casuing the sense loop to end sooner untill the LEDs are in sync.

Things to do:
I do not know how well this will work with more than 2 fireflys
more percise timing may be accomplished with interupts
add an asyncronus loop that makes a green LED fade in and out in phase with the other loops
design final board
spec final components

Usefull links
programing the attiny10
http://www.technoblogy.com/show?1YQY
http://junkplusarduino.blogspot.com/p/attiny10-resources.html

Attiny10 Datasheet
http://ww1.microchip.com/downloads/en/DeviceDoc/atmel-8127-avr-8-bit-microcontroller-attiny4-attiny5-attiny9-attiny10_datasheet.pdf


grant proposal
https://docs.google.com/document/d/1ZPEneeNdc6gHhQVbn35usS5zam0JfSeI3uuNZ7ygSEo/edit?usp=sharing

Photo transistor schematic
https://www.digikey.com/-/media/Images/Article%20Library/TechZone%20Articles/2018/September/How%20to%20Use%20Photodiodes%20and%20Phototransistors%20Most%20Effectively/article-2018september-how-to-use-fig3.jpg?ts=77b9d89e-30c2-4136-898a-1c47026af6cd&la=en-US

project insperation
https://www.youtube.com/watch?v=ix66tQ93bdU
