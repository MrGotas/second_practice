#ifndef DELETE
#define DELETE

#include <iostream>
#include <fstream>
#include <filesystem>
#include <nlohmann/json.hpp>
#include "array.h"
#include "instruments.h"

using namespace std;
using json = nlohmann::json;

bool checkCond (StrArray& tokens, string line);
void addToTemp (string line, string tempPath, string firstLine);
bool checkVals (string line, string condVal, int valuePos);
void delCom(string userCommand, string baseName);

#endif // DELETE