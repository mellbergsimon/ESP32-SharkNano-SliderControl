#include "database.h"

Database::Database() {
  keys = new SecuredLinkedList<const char*>;
}

void Database::addContent(const std::string& key, const List * l) {
  addKey(key);
  content.emplace(key, *l);
  selectedKey = key.c_str();
}

void Database::addKey(const std::string& key){
  keys->push(key.c_str());
}


List* Database::getList(const std::string& key) {
  if (content.find(key) != content.end()) {
    return &content.at(key);
  }
  return nullptr;
}


//-------------------------------------------------
SecuredLinkedList<item *>* Database::getItems(const std::string& key){
  return getList(key)->items;
}
item * Database::getSelectedItem(const std::string& key) {
  List * l = getList(key);
  return l->items->get(l->selectedItem);
}
//-------------------------------------------------



//-------------------------------------------------
SecuredLinkedList<const char *>* Database::getParameters(const std::string& key) {
  item * i = getSelectedItem(key);
  return i->parameters;
}

const char * Database::getSelectedParameter(const std::string& key){
  item * i = getSelectedItem(key);
  return i->parameters->get(i->selectedParameter);
}
//-------------------------------------------------



//-------------------------------------------------
void Database::incrementList(const std::string& key) {
  List * l = getList(key);
  
  l->selectedItem = (abs(l->selectedItem) + 1) % l->items->size();
}

void Database::decrementList(const std::string& key) {
  List * l = getList(key);
  
  l->selectedItem = (l->selectedItem) - 1 % l->items->size();
}
//-------------------------------------------------




void Database::addItem(const std::string& key, item * i){
  content.at(key).items->push(i);
}




List * Database::createList(){
  List * l = new List;
  l->selectedItem = 0;
  l->items = new SecuredLinkedList<item *>;

  return l;
}

item * Database::createItem(const char * itemName, const char* strings[]){
  
  item * i = new item;

  i->name = itemName;
  i->parameters = createParameters(strings);
  i->selectedParameter = 0;

  return i;
}



SecuredLinkedList<const char *> * Database::createParameters(const char* strings[]) {
    // Create a new SecuredLinkedList
    SecuredLinkedList<const char *> * list = new SecuredLinkedList<const char *>;

    // Populate the list with elements from the strings array
    for (const char **c = strings; *c; c++) {
      list->push(*c);
    }

    return list;
}

void Database::updateSelectedParameter(std::string key, int newVal){
  item * i = getSelectedItem(key);

  int newSelection = (newVal + i->parameters->size()) % i->parameters->size();

  i->selectedParameter = abs(newSelection);
}

std::map<std::string, List>& Database::getContent(){
  return content;
}

SecuredLinkedList<const char *> * Database::getKeys() {

  return keys;
}