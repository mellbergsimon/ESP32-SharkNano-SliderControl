#ifndef DATABASE_H
#define DATABASE_H

#include <map>
#include <string>
#include "SecuredLinkedList.h"
#include <vector>

#include <stdexcept>

struct item {
  int selectedParameter;
  const char * name;
  SecuredLinkedList<const char *> * parameters;
};

struct List {
  int selectedItem;
  SecuredLinkedList<item *> * items;
};

class Database {
public:
  Database();

  void addItem(const std::string& key, item * i);
  void addContent(const std::string& key, const List * l);

  void incrementList(const std::string& key);
  void decrementList(const std::string& key);


  List* getList(const std::string& key);

  SecuredLinkedList<item *>* getItems(const std::string& key);
  item * getSelectedItem(const std::string& key);


  SecuredLinkedList<const char *>* getParameters(const std::string& key);
  const char * getSelectedParameter(const std::string& key);

  void updateSelectedParameter(std::string key, int newVal);

  List * createList();
  item * createItem(const char * itemName, const char* strings[]);

  SecuredLinkedList<const char *> * getKeys();

  std::map<std::string, List>& getContent();

  const char * selectedKey;

private:
  std::map<std::string, List> content; // No need for pointer, saves memory
  SecuredLinkedList<const char *> * keys;

  void addKey(const std::string & key);

  SecuredLinkedList<const char *> * createParameters(const char* strings[]);

};

#endif
