#include "libs/select.h"

//SELECT engineer.modelWork, passengers.modelAirplane, airplane.model FROM engineer, passengers, airplane
//SELECT engineer.modelWork, passengers.modelAirplane FROM engineer, passengers
//SELECT engineer.modelWork FROM engineer
//SELECT engineer.modelWork, passengers.modelAirplane FROM engineer, passengers WHERE engineer.modelWork = 'boing_707' OR engineer.modelWork = passengers.modelAirplane

void selectCom (string userCommand, string baseName){
    stringstream ss(userCommand);
    string temp;
    ss >> temp; // SELECT
    
    StrArray outputCol;
    bool wasFrom = false;
    while (ss >> temp){ // значения после SELECT
        if (temp == "FROM"){
            wasFrom = true;
            break;
        }

        if (!checkOutputCol(temp, baseName)){
            cerr << "ERROR_13: Unknown condition." << endl;
            return;
        }
        outputCol.push(temp);
    }

    if (!wasFrom){ // не встретился FROM
        cerr << "ERROR_14: Unknown condition." << endl;
        return;
    }

    StrArray fromTable;
    bool wasWhere = false;
    while (ss >> temp){ // значения после FROM
        if (temp == "WHERE"){
            wasWhere = true;
            break;
        }
        removeComma(temp);
        fromTable.push(temp);
    }

    if (!checkFrom(fromTable, outputCol)){
        cerr << "ERROR_15: Unknown condition." << endl;
        return;
    }

    StrArray condArr;
    if (wasWhere){
        while (ss >> temp){
            condArr.push(temp);
        }

        if (!correctCond(condArr, fromTable, "SELECT")){
            cerr << "ERROR_16: Unknown condition." << endl;
            return;
        }
    }

    if (isLockTables(fromTable, baseName)){ //Проверка блокировки таблицы
        cout << "The table is currently locked for use, try again later." << endl;
        return;

    }else lockTables(fromTable, baseName, "1"); // блокируем на время работы

    StrArray tokens;
    if (wasWhere){
        toTokens(condArr, tokens);
    }

    StrArray outputTables;
    massWithoutCol(outputCol, outputTables);

    string firstLine = getColFromJson(outputTables);
    
    ofstream tempFile(baseName + "/temp.csv");
    tempFile << firstLine; //заносим названия таблиц.колонок
    tempFile.close();

    int nowTable = 0;
    string tempStr;
    crossJoin(baseName, outputTables, nowTable, tempStr);

    string result = getResStr(baseName, outputCol, tokens);
    
    //fs::remove(baseName + "/temp.csv");
    correctOutput(result, outputCol.sizeM());

    lockTables(fromTable, baseName, "0");
}

void lockTables(StrArray& tables, string baseName, string command){
    for (size_t i = 0; i < tables.sizeM(); i++){
        string tableName = tables.get(i);
        string tablePath = baseName + "/" + tableName;
        string path = tablePath + "/" + tableName + "_lock";
        ofstream file(path);
        file << command;
        file.close();
    }
}

bool isLockTables(StrArray& tables, string baseName){
    for (size_t i = 0; i < tables.sizeM(); i++){
        string tableName = tables.get(i);
        string tablePath = baseName + "/" + tableName;
        string path = tablePath + "/" + tableName + "_lock";

        ifstream file (path);

        int value;
        file >> value;
        file.close();

        if(value == 1){
            return true;
        }
    }
    return false;
}

string getValueFromLine(string line, int columnNum){
    string value;
    int checkedPos = 0;
    stringstream values(line);
    while (getline(values, value, ';')){
        if (checkedPos == 0){
            checkedPos++;
            continue;
        }
        checkedPos++;
        if (checkedPos == columnNum){
            break;
        }
    }
    return value;
}

void massWithoutCol (StrArray& outputCol, StrArray& outputTables){
    for (size_t i = 0; i < outputCol.sizeM(); i++){
        string elem = outputCol.get(i);
        stringstream values(elem);
        string tableName;
        getline(values, tableName, '.');
        outputTables.push(tableName);
    }
}

bool checkFrom (StrArray& fromTable, StrArray& outputCol){
    for (size_t i = 0; i < fromTable.sizeM(); i++){
        string table = fromTable.get(i);
        for (size_t i = 0; i < outputCol.sizeM(); i++){
            string elem = outputCol.get(i);
            stringstream ss(elem);
            getline(ss,elem,'.');
            if (elem == table){
                break;
            }else if (i == outputCol.sizeM() - 1){ // если так и не нашел значений
                return false;
            }
        }
    }
    return true;
}

bool checkOutputCol(string& tableCol, string baseName){
    removeComma(tableCol); // удаляем запятую, если она есть в конце

    string table, column;
    getTablCol(tableCol, table, column);

    string tablePath = baseName + "/" + table;
    if (!checkTable(tablePath)){
        return false;
    }

    if(!columnExist(table, column)){
        return false;
    }

    return true;
}

void removeComma(string& value){
    if (value[value.size() - 1] == ','){
        string result;
        for (size_t i = 0; i < value.size() - 1; i++){
            result += value[i];
        }
        value = result;
    }
}

void correctOutput(string result, int wordsPerRow){
    int columnWidth = 25;

    istringstream stream(result);
    string word;
    int count = 0; // Счетчик слов в текущей строке

    while (stream >> word) {
        cout << word;
        int spacesToAdd = columnWidth - word.length();
        for (int i = 0; i < spacesToAdd; ++i) {
            cout << " ";
        }

        count++;

        // Переход на новую строку после вывода нужного количества слов
        if (count == wordsPerRow) {
            cout << endl;
            count = 0;
        }
    }

    // Если остались слова, завершить строку
    if (count > 0) {
        cout << endl;
    }
}

string getValues(StrArray& outputCol, string line, string allCol){
    string result;
    for (size_t i = 0; i < outputCol.sizeM(); i++){
        string column = outputCol.get(i);

        int pos = 0;
        stringstream ss(allCol);
        string elem;
        while (getline(ss, elem, ';')){
            pos++;
            if (elem == column){
                break;
            }
        }

        stringstream values(line);
        string value;
        int valuesPos = 0;

        while (getline(values, value, ';')){
            valuesPos++;
            if (valuesPos == pos){
                result += value + " ";
                break;
            }
        } 
    }
    return result;
}

int getPosVal(string temp, string allCol){
    int pos = 0;
    stringstream elems(allCol);
    string elem;

    while(getline(elems, elem, ';')){
        pos ++;
        if (temp == elem){
            break;
        }
    }

    return pos;
}

bool trueToken(string line, int valPos, string allCol, string temp){
    if (temp[0] == '\''){ // = 'abs'
        string condVal = withoutApostr(temp);

        int counter = 0;
        stringstream elems(line);
        string elem;
        while (getline(elems, elem, ';')){
            counter ++;
            if (counter == valPos && condVal == elem){
                return true;
            }
        }

    }else{ // = tab1.col2
        int posRightVal = getPosVal(temp, allCol);
        int counter = 0;
        stringstream elems(line);
        string elem, valLeft, valRight;
        while (getline(elems, elem, ';')){
            counter ++;
            if (counter == valPos){
                valLeft = elem;
            }
            if (counter == posRightVal){
                valRight = elem;
            }
        }

        if (valLeft == valRight){
            return true;
        }
    }

    return false;
}

bool conditions(StrArray& tokens, string allCol, string line){
    for (size_t i = 0; i < tokens.sizeM(); i++){ // проходимся по токенам
        string token = tokens.get(i);
        stringstream ss(token);
        string temp, result = "";
        int countArg = 0, valPos;

        while (ss >> temp){
            if (countArg == 0){
                countArg++;
                valPos = getPosVal(temp, allCol);

            }else{
                countArg = 0;
                if (trueToken(line, valPos, allCol, temp)){
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

string getResStr(string baseName, StrArray& outputCol, StrArray& tokens){
    ifstream tempFile(baseName + "/temp.csv");
    string line, values;

    getline(tempFile, line); // записывем все доступные колонки
    string allCol = line;
    values += getValues(outputCol, line, allCol) + '\n';

    while (getline(tempFile, line)){
        if (tokens.sizeM() == 0){
            values += getValues(outputCol, line, allCol) + '\n';
        }else{
            if (conditions(tokens, allCol, line)){
                values += getValues(outputCol, line, allCol) + '\n';
            }
        }
    }

    return values;
}

string getColFromJson(StrArray& outputTables){
    ifstream file("files/schema.json");
    json config;
    file >> config;
    file.close();
    string result;
    for (size_t i = 0; i < outputTables.sizeM(); i++){
        string table = outputTables.get(i);
        if (config["structure"].contains(table)) {
            for (const auto& column : config["structure"][table]) {
                string colName = column;
                result += table + "." + colName + ";";
            }
        }
    }

    return result;

}

void withoutTablePk (string& line){
    stringstream ss(line);
    string elem, result;
    int count = 0;
    while(getline(ss, elem,';')){
        if (count == 0){
            count++;
            continue;
        }
        result += elem + ";";
    }

    line = result;
}

void addToTemp(string baseName, string tempStr){
    ofstream tempFile(baseName + "/temp.csv", ios::app);
    tempFile << endl << tempStr;
    tempFile.close();
}

int tempStrSize(string tempStr){
    stringstream words(tempStr);
    string word;
    int counter = 0;

    while (getline(words, word, ';')){
        counter++;
    }

    return counter;
}

void clearTemp(string& tempStr, int wasWords){
    if (wasWords == 0) {
        tempStr = "";
        return;
    }

    stringstream words(tempStr);
    string word, result;
    int count = 0;
    while (getline(words, word, ';')) {
        if (count == wasWords) {
            break;
        } else {
            result += word + ";";
        }
        count++;
    }

    tempStr = result;
}

void crossJoin(string baseName, StrArray& outputTables, int nowTable, string tempStr){
    string tableName;
    for (size_t i = 0; i < outputTables.sizeM(); i++){
        if (i == nowTable){
            tableName = outputTables.get(i);
            nowTable++;
            break;
        }
    }
    
    string tablePath = baseName + "/" + tableName;
    int countCsv = 1;
    while (true){
        string csvPath = tablePath + "/" + to_string(countCsv) + ".csv";

        if (!csvAvail(csvPath)) break;

        ifstream csv(csvPath);
        string line;

        getline(csv, line); // пропускаем названия колонок
        while(getline(csv, line)){

            withoutTablePk (line);

            if (nowTable != outputTables.sizeM()){ // если рассматривается не последняя таблица
                int wasWords = tempStrSize(tempStr);
                tempStr += line;

                crossJoin(baseName, outputTables, nowTable, tempStr);
                clearTemp(tempStr, wasWords); // убираем послденее добавленное значение

            }else{// в случае рассмотрения последней таблицы
                addToTemp(baseName, tempStr + line);
            }
        }

        countCsv++;
    }
}