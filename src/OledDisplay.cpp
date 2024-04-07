#include "OledDisplay.h"

#include "database.h"

OledDisplay::OledDisplay(){

  display = Adafruit_SSD1306(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);


  this->initDisplay();
  display.setRotation(2);
  
  
  // // Invert and restore display, pausing in-between
  // display.invertDisplay(true);
  // delay(1000);
  // display.invertDisplay(false);
  // delay(1000);


}


void OledDisplay::print(const char *str){
  display.print(F(str));
}

void OledDisplay::printMenuWithDatabase(Database * db){
  display.clearDisplay();
  display.stopscroll();
  display.setTextSize(1); // Draw 2X-scale text
  display.setTextColor(WHITE);

  display.setCursor(0, 0);

  printList(db);

  printMenu(db);
  display.display();
}

void OledDisplay::scrollList(Database * db) {
  display.setTextColor(SSD1306_WHITE);
  
  for (int i = 8; i > 0; i--) {
    display.clearDisplay();
    display.setCursor(0, i);

    printList(db);

    printMenu(db);
    display.display();
    delay(10);
  }

}

void OledDisplay::printList(Database* db) {
  List* currentList = db->getList(db->selectedKey);
  SecuredLinkedList<item*>* items = currentList->items;

  int listLength = items->size();
  if (listLength == 0){ //Return if the list is empty.
    return;
  }
  
  for (int i = -2; i <= 2; ++i) {
    display.setTextColor(WHITE);
    int index = (currentList->selectedItem + i + listLength) % listLength;
    item* currentItem = items->get(index);

    display.printf("%-14s", currentItem->name);

    if (i == 0) {
      display.setTextColor(BLACK, WHITE);
    }

    //Print the value.
    display.printf("%5s", currentItem->parameters->get(currentItem->selectedParameter));

    // Move cursor to the next line
    display.println();
  }
}

void OledDisplay::printMenu(Database * db) {
  display.setCursor(0, 0);
  display.fillRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT/4, BLACK); //Cover the part where this function prints.

  SecuredLinkedList<const char *> * keys = db->getKeys();

  const auto& content = db->getContent();

  auto selectedKey = content.lower_bound(db->selectedKey);
  
  for (auto key = selectedKey; key != content.end(); ++key) {
    display.setTextColor(WHITE);
    if (key->first == selectedKey->first) { // Compare the keys
        display.setTextColor(BLACK, WHITE);
    }
    display.printf("%s ", key->first.c_str());
  }
  
  
  // for (const auto& pair : content) {
  //   display.printf("%s ", pair.first.c_str());
  // }

  // for (int i = 0; i < keys->size(); i++){
  //   const char * currentKey = keys->get(i);

  //   //Color formatting
  //   if (currentKey == db->selectedKey){
  //     display.setTextColor(BLACK, WHITE); //Black on white BG
  //   }
  //   // Printing
  //   print(keys->get(i));

  //   display.setTextColor(WHITE);
    
  //   print(" ");
  // }
}


void OledDisplay::initDisplay() {
  
  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
  display.setTextWrap(false);
  // Show initial display buffer contents on the screen --
  // the library initializes this with an Adafruit splash screen.
  display.display();
  delay(2000);


}