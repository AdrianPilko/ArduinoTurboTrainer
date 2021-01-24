// Adrian Pilkington 2021
// 

#include <TM1638plus.h>
// GPIO I/O pins on the Arduino connected to strobe, clock, data,
//pick on any I/O you want.
#define  STROBE_PIN 4
#define  CLOCK_PIN 6  
#define  DATA_PIN 7 
bool freq = false; 

//Constructor object (GPIO STB , GPIO CLOCK , GPIO DIO, use high freq MCU)
TM1638plus module(STROBE_PIN, CLOCK_PIN, DATA_PIN, freq);


#define SWITCH_LOAD_INCREASE 2
#define SWITCH_LOAD_DECREASE 1

byte buttons;
int program = 0;
// number of time units in 1 minute
#define MINUTES (60 * 1000)
#define SECONDS (1000)

unsigned long lastFire = 0;
unsigned long timerSinceStart = 0;

// relay output pins
#define RELAY_1_PIN 8
#define RELAY_2_PIN 9
#define RELAY_3_PIN 10
#define RELAY_4_PIN 11
#define NUM_RELAY 4

#define SOFTWARE_ISSUE_NUM 3

int currentLoadValue = 0;

void setup() 
{
  char workString[9];
  for (int i = RELAY_1_PIN; i <= NUM_RELAY+RELAY_1_PIN;i++)
  {
     pinMode(i, OUTPUT);
     switchRelayOff(i);
  }
  Serial.begin(9600);
  module.displayBegin();
  sprintf(workString, "swr=%d", SOFTWARE_ISSUE_NUM);
  module.displayText(workString);
  delay(2000);
  module.reset();
}


void switchRelayOn(int relayNum)
{
      digitalWrite(relayNum, HIGH); 
}

void switchRelayOff(int relayNum)
{
      digitalWrite(relayNum, LOW); 
}

void disableAllRelays()
{
  for (int i = RELAY_1_PIN; i <= NUM_RELAY+RELAY_1_PIN;i++)
  {                         
    switchRelayOff(i);   
  }
}
 
void loop()
{
  char workString[9];

  buttons=module.readButtons();

  // Debounce button press
  if (millis() - lastFire < 200) return;

  //add time since last here to timerSinceStart for exercise program progression 
  timerSinceStart += millis() - lastFire;
  
  lastFire = millis();

  // handle update currentLoadValue after press, or do other stuff later to use other buttons
  press(buttons);

  // update program state
  switch (program)
  {
    case 0 : break;
    case 1 : NormalProgram(); break;
    case 2 : HITProgram(); break;
    case 3 : HardProgram(); break;
    case 4 : LongFairlyHardProgram(); break;
    default: break;
  }
  
  sprintf(workString, "LD=%dPro%d", currentLoadValue,program);
  module.displayText(workString); 
  module.setLED(currentLoadValue, 1);
  module.setLED(currentLoadValue+1, 0);
  if (currentLoadValue-1 >= 0) module.setLED(currentLoadValue-1, 0);
  
  switch (currentLoadValue)
  {
    case 0 : disableAllRelays();     break;
    case 1 : switchRelayOn(RELAY_1_PIN); switchRelayOff(RELAY_2_PIN); switchRelayOff(RELAY_3_PIN); switchRelayOff(RELAY_4_PIN); break;  
    case 2 : switchRelayOn(RELAY_1_PIN); switchRelayOn(RELAY_2_PIN); switchRelayOff(RELAY_3_PIN); switchRelayOff(RELAY_4_PIN); break;                   
    case 3 : switchRelayOn(RELAY_1_PIN); switchRelayOn(RELAY_2_PIN); switchRelayOn(RELAY_3_PIN); switchRelayOff(RELAY_4_PIN); break;                     
    case 4 : switchRelayOn(RELAY_1_PIN); switchRelayOn(RELAY_2_PIN); switchRelayOn(RELAY_3_PIN); switchRelayOn(RELAY_4_PIN); break; 
    default:     disableAllRelays(); break;
  }                                 
}

// exercise programs, these are built in presets different resistance profiles 

void NormalProgram()
{
   // 18minute total
   // 3minute low => 5minutes medium power => 2minute high => 5minutes medium => 3minutes low
   if (timerSinceStart < 3*MINUTES)
   {
      currentLoadValue = 1;
   }
   else if (timerSinceStart < 8*MINUTES)
   {
      currentLoadValue = 2;
   }
   else if (timerSinceStart < 10*MINUTES)
   {
      currentLoadValue = 3;
   }
   else if (timerSinceStart < 15*MINUTES)
   {
      currentLoadValue = 2;
   }
   else if (timerSinceStart < 18*MINUTES)
   {
      currentLoadValue = 1;
   }
}

void HITProgram()
{
   // 15minute total
   // 2minute low => 2minutes medium power => 30second max => 1minutes medium => 30seconds max => 1minute medium => 30seconds max => 2minutes low
   if (timerSinceStart < 2*MINUTES)
   {
      currentLoadValue = 1;
   }
   else if (timerSinceStart < 3*MINUTES)
   {
      currentLoadValue = 2;
   }
   else if (timerSinceStart < (3*MINUTES)+(30*SECONDS))
   {
      currentLoadValue = 4;
   }
   else if (timerSinceStart < (4*MINUTES)+(30*SECONDS))
   {
      currentLoadValue = 2;
   }
   else if (timerSinceStart < 5*MINUTES)
   {
      currentLoadValue = 4;
   }
   else if (timerSinceStart < 6*MINUTES)
   {
      currentLoadValue = 2;
   }
   else if (timerSinceStart < (6*MINUTES)+(30*SECONDS))
   {
      currentLoadValue = 4;
   }
   else 
   {
      currentLoadValue = 1;
   }
}

void HardProgram()
{
   // 15minute total
   // 2minute low => 2minutes medium power => 30second max => 1minutes medium => 30seconds max => 1minute medium => 30seconds max => 2minutes low
   if (timerSinceStart < 2*MINUTES)
   {
      currentLoadValue = 1;
   }
   else if (timerSinceStart < 15*MINUTES)
   {
      currentLoadValue = 4;
   }
   else 
   {
      currentLoadValue = 1;
   }
}

void LongFairlyHardProgram()
{
   if (timerSinceStart < 25*MINUTES)
   {
      currentLoadValue = 3;
   }
   else 
   {
      currentLoadValue = 1;
   }
}

void press(int button) 
{
  char workString[20];
  /// left most button S1, buttons returns 1, press S2 and get 2, press S3 and get 4, press S4 and get 8, S5=16, S6=32, S7=64, and S8 returns 128
 
  // check for preset program buttons 5th 6th 7th 8th buttons from left
  // when those are pressed the load increments are ignored
  if (button == 16)
  {
    program = 1;
    timerSinceStart = 0;
    Serial.println("Program = 1");
    return;
  }
  else if (button == 32)
  {
    program = 2;
    timerSinceStart = 0;
    Serial.println("Program = 2");
    return;
  }
  else if (button == 64)
  {
    program = 3;
    timerSinceStart = 0;
    Serial.println("Program = 3");
    return;
  }
  else if (button == 128)
  {
    program = 4;
    timerSinceStart = 0;
    Serial.println("Program = 4");
    return;
  }

  
  // change load value
  if (button == SWITCH_LOAD_INCREASE)
  {
    currentLoadValue +=1;
    program = 0; // reset program to manual control
    sprintf(workString,"Load increase = %d",currentLoadValue);
    
  }
  if (button == SWITCH_LOAD_DECREASE)
  {
    currentLoadValue -=1;
    program = 0; // reset program manual control
    sprintf(workString,"Load increase = %d",currentLoadValue);
  }
  Serial.println(workString);
  // limit the load value to valid range
  if (currentLoadValue > NUM_RELAY) currentLoadValue = NUM_RELAY;
  if (currentLoadValue < 0) currentLoadValue = 0;
}
