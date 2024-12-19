#include "libs/delete.h"

// DELETE FROM airplane WHERE airplane.model = 'boing_707'
// DELETE FROM airplane WHERE airplane.model = 'boing_707' OR airplane.model = engineer.name

string delCom(string userCommand, string baseName){
    string tableName, conditions, message = "";
    if (!checkSyntax(tableName, conditions, baseName, userCommand, "DEL", message)){
        return message;
    }

    string tablePath = baseName + "/" + tableName;
    if (!checkTable(tablePath)){
        message =  "ERROR_11: Unknown table name.";
        return message;
    }

    stringstream ss (conditions);
    string temp;
    StrArray condArr;
    while (ss >> temp){
        condArr.push(temp);
    }

    StrArray table;
    table.push(tableName);
    if (!correctCond(condArr, table, "DELETE")){
        message =  "ERROR_12: Unknown condition.";
        return message;
    }

    if (isLock(tablePath, tableName, message)){ //Проверка блокировки таблицы
        message += "The table is currently locked for use, try again later.";
        return message;
    }else{
        lockTable(tablePath, tableName); // блокируем на время работы
    }

    StrArray tokens;
    toTokens(condArr, tokens);

    int turplLim = getTurplLim(), countCsv = 1, countTemp = 1;
    int countAddStr = 0; // кол-во добавленных строк в temp файле
    bool stop = false;
    while (stop != true){
        if (stop == true){
            break;
        }
        string csv = tablePath + "/" + to_string(countCsv) + ".csv";
        if (!csvAvail(csv)){
            break;
        }
        ifstream fileCsv(csv);
        string line, firstLine;
        int lineCount = -1;
        while (getline(fileCsv,line)){
            if (lineCount == -1){
                firstLine = line;
                if(fileCsv.eof()){ // если csv пустой
                    ofstream file (tablePath + "/1temp.csv");
                    file << firstLine;
                    file.close();
                    stop = true;
                }
                lineCount++;
                continue;
            }

            if(!checkCond(tokens, line)){ // запись в новый temp файл, т.к. старый заполнился
                if (countAddStr == turplLim){
                    countAddStr = 0;
                    countTemp++;
                }
                string tempFile = tablePath + "/" + to_string(countTemp) + "temp.csv";
                addToTemp(line, tempFile, firstLine);
                countAddStr ++;
            }
            
            lineCount++;
            if (lineCount == turplLim){
                countCsv++;
            }else if(fileCsv.eof()){ // на последней строке файла
                stop = true;
            }
        }
        fileCsv.close();
        if (countAddStr == 0){
            ofstream file (tablePath + "/1temp.csv");
            file << firstLine;
            file.close();
        }
    }

    int delCsv = 1;
    while (delCsv <= countCsv){ // удаление старых csv
        fs::remove(tablePath + "/" + to_string(delCsv) + ".csv");
        delCsv++;
    }

    int renameTemp = 1;
    while (renameTemp <= countTemp){ // переименовывание temp файлов
        fs::rename(tablePath + "/" + to_string(renameTemp) + "temp.csv", tablePath + "/" + to_string(renameTemp) + ".csv");
        renameTemp++;
    }

    unlockTable(tablePath, tableName);
    
    if (message == ""){
        return "DELETE successful.";
    }

    return message;
}

bool checkVals (string line, string condVal, int valuePos){
    int checkedPos = 0;
    stringstream ss (line);
    string value;
    while(getline(ss, value, ';')){
        if (checkedPos == 0){
            checkedPos++;
            continue;
        }
        checkedPos++;
        if (checkedPos == valuePos){
            if(value == condVal){
                return true;
            }
        }
    }
    return false;
}

void addToTemp (string line, string tempPath, string firstLine){
    if (!fs::exists(tempPath)){
        ofstream file (tempPath);
        file << firstLine << endl << line;
        file.close();
        return;
    }
    ofstream file (tempPath, ios::app);
    file << endl << line;
    file.close();
}

bool checkCond (StrArray& tokens, string line){
    for (size_t i = 0; i < tokens.sizeM(); i++){ // проходимся по токенам
        string token = tokens.get(i);
        stringstream ss(token);
        int countArg = 0, valuePos;
        string temp, table, column, condVal;
        string result = "";
        while (ss >> temp){
            if (countArg == 0){
                countArg ++;
                getTablCol(temp, table, column); // отдельно название таблицы и стобец
                valuePos = colIndex(table, column); // позиция значения в строке csv

            }else{
                countArg = 0;
                condVal = withoutApostr(temp);
                if (checkVals (line, condVal, valuePos)){
                    result += " true";
                }else{
                    result += " false";
                }
            }
        }
        
        if (checkRes(result)){
            return true;
        }
    }
    return false;
}