#include "libs/insert.h"

void insertCom(string userCommand, string baseName){
    string tableName, values;
    if (!checkSyntax(tableName, values, baseName, userCommand, "INS")){
        return;
    }

    string tablePath = baseName + "/" + tableName;
    if (!checkTable(tablePath)){
        cerr << "ERROR_5: Unknown table name." << endl;
        return;
    }

    stringstream ss (values);
    string temp;
    StrArray valuesToCol;
    while (ss >> temp){
        valuesToCol.push(temp);
    }

    int countCols = countCol(tableName); // кол-во столбцов в таблице

    if (countCols != valuesToCol.sizeM()){
        cerr << "ERROR_6: Incorrect count of values." << endl;
        return;
    }

    if (checkValues(valuesToCol) == false){ //проверка синтаксиса вводимых значений
        cerr << "ERROR_7: Incorrect syntax values." << endl;
        return;
    }

    if (isLock(tablePath, tableName)){ //Проверка блокировки таблицы
        cout << "The table is currently locked for use, try again later." << endl;
        return;
    }else{
        lockTable(tablePath, tableName); // блокируем на время работы
    }

    string pkFile = tablePath + "/" + tableName + "_pk";
    ifstream filePk(pkFile);
    if (!filePk.is_open()){
        cerr << "ERROR_10: Unable to open file: " << pkFile << endl;
        unlockTable(tablePath, tableName);
        return;
    }
    int pkVal;
    filePk >> pkVal;
    filePk.close();

    string dataToTable; // строка к вставке
    dataToTable = to_string(pkVal) + ";";
    for (size_t i = 0; i < valuesToCol.sizeM(); i++){
        string elem = valuesToCol.get(i);
        dataToTable += elem + ";";
    }

    string csv; // выбор csv файла в который будем записывать строку
    checkCsv (tablePath, tableName, csv);

    ofstream fileCsv(tablePath + "/" + csv, ios::app);
    fileCsv << endl << dataToTable;
    fileCsv.close();

    ofstream filePkEnd(pkFile);
    filePkEnd << pkVal + 1;

    unlockTable(tablePath, tableName);
}

int countCol(string tableName){
    ifstream file("files/schema.json");
    json jsonData;
    file >> jsonData;
    file.close();

    return jsonData["structure"][tableName].size();
}

bool checkValues (StrArray& valuesToCol){
    for (size_t i = 0; i < valuesToCol.sizeM(); i++){
        string temp;
        string correctVal; // то, куда будем записывать отредактированное значение
        if (i == 0){
            temp = valuesToCol.get(i);
            if (temp.size() < 5){
                return false;
            }
            for (size_t j = 0; j < temp.size(); j++){
                if (j == 0){
                    if (temp[j] != '('){
                        return false;
                    }else{
                        continue;
                    }
                }

                if (j == 1 || j + 2 == temp.size()){
                    if (temp[j] != '\''){
                        return false;
                    }else{
                        continue;
                    }
                }

                if (j + 1 == temp.size()){
                    if (temp[j] != ','){
                        return false;
                    }else{
                        continue;
                    }
                }
                correctVal += temp[j];
            }
        }else if (i + 1 == valuesToCol.sizeM()){
            temp = valuesToCol.get(i);
            if (temp.size() < 4){
                return false;
            }
            for (size_t j = 0; j < temp.size(); j++){
                if (j == 0 || j + 2 == temp.size()){
                    if (temp[j] != '\''){
                        return false;
                    }else{
                        continue;
                    }
                }
                if (j + 1 == temp.size()){
                    if (temp[j] != ')'){
                        return false;
                    }else{
                        continue;
                    }
                }
                correctVal += temp[j];
            }
        }else{
            temp = valuesToCol.get(i);
            if (temp.size() < 4){
                return false;
            }
            for (size_t j = 0; j < temp.size(); j++){
                if (j == 0 || j + 2 == temp.size()){
                    if (temp[j] != '\''){
                        return false;
                    }else{
                        continue;
                    }
                }

                if (j + 1 == temp.size()){
                    if (temp[j] != ','){
                        return false;
                    }else{
                        continue;
                    }
                }
                correctVal += temp[j];
            }
        }

        valuesToCol.replace(i, correctVal);
    }
    return true;
}

bool fileAvail (string nextCsv, string tablePath){ // Проверка существования CSV
    ifstream nextFileCsv(tablePath + "/" + nextCsv);
    if (nextFileCsv.is_open()) {
        return true;
    }else{ // если не существует - создаём
        ifstream firstCsv (tablePath + "/1.csv");
        string lineFirstCsv;
        getline(firstCsv, lineFirstCsv);
        firstCsv.close();

        ofstream csv(tablePath + "/" + nextCsv);
        csv << lineFirstCsv;

        return false;
    }
}

void checkCsv (string tablePath, string tableName, string& csv){
    int turplLim = getTurplLim();
    int countCsv = 1;
    while (true){
        ifstream fileCsv(tablePath + "/" + to_string(countCsv) + ".csv");

        int lineCount = -1;
        string line;
        while (getline(fileCsv, line)){
            lineCount++;
        }
        fileCsv.close();

        if (lineCount < turplLim){
            csv = to_string(countCsv) + ".csv";
            return;
        }

        if (fileAvail(to_string(countCsv + 1) + ".csv", tablePath)){
            countCsv++;
        }else{
            countCsv++;
            csv = to_string(countCsv) + ".csv";
            return;
        }
    }
}