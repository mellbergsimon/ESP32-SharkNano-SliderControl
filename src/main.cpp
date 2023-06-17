/**
 * Ska kolla på denna video:
 *
 */


//Custom includes
#include "nanoWebServer.h"
#include "nanoBluetooth.h"




void setup() {
  Serial.begin(115200);

  websetup();
  bluetoothsetup();

} // End of setup.

// This is the Arduino main loop function.
void loop() {
  bluetoothLoop();


  delay(1000); // Delay a second between loops.
} // End of loop
