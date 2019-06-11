#include <Arduino.h>
#include <string.h>
#include <ESP8266WiFi.h>

#define led 16

// Motors
#define forward 5
#define backward 4
#define left 0
#define right 10
#define length 50

// HC-SR04
#define trig 14
#define echo 12

long duration;
double distance;

// Network
#define port 5000

const char* ssid = "linksys";
const char* password = "Vasolka7";

WiFiServer server(port);
WiFiClient client;

String message = "";

void turnMotor(int pin, int duration) {
  digitalWrite(pin, HIGH);
  delay(duration);
  digitalWrite(pin, LOW);
}

void turnMotor(int pin1, int pin2, int duration) {
  digitalWrite(pin1, HIGH);
  digitalWrite(pin2, HIGH);
  delay(duration);
  digitalWrite(pin1, LOW);
  digitalWrite(pin2, LOW);
}

void setup() {
  Serial.begin(115200);
  delay(500);

  pinMode(led, OUTPUT);
  pinMode(forward, OUTPUT);
  pinMode(backward, OUTPUT);
  pinMode(left, OUTPUT);
  pinMode(right, OUTPUT);
  
  pinMode(trig, OUTPUT);
  pinMode(echo, INPUT);

  // Off by default
  digitalWrite(led, HIGH);
  digitalWrite(forward, LOW);
  digitalWrite(backward, LOW);
  digitalWrite(left, LOW);
  digitalWrite(right, LOW);

  Serial.print("SSID: ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.print("WiFi connected. IP: ");
  Serial.println(WiFi.localIP());

  server.begin();
}

void loop() {
  if(client) {
    // The built-in LEDs for some reason turn on when set to LOW
    digitalWrite(led, LOW);

    while(client.available() > 0) {
      char c = client.read();

      message += c;
    }

    if(message != "") {
      Serial.print(message);

      int direction = message.toInt();

      // Motor control
      if(direction != 0) {
        switch(direction) {
          // Forward
          case 1:
            turnMotor(forward, length);
            break;
          // Backward
          case -1:
            turnMotor(backward, length);
            break;
          // Right
          case 3:
            turnMotor(right, length);
            break;
          // Left
          case -3:
            turnMotor(left, length);
            break;
          // FL
          case -2:
            turnMotor(forward, left, length);
            break;
          // FR
          case 4:
            turnMotor(forward, right, length);
            break;
          // BL
          case -4:
            turnMotor(backward, left, length);
            break;
          // BR
          case 2:
            turnMotor(backward, right, length);
            break;
        }
      }

      // Collect and send data from the sensors
      digitalWrite(trig, LOW);
      delayMicroseconds(5);

      digitalWrite(trig, HIGH);
      delayMicroseconds(10);
      digitalWrite(trig, LOW);

      duration = pulseIn(echo, HIGH);
      distance = duration * 0.034 / 2;

      String d = String(distance, 2);

      Serial.print(" -> ");
      Serial.println(d);

      client.write(d.c_str());

      message = "";
    }
  }

  else {
    // Remember it's inverted
    digitalWrite(led, HIGH);

    client = server.available();
  }
}