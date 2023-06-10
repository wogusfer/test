#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <utility>
#include <stdexcept>
#include <sstream>
using namespace std;

vector<vector<string>> read_csv(string filename) { // функция чтения csv-файла и записи в двумерный вектор

    vector<vector<string>> result;

    ifstream myFile(filename);

    if (!myFile.is_open()) throw runtime_error("Could not open file");

    string line;
    while (getline(myFile, line)) { // построчное считывание
        stringstream ss(line);
        vector<string> row;
        string cell;
        while (getline(ss, cell, ',')) {
            row.push_back(cell);
        }
        result.push_back(row);
    }

    myFile.close();

    return result;
}

string calcCellValue(const vector<vector<string>>& table, const string& expr) // функция вычисления значений ячеек
{
    if (expr.empty() || expr[0] != '=') {
        return expr; // проверка на наличие =, если нет значит обычное число или пустое множество
    }

    size_t opPos = string::npos;
    string arg1, arg2, op; // разделяем значение после = на отдельные составляющие
    for (size_t i = 1; i < expr.size(); ++i) {
        if (expr[i] == '+' || expr[i] == '-' || expr[i] == '*' || expr[i] == '/') {
            opPos = i; // определяется позиция арифметического знака
            op = expr.substr(opPos, 1); // после отдельные аргументы
            arg1 = expr.substr(1, opPos - 1); // до и после знака
            arg2 = expr.substr(opPos + 1);
            break;
        }
    }

    if (opPos == string::npos) {
        return "=ERROR";
    }

    string val1, val2; // инициализация аргументов

    if (arg1[0] >= 'A' && arg1[0] <= 'Z') {
        string arg11, arg12; // отдельные переменные для буквенного и численного значений аргумента

        for (int i = 0; i < arg1.size(); i++) {
            if (isdigit(arg1[i])) arg12.push_back(arg1[i]); // если число то в одну переменную
            else arg11.push_back(arg1[i]); // иначе в другую
        }

        int i = 0, j = 0, col = 0, row = 0;
        while (arg11 != table[0][i]) { // определяем номер колонны
            i++;
        }
        col = i;
        
        while (arg12 != table[j][0]) { // и строки соответственно
            j++;
        }
        row = j;

        if (row >= table.size() || col >= table[0].size()) { // если значения вышли за пределы таблицы
            return "=ERROR"; // то ошибка
        }
        val1 = calcCellValue(table, table[row][col]);
    }
    else {
        val1 = arg1; 
    }

    if (arg2[0] >= 'A' && arg2[0] <= 'Z') {

        string arg21, arg22;

        for (int i = 0; i < arg2.size(); i++) {
            if (isdigit(arg2[i])) arg22.push_back(arg2[i]);
            else arg21.push_back(arg2[i]);
        }

        int i = 0, j = 0, col = 0, row = 0;
        while (arg21 != table[0][i]) {
            i++;
        }
        col = i;

        while (arg22 != table[j][0]) {
            j++;
        }
        row = j;

        val2 = calcCellValue(table, table[row][col]);

        if (row >= table.size() || col >= table[0].size()) {
            return "=ERROR";
        }
    }
    else {
        val2 = arg2;
    }

    try { // вычисление 
        int intVal1 = stoi(val1);
        int intVal2 = stoi(val2);
        int result;
        if (op == "+") {
            result = intVal1 + intVal2;
        }
        else if (op == "-") {
            result = intVal1 - intVal2;
        }
        else if (op == "*") {
            result = intVal1 * intVal2;
        }
        else if (op == "/") {
            if (intVal2 == 0) {
                return "=ERROR";
            }
            result = intVal1 / intVal2;
        }
        else {
            return "=ERROR";
        }
        return to_string(result);
    }
    catch (...) {
        return "=ERROR";
    }
}

int main(int argc, char* argv[]) {

    vector<vector<string>> table = read_csv(argv[1]);

    int row = table.size();
    int col = table[0].size();

    for (size_t i = 0; i < row; ++i) {
        for (size_t j = 0; j < col; ++j) {
            table[i][j] = calcCellValue(table, table[i][j]);
        }
    }

    for (const auto& row : table) {
        for (const auto& cell : row) {
            cout << cell << ",";
        }
        if (!row.empty()) {
            cout << "\n";
        }
    }

    return 0;
}