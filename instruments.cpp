#include "libs/instruments.h"

int countWords (string words){
    stringstream ss (words);
    string word;
    int countWords = 0;

    while (ss >> word){
        countWords++;
    }

    return countWords;
}

bool checkSyntax(string& tableName, string& values, string baseName, string userCommand, string command){
    int countWor = countWords(userCommand); // проверка кол-ва слов
    if (command == "INS" && countWor < 5 || command == "DEL" && countWor < 7){
        cerr << "ERROR_2: Unknown command." << endl;
        return false;
    }

    stringstream ss (userCommand);
    string temp;
    ss >> temp; // первое слово

    ss >> temp;
    if (command == "INS" && temp != "INTO" || command == "DEL" && temp != "FROM"){
        cerr << "ERROR_3: Unknown command." << endl;
        return false;
    }

    ss >> tableName;

    ss >> temp;
    if (command == "INS" && temp != "VALUES" || command == "DEL" && temp != "WHERE"){
        cerr << "ERROR_4: Unknown command." << endl;
        return false;
    }

    while (ss >> temp){
        values += temp + " ";
    }

    return true;
}

bool checkTable(string tablePath){
    if (fs::exists(tablePath) && fs::is_directory(tablePath)){
        return true;
    }else{
        return false;
    }
}

bool isLock(string tablePath, string tableName){
    string path = tablePath + "/" + tableName + "_lock";
    ifstream file (path);
    if (!file.is_open()){
        cerr << "ERROR_8: Unable to open file: " << path << endl;
        return false;
    }

    int value;
    file >> value;
    file.close();

    if (value == 1){
        return true;
    }else if (value == 0){
        return false;
    }else{
        cerr << "ERROR_9: Unknown value from file: " << path << endl;
        return false;
    }
}

void lockTable (string tablePath, string tableName){
    string path = tablePath + "/" + tableName + "_lock";
    ofstream file (path);
    file << 1;
    file.close();
}

void unlockTable (string tablePath, string tableName){
    string path = tablePath + "/" + tableName + "_lock";
    ofstream file (path);
    file << 0;
    file.close();
}

bool columnExist (string tableName, string columnName){
    ifstream file("files/schema.json");
    json jsonData;
    file >> jsonData;
    file.close();

    if (jsonData.contains("structure") && jsonData["structure"].contains(tableName)){
        const auto& columns = jsonData["structure"][tableName];
        for (const auto& column : columns){
            if (column == columnName){
                return true;
            }
        }
    }
    return false;
}

int getTurplLim (){
    ifstream jsonFile("files/schema.json");
    json configs;
    jsonFile >> configs;
    jsonFile.close();

    return configs["tuples_limit"];
}

bool csvAvail (string csv){
    if (fs::exists(csv)){
        return true;
    }else{
        return false;
    }
}

void getTablCol (string tablCol, string& table, string& column){
    stringstream ss(tablCol);
    getline(ss, table, '.');
    getline(ss, column);
}

int colIndex (string table, string column){
    ifstream jsonFile("files/schema.json");
    json configs;
    jsonFile >> configs;
    jsonFile.close();

    const auto& tableColumns = configs["structure"][table];
    int columnPos = 1; // чтобы пропустить table_pk
    for (const auto& tableColumn : tableColumns){ //нахоидм до какого значения строки идти
        columnPos++;
        if (tableColumn == column){
            break;
        }
    }
    return columnPos;
}

bool checkZeroElem (StrArray& tables, string inputTabNam){ // проверка существования таблицы
    for (size_t i = 0; i < tables.sizeM(); i++){
        string table = tables.get(i);

        if (inputTabNam == table) return true;
    }
    return false;
}

bool checkDot(string elem){ // проверка корректного количества точек
    int counter = 0;
    for (size_t i = 0; i < elem.size(); i++){
        if (elem[i] == '.'){
            counter++;
        }
    }
    if (counter == 1){
        return true;

    }else return false;
}

bool checkSecondElem (string elem, string command){
    if (elem[0] == '\''){
        if (elem.size() < 2 || elem[elem.size() - 1] != '\''){
            return false;

        }else return true;

    }else if (checkDot(elem)){
        stringstream ss(elem);
        string tableName, columnName;
        getline(ss,tableName, '.');
        getline(ss, columnName);

        StrArray table;
        table.push(tableName);
        if (checkZeroElem(table, tableName) && columnExist(tableName, columnName)){
            if (command == "DELETE"){
                return false; 

            } else return true;

        }else return false;

    }else return false;
}

bool correctCond(StrArray& condArr, StrArray& tables, string command){
    int numElem = 0, countTokens = 1, countLogicOp = 0; // елементов просмотренно
    for (size_t i = 0; i < condArr.sizeM(); i++){
        string elem = condArr.get(i);
        string temp; // записываем готовый токен

        if (numElem > 3){
            return false;

        }else if (elem == "="){
            if (numElem != 1){
                return false;
            }else{
                numElem++;
            }

        }else if (numElem == 0){ // левая часть равенства
            stringstream ss (elem);
            string inputTabNam;
            getline(ss, inputTabNam, '.');
            if (!checkZeroElem(tables, inputTabNam)){
                return false;
            }

            string column;
            getline(ss, column);
            if (!columnExist(inputTabNam, column)){
                return false;
            }
            numElem++;

        }else if(numElem == 2){ // правая часть равенства
            if (!checkSecondElem(elem, command)){
                return false;
            }else{
                numElem++;
            }

        }else if (elem == "OR" || elem == "AND"){
            if (numElem != 3){
                return false;
            }else{
                numElem = 0;
                countTokens++;
                countLogicOp++;
            }

        }else{
            return false;
        }
    }

    if (countLogicOp != countTokens - 1){ // OR или AND > либо < токенов
        return false;
    }else if (numElem != 3){ // недозаполненные значения
        return false;
    }else{
        return true;
    }
}

void replaceTok(StrArray& tokens, string nextToken){
    string lastTok = tokens.get(tokens.sizeM() - 1);
    tokens.replace(tokens.sizeM() - 1, lastTok + " " + nextToken);
}

void toTokens(StrArray& condArr, StrArray& tokens){
    int countLogicOp = condArr.sizeM() / 4; // количество OR/AND в массиве
    if (countLogicOp == 0){
        string token = condArr.get(0) + " " + condArr.get(2);
        tokens.push(token);
        return;
    }
    int viewLogOp = 0; // сколько из них просмотренно
    bool wasOr = false, wasAnd = false; // маркеры просмотренных операторов
    for (size_t i = 3; i < condArr.sizeM(); i += 4){
        string elem = condArr.get(i);
        string token = condArr.get(i - 3) + " " + condArr.get(i - 1);
        string nextToken = condArr.get(i + 1) + " " + condArr.get(i + 3);
        if (elem == "OR"){
            viewLogOp++;
            if (wasAnd == false){
                tokens.push(token);
            }else{
                replaceTok (tokens, token);
            }

            if (viewLogOp == countLogicOp){
                tokens.push(nextToken);
            }
            wasOr = true;
            wasAnd = false;
        }else if (elem == "AND"){
            viewLogOp++;
            if (wasOr == true){
                tokens.push(token);
            }else{
                if (tokens.sizeM() == 0){
                    tokens.push(token);
                }else{
                    replaceTok (tokens, token);
                }
            }

            if (viewLogOp == countLogicOp){
                replaceTok (tokens, nextToken);
            }
            wasOr = false;
            wasAnd = true;
        }
    }
}

string withoutApostr (string word){
    string result;
    for (size_t i = 1; i < word.size() - 1; i++){
        result += word[i];
    }
    return result;
}

bool checkRes (string result){
    stringstream ss (result);
    string temp;
    while (ss >> temp){
        if (temp == "false"){
            return false;
        }
    }
    return true;
}