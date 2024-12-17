#ifndef INSERT
#define INSERT

#include <iostream>
#include <fstream>
#include <filesystem>
#include <nlohmann/json.hpp>
#include "array.h"
#include "instruments.h"

using namespace std;
namespace fs = filesystem;
using json = nlohmann::json;

int countCol(string tableName);
bool checkValues (StrArray& valuesToCol);
bool fileAvail (string nextCsv, string tablePath);
void checkCsv (string tablePath, string tableName, string& csv);
void insertCom(string userCommand, string baseName);

#endif // INSERT