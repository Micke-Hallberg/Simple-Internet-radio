# Simple-Internet-radio
A simple Internet radio using ESP32Wroom
For this we use the following:<br>
ESP32 Wrooom<br>
PCM5102 DAC <br>
128x32 SSD1306 Oled <br>
KCX BT Emitter<br>
2 push buttons<br>

It is just my test if I could get it to work and I am satisfied with it.<br>
There are more advanced on the net to find but I wanted it my way. 
Connecting the part together isn't that difficult, rather straight forward I might say.<br>
Enough with this! Let start connecting!<br>
<br>
Lets start with the PCM5102, it is a little disturbing that I couldn't find proper instruction for my module.<br>
At last I managed to figure it out using several sites to get it together.<br>
Those are important, without the connections you will only hear static noioce.<br>

XMT -> 3.3V<br>
FMT -> GND<br>
SCL -> GND<br>
<br>
BCK, LCK and DIN (Those are the names on my module) BCK = BCLK, LCK = LRC, DIN = DOUT on some models. <br>
In my code I have those three connected as follows:<br>
<b>PCM &emsp;ESP</b><br>
BCLK -> GPIO26<br>
LRC -> GPIO25<br>
DOUT -> GPIO27<br>
Then we have three more! But we wait a little while with them since they are to be connected to the BT Emitter.<br>

Now the Oled.
Vcc - 5V (Some models can't take that! Then use 3.3V!) Check your module!! If you get it wrong you can damage the module!<br>
GND -> GND (Naturally)<br>
<b>Oled &emsp;ESP</b><br>
SDA -> 21<br>
SCL -> 22<br>
Those two GPIO is the standard for most ESP32 and the Wire library.<br>
<br>
<br>
Will fill in more later!


