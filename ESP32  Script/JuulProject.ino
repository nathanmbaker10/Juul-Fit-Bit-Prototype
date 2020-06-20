#include "BluetoothSerial.h"

// Defined Constants
#define HIT_COUNTER_LIMIT 1800
#define DOUBLE_TAP_COUNTER_LIMIT 110
#define CARTRIDGE_REMOVE_LIMIT 100

// Static Global Variables 
static BluetoothSerial SerialBT;
static int numBytes = 0;
static int num_hits = 0;
static int num_taps = 0;
static int globalTimer;
static bool cartridge_removed = false;
static bool juulActivated = false;

void setup() {
  // put your setup code here, to run once:
  SerialBT.begin("MyJuul");
  Serial1.begin(57600, SERIAL_8N1, 26, 25);
}


void loop() {
  if (millis() - globalTimer > 1000 ) { // Runs this code after 1s
   if (juulActivated) {
      switch (numBytes) {
        case 0 ... CARTRIDGE_REMOVE_LIMIT:
          cartridge_removed  = true;
          SerialBT.println("JUUL: Cartridge Removed");
          break;
        case CARTRIDGE_REMOVE_LIMIT + 1 ... DOUBLE_TAP_COUNTER_LIMIT - 1:
          SerialBT.print("JUUL: Noise Bytes \n");
          break;
        case DOUBLE_TAP_COUNTER_LIMIT ... HIT_COUNTER_LIMIT - 1:
           if(cartridge_removed) {
             cartridge_removed = false;
             SerialBT.println("JUUL: Cartridge Inserted");
           } else {
             num_taps++;
             SerialBT.println("JUUL: Double Tap Detected");
             SerialBT.print("NumTaps: ");
             SerialBT.println(num_taps); 
           }           
           break;
        case HIT_COUNTER_LIMIT ... HIT_COUNTER_LIMIT * 2:
           num_hits++;
           SerialBT.println("JUUL: Hit Detected");
           SerialBT.print("NumHits: ");
           SerialBT.println(num_hits);
           break;
        default: 
           SerialBT.println("JUUL: Excessive Amount of Bytes Sent");
      }
      SerialBT.print("NumBytes: ");
      SerialBT.print(numBytes);
      SerialBT.print("\n");
   }
    numBytes = 0;
    juulActivated = false;
  }

  while (Serial1.available() > 0) { 
    juulActivated = true; 
    globalTimer = millis();
    numBytes++;
    Serial1.read();
  }
}
