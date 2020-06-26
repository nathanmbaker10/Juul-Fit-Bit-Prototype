#include "BluetoothSerial.h"

#define ANALOG_GPIO 34
#define ANALOG_EVENT_THRESH 3000
#define EVENT_TIME_BUFFER 200

// All times are in MS
#define HIT_TIME_LIMIT 15000
#define DOUBLE_TAP_TIME_LIMIT 1100
#define CARTRIDGE_TIME_LIMIT 500

BluetoothSerial SerialBT;
enum JuulState {CartridgeRemoved, CartridgeInserted,Hit, DoubleTap};
static int AnalogVal = 0;
static bool JuulActivated = false;
static int EventStartTime = 0;
static int GlobalTimer = 0;
static JuulState EventState = CartridgeInserted;
static int NumTaps = 0;
static int NumHits = 0;

// Modifies Static Variable EventState
void runStateMachine(int eventDuration) {
    Serial.print("Duration: ");
    Serial.println(eventDuration);
    switch (eventDuration) {
        case 0 ... CARTRIDGE_TIME_LIMIT:                    // Cartridge Removed
          EventState = CartridgeRemoved;
          SerialBT.print("1");
          Serial.println("JUUL: Cartridge Removed");
          break;
        case CARTRIDGE_TIME_LIMIT + 1 ... DOUBLE_TAP_TIME_LIMIT - 1: // Noise 
          SerialBT.print("0");
          Serial.print("JUUL: Noise \n");
          break;
        case DOUBLE_TAP_TIME_LIMIT ... HIT_TIME_LIMIT - 1:  // Cartridge Inserted
           if(EventState == CartridgeRemoved) {
             // Cartridge Inserted
             EventState = CartridgeInserted;
             SerialBT.print("2");
             Serial.println("JUUL: Cartridge Inserted"); 
           } else {                                         // Tap Detected
             EventState = DoubleTap;
             NumTaps++;
             SerialBT.print("3");
             Serial.println("JUUL: Double Tap Detected");
             Serial.print("NumTaps: ");
             Serial.println(NumTaps); 
           }           
           break;
        case HIT_TIME_LIMIT ... HIT_TIME_LIMIT * 2:         // Hit Detected
           EventState = Hit;
           NumHits++;
           SerialBT.print("4");
           Serial.println("JUUL: Hit Detected");
           Serial.print("NumHits: ");
           Serial.println(NumHits);
           break;
        default: 
           SerialBT.print("5");
           Serial.println("JUUL: Excessive Time ");
      }
}

void setup() {
  SerialBT.begin("ESP32");
  Serial.begin(115200);
  Serial1.begin(9600, SERIAL_8N1, 26,27);
}

void loop() {
  if(millis() - GlobalTimer > EVENT_TIME_BUFFER){
    if(JuulActivated) {
      int eventDuration = millis() - EventStartTime;
      runStateMachine(eventDuration);
      JuulActivated = false;
    }
  }

  while(analogRead(ANALOG_GPIO) <= ANALOG_EVENT_THRESH) {
    if (!JuulActivated) {
      EventStartTime = millis();
    }
    GlobalTimer = millis();
    JuulActivated = true;
  }
}
