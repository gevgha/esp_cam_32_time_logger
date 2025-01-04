#include "SD_MMC.h"
#include "SPI.h"

#include <WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

#define BELL_PIN_1 0
#define SMALL_LED_PIN 33
// Replace with your network credentials
//const char* ssid     = "BooTech";
//const char* password = "BooW!f!~2o2%";
const char* ssid     = "Galaxy S23 Ultra";
const char* password = "12345678";
//const char* ssid     = "GEVORG-HOME";
//const char* password = "77909157";
// Define NTP Client to get time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);

// Variables to save date and time
String formattedDate;
String dayStamp;
String timeStamp;


void setup() {

  Serial.begin(115200);
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    digitalWrite(SMALL_LED_PIN, HIGH);
    delay(500);
    digitalWrite(SMALL_LED_PIN, LOW);
  }
  digitalWrite(SMALL_LED_PIN, LOW);
  // Print local IP address and start web server
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  timeClient.begin();
  timeClient.setTimeOffset(14400);

  delay(2000);  // Wait for SD card to initialize
  while (true) {
    if (!SD_MMC.begin()) {
      Serial.println("Failed to mount card");
    } else {
      break;
    }
    delay(2000);
  }

  pinMode(BELL_PIN_1, INPUT_PULLUP);
  pinMode(SMALL_LED_PIN, OUTPUT);
  writeTxt("Start Arduino ( " + getCurrentTime() + " )");
  Serial.println("Setup");
}

void loop() {
  int bellPin1State = digitalRead(BELL_PIN_1);
  if (bellPin1State == LOW) {
    Serial.println("BELL 1 ring");
    writeTxt(getCurrentTime());
    delay(12000);

  }
  delay(999);
  Serial.println("Loop");
}

void writeTxt(String txt) {
  File file = SD_MMC.open("/bell_1.txt", FILE_APPEND);

  if (!file) {
    Serial.println("Opening file failed");
    delay(2000);  // Wait to avoid flooding the serial monitor
    return;
  }
  file.println(txt);
  file.close();
  delay(2000);
}

String getCurrentTime() {
  while (!timeClient.update()) {
    timeClient.forceUpdate();
  }
  formattedDate = timeClient.getFormattedDate();
  Serial.println(formattedDate);
  int splitT = formattedDate.indexOf("T");
  dayStamp = formattedDate.substring(0, splitT);
  Serial.print("DATE: ");
  Serial.println(dayStamp);
  timeStamp = formattedDate.substring(splitT + 1, formattedDate.length() - 3);
  Serial.print("FULL TIME: ");
  Serial.println(timeStamp);

  // Extract hours and minutes
  int splitColon = timeStamp.indexOf(":");
  String hours = timeStamp.substring(0, splitColon);
  String minutes = timeStamp.substring(splitColon + 1, splitColon + 3);
  String formattedTime = hours + ":" + minutes;

  Serial.print("TIME (HH:MM): ");
  Serial.println(formattedTime);
  return dayStamp + " | " + formattedTime;
}
