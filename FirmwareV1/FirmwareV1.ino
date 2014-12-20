#include <LiquidCrystal.h>
#include <Wire.h>
#include "Hardware.h"
#include "Shiftduino.h"
#include "Place.h"
#include "SRF08.h"

//#define DEBUG

#define NUM_PLACES    4
const uint8_t adrresses[NUM_PLACES] = {
  0xE0, 0xE2, 0xE4, 0xE6};
#define MESSAGE_INTERVAL  150 
#define TEMPERATURE_INTERVAL  1000

//Objects
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);
Shiftduino sd(DATA_PIN, CLOCK_PIN, LATCH_PIN, 1);
Place places[NUM_PLACES];

//Variables
String fullMessage;
String partialMessage;
char temperatureString[10];
int index = 0;
long prevMessageTime = 0;
long prevTemperatureTime = -TEMPERATURE_INTERVAL;
int rightFree = NUM_PLACES / 2;
int leftFree = NUM_PLACES / 2;

//
// setup
//
void setup() {

#ifdef DEBUG
  Serial.begin(9600);
#endif

  Wire.begin();
  for(int i = 0; i < NUM_PLACES; i++)
    places[i].begin(adrresses[i], &sd, i * 2, (i * 2) +1);
  lcd.begin(16, 2);
  // Print a message to the LCD.
  lcd.setCursor(0, 2);  
  lcd.print("2 <- LIBRES -> 2");
  fullMessage = "    Bienvenidos a PARKINGUINO";
}

//
// loop
//
void loop() {

  //Update place status
  for(int i = 0; i < NUM_PLACES; i++) {
    places[i].update();
    delay(10);
#ifdef DEBUG
    Serial.print("Update Place ");
    Serial.println(i, DEC);
#endif
  }
  //Update left free places on the LCD
  int tempFree = 0;
  for (int i = 0; i < (NUM_PLACES / 2); i++)
    if (places[i].getState() == false)
      tempFree++;
  if (tempFree != leftFree) {
    leftFree = tempFree;
    lcd.setCursor(0, 2);
    lcd.print(leftFree, DEC);
  }
  //Update right free places on the LCD
  tempFree = 0;
  for (int i = (NUM_PLACES / 2); i < NUM_PLACES; i++)
    if (places[i].getState() == false)
      tempFree++;
  if (tempFree != rightFree) {
    rightFree = tempFree;
    lcd.setCursor(15, 2);
    lcd.print(rightFree, DEC);
  }
  //get temperature value
  if ((millis() - prevTemperatureTime) > TEMPERATURE_INTERVAL) {
    float temperature = analogRead(TEMPERATURE_PIN) * 0.48;
    fullMessage = "    Bienvenidos a PARKINGUINO    Temperatura: ";
    fullMessage += floatToString(temperatureString, temperature, 1, 0);
    fullMessage += "*C";        //insert * to replace after for lcd special symbol 'º'
  }
  //print message in the first line
  if ((millis() - prevMessageTime) > MESSAGE_INTERVAL) {
    prevMessageTime = millis();
    index++;
    if (index >= fullMessage.length())
      index = 0;
    if ((index + 16) <= fullMessage.length())
      partialMessage = fullMessage.substring(index, index + 16);    
    else {
      partialMessage = fullMessage.substring(index, fullMessage.length());
      if (partialMessage.length() < 16)
        partialMessage += fullMessage.substring(0, 16 - partialMessage.length());
    }
    //correct º symbol
    lcd.setCursor(0, 0);  
    int specialIndex = partialMessage.indexOf('*');
    if (specialIndex == -1)
      lcd.print(partialMessage);
    else {
      String tempString = partialMessage.substring(0, specialIndex);
      lcd.print(tempString);
      lcd.print((char)223);
      tempString = partialMessage.substring(specialIndex + 1, 16);
      lcd.print(tempString);
    }
  }  
}

char * floatToString(char * outstr, double val, byte precision, byte widthp){
  char temp[16];
  byte i;

  // compute the rounding factor and fractional multiplier
  double roundingFactor = 0.5;
  unsigned long mult = 1;
  for (i = 0; i < precision; i++)
  {
    roundingFactor /= 10.0;
    mult *= 10;
  }
  
  temp[0]='\0';
  outstr[0]='\0';

  if(val < 0.0){
    strcpy(outstr,"-\0");
    val = -val;
  }

  val += roundingFactor;

  strcat(outstr, itoa(int(val),temp,10));  //prints the int part
  if( precision > 0) {
    strcat(outstr, ".\0"); // print the decimal point
    unsigned long frac;
    unsigned long mult = 1;
    byte padding = precision -1;
    while(precision--)
      mult *=10;

    if(val >= 0)
      frac = (val - int(val)) * mult;
    else
      frac = (int(val)- val ) * mult;
    unsigned long frac1 = frac;

    while(frac1 /= 10)
      padding--;

    while(padding--)
      strcat(outstr,"0\0");

    strcat(outstr,itoa(frac,temp,10));
  }

  // generate space padding 
  if ((widthp != 0)&&(widthp >= strlen(outstr))){
    byte J=0;
    J = widthp - strlen(outstr);
    
    for (i=0; i< J; i++) {
      temp[i] = ' ';
    }

    temp[i++] = '\0';
    strcat(temp,outstr);
    strcpy(outstr,temp);
  }
  
  return outstr;
}



