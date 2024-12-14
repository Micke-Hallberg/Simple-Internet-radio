/*
My little internet radio project. 
Was hoping I would manage to get BT and Wifi working together on the ESP32Wroom module but it seems impossible! 
I will use an external BT Emitter just for this.
My setup is as follows:
ESP32Wroom 
PCM5102A
128x32 OLED SSD1306
KCX BT Emitter
2 push buttons
*/
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <WiFi.h>
#include <Audio.h>
#define BCLK 26 // BCK 
#define LRC  25 // LCK 
#define DOUT 27  // DIN 
/*
The following has to be done on the 5102
XMT -> 3.3V
FMT -> GND
SCL -> GND
*/
#define BUTTON_NEXT 32
#define BUTTON_SELECT 33
#define SDA 21 // SDA and SCL no need to define them really but I put it here 
#define SCL 22 // so I won't forgett where they go!
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32
#define OLED_RESET -1
WiFiClient client;
Audio audio;
Adafruit_SSD1306 oled(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
// This is my channels. Not sure they work worldwide Put your own favourites here!
char* radioStreams[] = { "https://sverigesradio.se/topsy/direkt/4951-hi-mp3.m3u",
                         "https://sverigesradio.se/topsy/direkt/164-hi-mp3.m3u",
                         "http://vis.media-ice.musicradio.com/CapitalMP3",
                         "https://sverigesradio.se/topsy/direkt/211-hi-mp3.m3u",
                         "https://sverigesradio.se/topsy/direkt/4540-hi-mp3.m3u",
                         "http://stream.piraterock.se:8101/webradio",
                         "http://www.1.fm/tunestream/crock/listen.pls",
                         "http://open.live.bbc.co.uk/mediaselector/5/select/mediaset/http-icy-mp3-a/format/pls/proto/http/vpid/bbc_world_service.pls",
                         "http://live-bauerse-fm.sharp-stream.com/retrofm_mp3",
                         "http://live-bauerse-fm.sharp-stream.com/nrj_instreamtest_se_mp3",
                         "http://media-ice.musicradio.com/Gold",
                         "https://media-ssl.musicradio.com/Heart70s",
                         "https://media-ssl.musicradio.com/Heart80s",
                         "https://media-ssl.musicradio.com/Heart90s",
                         "https://media-ssl.musicradio.com/Heart00s",
                         "http://live-bauerse-fm.sharp-stream.com/retrofm_mp3",
                         "http://stream.dbmedia.se/gkMP3"
};
77 This is the real names since some doesn't show them- 
char* realNames[] = { "SR P4 Plus",
                     "SR P3",
                     "London",
                     "Kristiansta",
                     "EKOT Direkt",
                     "PirateRock",
                     "ClassicRock",
                     "BBC Word",
                     "RetroFM",
                     "N R J",
                     "Gold",
                     "Heart 70",
                     "Heart 80",
                     "Heart 90",
                     "Heart 00",
                     "Retro FM",
                     "Guld Maloe"
};
const char* ssid = "YOUR_AP";
const char* password = "YOUR_PWD";
const int debounceTime = 250; // Change to what you find fitting
const int channelTimeout = 30000; // Change to what you find fitting
unsigned long lastButtonPress = 0;
unsigned long lastChannelChange = 0;
bool newChannelSelected = false;// Not used for the moemnt. Good to have though. :-)
int knappStatus = 0;
int currentChannel = 0;
int showListChannel;
void setupDAC(){
  Serial.println("Dac setup");
  audio.setPinout(BCLK, LRC, DOUT);
  audio.setVolume(15); // 0 - 21 I think the span is. I find 15 perfect for my speakers both wired and BT modells. Thinking of adding a volume pot later for use with earphones.
  Serial.println("Dac setup klar");
}
void setupOLED(){
  Serial.println("Setting up OLED");
  if (!oled.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Change the address to what your Oled is set. 
    Serial.println("Nope");
    for (;;) // Eternal loop if it doesn't find the oled. Not perfect but it works for now.
      ;
  }
  Serial.println("Fann OLED");
  oled.clearDisplay();
  oled.setTextSize(2); // BIG letters only one row can be shown. 
  oled.setTextColor(WHITE);
  oled.setCursor(0, 0);
  oled.println("Starting");
  oled.display();
  delay(1000);
  oled.clearDisplay();
  oled.println("Hello!");//Just checking if it is properly done
  oled.display(); 
}
void buttonCheck(){
  unsigned long currentTime = millis();
  
  if (currentTime - lastChannelChange > channelTimeout) {
    showListChannel = currentChannel;
    lastChannelChange = currentTime;
    showCurrentChannel(showListChannel);
  }
  if (digitalRead(BUTTON_NEXT) == LOW && currentTime - lastButtonPress > debounceTime) {
        showListChannel++;
        if (showListChannel >= sizeof(radioStreams) / sizeof(radioStreams[0])) {
            showListChannel = 0; // Start over again
        }
        showCurrentChannel(showListChannel);
        lastButtonPress = currentTime;
        lastChannelChange = currentTime;
        newChannelSelected = false;
    }

  if (digitalRead(BUTTON_SELECT) == LOW && currentTime - lastButtonPress > debounceTime) {
      audio.connecttohost(radioStreams[showListChannel]);
      lastButtonPress = currentTime;
      lastChannelChange = currentTime;
      newChannelSelected = true;
      currentChannel = showListChannel;
      showCurrentChannel(currentChannel);
    }  
}
void showCurrentChannel(int channel) {
    oled.clearDisplay();
    oled.setCursor(0, 0);    
    oled.print(realNames[channel]);
    oled.display();
}
void checkChannelTimeout(){
  unsigned long currentTime = millis();
  if (currentTime - lastChannelChange > channelTimeout) {
    showListChannel = currentChannel;
    showCurrentChannel(showListChannel);
  }
}
void setup(){
  pinMode(BUTTON_NEXT, INPUT_PULLUP);
  pinMode(BUTTON_SELECT, INPUT_PULLUP);
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
  setupOLED();
  setupDAC();
  
  if (audio.connecttohost("https://sverigesradio.se/topsy/direkt/4951-hi-mp3.m3u")) {
    Serial.println("Attempting to play audio...");
  } else {
    Serial.println("Failed to connect to audio stream.");
  }

  // Using a loop to make sure the connection is there.
  unsigned long startTime = millis();
  while (!audio.isRunning()) {
    // Kontrollera om vi har väntat för länge
    if (millis() - startTime > 10000) { // 10 sekunder timeout
      Serial.println("Timeout: Audio is not running.");
      break;
    }
    delay(100); // Wait a while before checking again
  }
  
  if (audio.isRunning()) {
    Serial.println("Audio is running, playing through external DAC...");
    showCurrentChannel(0);
  } else {
    Serial.println("Audio is still not running after waiting.");
    oled.clearDisplay();
    oled.setCursor(0, 0);
    oled.print("No Signal!");
    oled.display();
  }
  
}
void loop() {
  audio.loop();
  buttonCheck();
}
