#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <cctype>
#include <windows.h>
#include <chrono>

using namespace std;
using namespace chrono;

string findLongestPalindromeLikeWord(const string& sentence) {
    string longestWord = "";
    int n = sentence.length();
    int i = 0;

    while (i < n) { // Проходим по всей строке
        while (i < n && !isalnum(static_cast<unsigned char>(sentence[i]))) i++; // Проуск символа если он не буква и не цифра
        if (i >= n) break; // Если достигли конца строки — выходим

        int start = i;
        while (i < n && isalnum(static_cast<unsigned char>(sentence[i]))) i++;

        // Получаем первую и последнюю буквы слова и приводим их к нижнему регистру
        char first = tolower(static_cast<unsigned char>(sentence[start]));
        char last = tolower(static_cast<unsigned char>(sentence[i - 1]));

        if (first == last) { // Проверяем, совпадают ли первая и последняя буквы
            int len = i - start;
            if (len > (int)longestWord.length()) longestWord = sentence.substr(start, len);
        }
    }

    return longestWord;
}

vector<int> computePrefixFunction(const string& pattern) {
    int n = pattern.length();
    vector<int> pi(n, 0); // Массив префикс-функции
    for (int i = 1; i < n; i++) {
        int j = pi[i - 1];
        while (j > 0 && pattern[i] != pattern[j]) j = pi[j - 1]; // Пока j > 0 и символы не совпадают — "откатываемся" по префикс-функции
        if (pattern[i] == pattern[j]) j++; // Если символы совпали — увеличиваем длину префикса
        pi[i] = j;
    }
    return pi;
}

int kmpFindLastOccurrence(const string& text, const string& pattern) {
    if (pattern.empty()) return -1;
    if (text.empty()) return -1;

    vector<int> pi = computePrefixFunction(pattern); // Вычисление префикс-функции для образца
    int n = text.length();
    int m = pattern.length();
    int j = 0;
    int lastPos = -1;

    // Основной этап: проход по тексту
    for (int i = 0; i < n; i++) {
        while (j > 0 && text[i] != pattern[j]) j = pi[j - 1]; // При несовпадении — используем префикс-функцию для сдвига в образце
        if (text[i] == pattern[j]) j++; // Если символы совпали — продвигаемся вперёд по образцу
        if (j == m) { // Если нашли полное совпадение, то запоминаем его и продолжаем поиск
            lastPos = i - m + 1;
            j = pi[j - 1];
        }
    }
    return lastPos;
}

int time_decorator(string t, string p) {
    auto start_time = high_resolution_clock::now();

    int result = kmpFindLastOccurrence(t, p);

    auto end_time = high_resolution_clock::now();
    duration<double> duration = end_time - start_time;
    cout << duration.count() << " sec" << endl;
    return result;
}

int main() {
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);
    setlocale(LC_ALL, "Russian");

    string sentence;
    cout << "Введите предложение для 1 задания:\n";
    getline(cin, sentence);

    string result1 = findLongestPalindromeLikeWord(sentence);
    if (result1.empty()) {
        cout << "Подходящих слов не найдено." << endl;
    }
    else {
        cout << "Самое длинное слово с одинаковыми первой и последней буквами: \""
            << result1 << "\"" << endl;
    }

    string text, pattern;
    cout << "\nВведите текст для 2 задания:\n";
    getline(cin, text);

    cout << "Введите искомый образец:\n";
    getline(cin, pattern);

    int lastOccurrence = time_decorator(text, pattern);
    if (lastOccurrence != -1) {
        cout << "Последнее вхождение образца \"" << pattern
            << "\" в тексте начинается с индекса: " << lastOccurrence << endl;
    }
    else {
        cout << "Образец не найден." << endl;
    }

    return 0;
}
