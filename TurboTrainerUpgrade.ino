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

unsigned long lastFire = 0;


// relay output pins
#define RELAY_1_PIN 8
#define RELAY_2_PIN 9
#define RELAY_3_PIN 10
#define RELAY_4_PIN 11
#define NUM_RELAY 4

int currentLoadValue = 0;

// the setup function runs once when you press reset or power the board
void setup() 
{
  for (int i = RELAY_1_PIN; i <= NUM_RELAY+RELAY_1_PIN;i++)
  {
     pinMode(i, OUTPUT);
     switchRelayOff(i);
  }
  Serial.begin(9600);
  module.displayBegin();
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
  char workString[11];

  buttons=module.readButtons();

  // Debounce button press
  if (millis() - lastFire < 200) return;
  lastFire = millis();

  // handle update currentLoadValue after press, or do other stuff later to use other buttons
  press(buttons);
  sprintf(workString, "LOAD=%d", currentLoadValue);
  module.displayText(workString); 
  module.setLED(currentLoadValue, 1);
  module.setLED(currentLoadValue+1, 0);
  if (currentLoadValue-1 >= 0) module.setLED(currentLoadValue-1, 0);
  
  Serial.println(buttons);
  Serial.println(currentLoadValue);


  switch (currentLoadValue)
  {
    case 0 : disableAllRelays();     break;
    case 1 : switchRelayOn(RELAY_1_PIN); switchRelayOff(RELAY_2_PIN); switchRelayOff(RELAY_3_PIN); switchRelayOff(RELAY_4_PIN); break;  
    case 2 : switchRelayOn(RELAY_1_PIN); switchRelayOn(RELAY_2_PIN); switchRelayOff(RELAY_3_PIN); switchRelayOff(RELAY_4_PIN); break;                   
    case 3 : switchRelayOn(RELAY_1_PIN); switchRelayOn(RELAY_2_PIN); switchRelayOn(RELAY_3_PIN); switchRelayOff(RELAY_4_PIN); break;                     
    case 4 : switchRelayOn(RELAY_1_PIN); switchRelayOn(RELAY_2_PIN); switchRelayOn(RELAY_3_PIN); switchRelayOn(RELAY_4_PIN); break;     
  }                                 
}




void press(int button) { // Our handler
  
  
  // change load value
  if (button == SWITCH_LOAD_INCREASE)
  {
    currentLoadValue +=1;
  }
  if (button == SWITCH_LOAD_DECREASE)
  {
    currentLoadValue -=1;
  }
  // limit the load value to valid range
  if (currentLoadValue > NUM_RELAY) currentLoadValue = NUM_RELAY;
  if (currentLoadValue < 0) currentLoadValue = 0;
}
