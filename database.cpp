#include "libs/database.h"

using namespace std;

string database(string userCommand, string baseName){
    stringstream ss(userCommand);
    string temp;
    ss >> temp;
    string message;
    if (temp == "EXIT"){
        message = "EXIT";
        
    }else if (temp == "INSERT"){
        message = insertCom(userCommand, baseName);

    }else if (temp == "DELETE"){
        message = delCom(userCommand, baseName);

    }else if (temp == "SELECT"){
        message = selectCom(userCommand, baseName);

    }else{
        message = "ERROR_1: Unknown command.";
    }

    return message;
}