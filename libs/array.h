#ifndef MASSIVE_H_INCLUDED
#define MASSIVE_H_INCLUDED

#include <iostream>
#include <string>

using namespace std;

struct StrArray {
    string* data;   
    size_t size = 0; // размер
    size_t capacity; // ёмкость

    StrArray(size_t initialCapacity = 5) {
        size = 0;
        capacity = initialCapacity; // Устанавливаем начальную емкость
        data = new string[capacity]; // Выделяем память под массив
    }

    // Деструктор
    ~StrArray() {
        delete[] data;
    }

    void resize() {
        size_t newCapacity = capacity * 2;
        string* newData = new string[newCapacity]; // новый массив
        for (size_t i = 0; i < size; i++) {
            newData[i] = data[i];
        }
        delete[] data; 
        data = newData; //указатель на новый массив
        capacity = newCapacity;
    }

    void push(const string& value) {
        if (size >= capacity) {
            resize();
        }
        data[size++] = value; // заносим строку и увеличиваем размер
    }

    size_t sizeM() const {
        return size; // Возвращаем текущий размер
    }

    string get(size_t index) {
        if (index < 0 || index >= size) {
            cout << "ERROR: Index out of range." << endl;
            return "";
        }
        return data[index];
    }

    void replace(size_t index, const string& value) {
        if (index < 0 || index >= size) {
            cout << "ERROR: Index out of range." << endl;
            return;
        }
        data[index] = value;
    }

    void del(size_t index) {
        if (index < 0 || index >= size) {
            cout << "ERROR: Index out of range." << endl;
            return;
        }
        for (size_t i = index; i < size - 1; ++i) {
            data[i] = data[i + 1];
        }
        --size;
    }

    void print() const {
        for (size_t i = 0; i < size; ++i) {
            cout << data[i] << " ";
        }
        cout << endl;
    }
};

#endif // MASSIVE_H_INCLUDED