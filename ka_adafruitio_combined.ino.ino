// Adafruit IO Publish Example
//
// Adafruit invests time and resources providing this open source code.
// Please support Adafruit and open source hardware by purchasing
// products from Adafruit!
//
// Written by Todd Treece for Adafruit Industries
// Copyright (c) 2016 Adafruit Industries
// Licensed under the MIT license.
//
// All text above must be included in any redistribution.

/************************** Configuration ***********************************/

// edit the config.h tab and enter your Adafruit IO credentials
// and any additional configuration needed for WiFi, cellular,
// or ethernet clients.
#include "config.h"
#include <dht.h>
#include <MQ2.h>
#include <FastLED.h>
#define DHT11_PIN 2   //D4
int mq2pin= A0;
int i;

dht DHT;
MQ2 mq2(mq2pin);
CRGB leds[6];

float temp;
float humid;
float smoke;
float lpg;
float co;
float prevsmoke;
float prevlpg;
float prevco;
float prevtemp;
float prevhumid;
long currMillis;
long prevMillis;
long prevMillisMQ2;
int ir;
int previr;
int counter;
// set up the feed
AdafruitIO_Feed *c1 = io.feed("humidity");
AdafruitIO_Feed *c2 = io.feed("temperature");
AdafruitIO_Feed *c3 = io.feed("relay 1");
AdafruitIO_Feed *c4 = io.feed("mq2_lpg");
AdafruitIO_Feed *c5 = io.feed("mq2_smoke");
AdafruitIO_Feed *c6 = io.feed("mq2_CO");
AdafruitIO_Feed *c7 = io.feed("lights");
AdafruitIO_Feed *c8 = io.feed("entry_IR");

void setup() {

  // start the serial connection
  Serial.begin(115200);
  prevMillis=long(0);
  prevMillisMQ2=long(0);
  prevtemp=0;
  prevhumid=0;
  prevsmoke=0;
  prevlpg=0;
  prevco=0;
  previr=0;
  mq2.begin();
  delay(3000);
  // wait for serial monitor to open
  while(! Serial);

  Serial.print("Connecting to Adafruit IO");

  // connect to io.adafruit.com
  io.connect();
  c3->onMessage(handle_r1_Message);
  c5->onMessage(handle_r2_Message);
  c7->onMessage(handle_lights_Message);
  c8->onMessage(handle_entry_IR_Message);

  // wait for a connection
  while(io.mqttStatus() < AIO_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  c3->get();
  c5->get();
  c7->get();
  c8->get();
  pinMode(14,OUTPUT);
  pinMode(12,OUTPUT);
  pinMode(13,INPUT);
  FastLED.addLeds<WS2812B, 8, GRB>(leds, 6);
  FastLED.setBrightness(100);

  // we are connected
  Serial.println();
  Serial.println(io.statusText());

}

void loop() {

  // io.run(); is required for all sketches.
  // it should always be present at the top of your loop
  // function. it keeps the client connected to
  // io.adafruit.com, and processes any incoming data.
  io.run();

  //DHT11
/*  currMillis = (long)(millis());
  if (currMillis - prevMillis >= 3000) {
  int chk = DHT.read11(DHT11_PIN);
  temp = DHT.temperature;
  humid = DHT.humidity;
  if (prevtemp != temp) {
    c2->save(temp);
    Serial.print(temp);
    prevtemp=temp;
  }
  if (prevhumid != humid) {
    c1->save(humid);
    Serial.print("   ");
    Serial.println(humid);
    prevhumid=humid;
  }
  prevMillis = currMillis;
  }*/
  // Adafruit IO is rate limited for publishing, so a delay is required


//MQ2
  currMillis = (long)(millis());
  if (currMillis - prevMillisMQ2 >= 10000) {
  float* values= mq2.read(true);
  smoke= mq2.readSmoke();
  lpg= mq2.readLPG();
  co= mq2.readCO();
  if (prevsmoke != smoke) {
    c5->save(smoke);
    //Serial.print(smoke);
    prevsmoke=smoke;
  }
  if (prevlpg != lpg) {
    c4->save(lpg);
    //Serial.print("   ");
    //Serial.println(lpg);
    prevlpg=lpg;
  }
  if (prevco != co) {
    c6->save(co);
    //Serial.print("   ");
    //Serial.println(co);
    prevco=co;
  }
  prevMillisMQ2=currMillis;
  // Adafruit IO is rate limited for publishing, so a delay is required
}

//IR***************
  /*ir= digitalRead(13);
  counter=0;
  if (previr != ir) {
    counter= counter + 1;
    c8->save(counter);
    Serial.print(counter);
    previr=ir;
  } */
}

//Functions of Output Devices
void handle_r1_Message(AdafruitIO_Data *data) {

  int reading= data->toInt();
  Serial.print("received <- ");
  Serial.println(reading);
  if (reading == 0){
    digitalWrite(14, HIGH);
  }
  else if (reading == 1){
    digitalWrite(14,LOW);
  }
}

void handle_r2_Message(AdafruitIO_Data *data) {

  int smoke_rec= data->toInt();
  Serial.print("received <- ");
  Serial.println(smoke_rec);
  if (smoke_rec<=30){
    digitalWrite(12,HIGH);
  }
  else{
    digitalWrite(12,LOW);
  }
}

void handle_lights_Message(AdafruitIO_Data *data) {

  int reading2= data->toInt();
  Serial.print("received <- ");
  Serial.println(reading2);
  if (reading2 == 0){
    for (i=0; i<=5; i++){
      leds[i]= CRGB::White;
      FastLED.setBrightness(255);
      FastLED.show();
    }
  }
  else if (reading2 == 1){
    for (i=0; i<=5; i++){
      leds[i]= CRGB::White;
      FastLED.setBrightness(0);
      FastLED.show();
    }
  }
}

void handle_entry_IR_Message(AdafruitIO_Data *data) {

  int c= data->toInt();
  Serial.print("received <- ");
  Serial.println(c);
  if (c%4 != 0){
    for (i=0; i<=5; i++){
      leds[i]= CRGB::White;
      FastLED.setBrightness(255);
      FastLED.show();
    }
  }
  else {
    for (i=0; i<=5; i++){
      leds[i]= CRGB::White;
      FastLED.setBrightness(0);
      FastLED.show();
    }
  }
}
