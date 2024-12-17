#ifndef READ_JSON
#define READ_JSON

#include <iostream>
#include <fstream>
#include <filesystem>
#include <nlohmann/json.hpp>

using namespace std;
namespace fs = filesystem;
using json = nlohmann::json;

string remQuotes(string column);
void readJson(string& baseName);

#endif // READ_JSON