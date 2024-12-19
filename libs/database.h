#ifndef DATABASE
#define DATABASE

#include <iostream>
#include <string>
#include <sstream>
#include "read_json.h"
#include "insert.h"
#include "delete.h"
#include "select.h"

using namespace std;

string database(string userCommand, string baseName);

#endif // DATABASE