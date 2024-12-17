#ifndef INSTRUMENTS
#define INSTRUMENTS

#include <iostream>
#include <fstream>
#include <filesystem>
#include <nlohmann/json.hpp>
#include "array.h"

using namespace std;
namespace fs = filesystem;
using json = nlohmann::json;

int countWords (string words);
bool checkSyntax(string& tableName, string& values, string baseName, string userCommand, string command);
bool checkTable(string tablePath);
bool isLock(string tablePath, string tableName);
void lockTable (string tablePath, string tableName);
void unlockTable (string tablePath, string tableName);
bool columnExist (string tableName, string columnName);
int getTurplLim ();
bool csvAvail (string csv);
void getTablCol (string tablCol, string& table, string& column);
int colIndex (string table, string column);
bool checkDot(string elem);
bool checkSecondElem (string elem, string command);
bool checkZeroElem (StrArray& tables, string inputTabNam);
bool correctCond(StrArray& condArr, StrArray& tables, string command);
void replaceTok(StrArray& tokens, string nextToken);
void toTokens(StrArray& condArr, StrArray& tokens);
string withoutApostr (string word);
bool checkRes (string result);

#endif // INSTRUMENTS