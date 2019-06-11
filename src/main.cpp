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

void turnMotor(int direction) {
  switch(direction) {
    // F
    case 1:
      digitalWrite(forward, HIGH);
      digitalWrite(backward, LOW);
      digitalWrite(left, LOW);
      digitalWrite(right, LOW);
      break;
    // B
    case -1:
      digitalWrite(forward, LOW);
      digitalWrite(backward, HIGH);
      digitalWrite(left, LOW);
      digitalWrite(right, LOW);
      break;
    // L
    case -3:
      digitalWrite(forward, LOW);
      digitalWrite(backward, LOW);
      digitalWrite(left, HIGH);
      digitalWrite(right, LOW);
      break;
    // R
    case 3:
      digitalWrite(forward, LOW);
      digitalWrite(backward, LOW);
      digitalWrite(left, LOW);
      digitalWrite(right, HIGH);
      break;
    // FL
    case -2:
      digitalWrite(forward, HIGH);
      digitalWrite(backward, LOW);
      digitalWrite(left, HIGH);
      digitalWrite(right, LOW);
      break;
    // FR
    case 4:
      digitalWrite(forward, HIGH);
      digitalWrite(backward, LOW);
      digitalWrite(left, LOW);
      digitalWrite(right, HIGH);
      break;
    // BL
    case -4:
      digitalWrite(forward, LOW);
      digitalWrite(backward, HIGH);
      digitalWrite(left, HIGH);
      digitalWrite(right, LOW);
      break;
    // BR
    case 2:
      digitalWrite(forward, LOW);
      digitalWrite(backward, HIGH);
      digitalWrite(left, LOW);
      digitalWrite(right, HIGH);
      break;
    default:
      digitalWrite(forward, LOW);
      digitalWrite(backward, LOW);
      digitalWrite(left, LOW);
      digitalWrite(right, LOW);
  }
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
      turnMotor(direction);

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

    // If client disconnects we no longer want the motors to run
    turnMotor(0);

    client = server.available();
  }
}