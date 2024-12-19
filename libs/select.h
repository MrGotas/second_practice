#ifndef SELECT
#define SELECT

#include <iostream>
#include <fstream>
#include <filesystem>
#include <nlohmann/json.hpp>
#include "array.h"
#include "instruments.h"

using namespace std;
namespace fs = filesystem;
using json = nlohmann::json;


void crossJoin(string baseName, StrArray& outputTables, int nowTable, string tempStr);
void clearTemp(string& tempStr, int wasWords);
int tempStrSize(string tempStr);
void addToTemp(string baseName, string tempStr);
void withoutTablePk (string& line);
string getColFromJson(StrArray& outputTables);
int getPosVal(string temp, string allCol);
bool trueToken(string line, int valPos, string allCol, string temp);
bool conditions(StrArray& tokens, string allCol, string line);
string getResStr(string baseName, StrArray& outputCol, StrArray& tokens);
string getValues(StrArray& outputCol, string line, string allCol);
void correctOutput(string result, int wordsPerRow);
void removeComma(string& value);
bool checkOutputCol(string& tableCol, string baseName);
bool checkFrom (StrArray& fromTable, StrArray& outputCol);
void massWithoutCol (StrArray& outputCol, StrArray& outputTables);
string getValueFromLine(string line, int columnNum);
bool isLockTables(StrArray& tables, string baseName);
void lockTables(StrArray& tables, string baseName, string command);
string selectCom(string userCommand, string baseName);

#endif // SELECT