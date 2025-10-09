#include <iostream>
#include <vector>
#include <string>
#include <cctype>
#include <windows.h>

using namespace std;

class Student {
public:
    string recordBook;
    string group;
    string fio;

    Student() = default;


    //конструктор для добавления в объект сразу всех полей класса
    Student(const std::string& rb, const std::string& group, const std::string& fio) {
        this->recordBook = rb;
        this->group = group;
        this->fio = fio;
    }
};

const string DELETED_MARKER = "__DELETED__";

class HashTable {
private:
    vector<Student> table; //массив, где хранятся наши данные
    size_t size; //текущий размер
    size_t capacity; //максимальная вместимость

    //Основная хэш-функция
    size_t hash1(const std::string& key) const {
        const size_t p = 31;
        size_t hash = 0;
        for (char c : key) {
            hash = (hash * p + c) % capacity;
        }
        return hash;
    }

    //Добавочная хэш-функция
    size_t hash2(const std::string& key) const {
        const size_t p = 37; //2 простых числа 37 и 31 чтобы не было линейной зависимости
        size_t hash = 0;
        for (char c : key) {
            hash = (hash * p + c) % (capacity - 1);
        }
        return 1 + hash; //возвращаем хэш + 1 для того чтобы было возможно пробирование
    }

    // Поиск
    int findIndex(const std::string& key) const {
        size_t h1 = hash1(key); 
        size_t h2 = hash2(key);
        //Пробирование
        for (size_t i = 0; i < capacity; ++i) {
            size_t idx = (h1 + i * h2) % capacity; // Вычисление индекса элемента
            if (table[idx].recordBook == key) {
                return static_cast<int>(idx);
            }
            if (table[idx].recordBook.empty()) {
                break;
            }
        }
        return -1;
    }

    // Рехеширование при увеличении размера
    void rehash() {
        vector<Student> oldTable = table;
        size_t oldCap = capacity;

        capacity *= 2;
        table.assign(capacity, Student{});
        size = 0;

        //Пересчёт хэша для всех элементов с учетом увеличенного размера таблицы
        for (size_t i = 0; i < oldCap; ++i) {
            if (!oldTable[i].recordBook.empty() && oldTable[i].recordBook != DELETED_MARKER) {
                insert(oldTable[i].recordBook, oldTable[i].group, oldTable[i].fio);
            }
        }
    }

public:
    HashTable(size_t initialCapacity = 11) : capacity(initialCapacity), size(0) {
        table.resize(capacity, Student{});
    }

    // Вставка
    bool insert(const std::string& recordBook, const std::string& group, const std::string& fio) {
        //Рехэширование при большой заполненности таблицы
        if (static_cast<double>(size+1) / capacity >= 0.75) {
            rehash();
        }

        size_t h1 = hash1(recordBook);
        size_t idx = (h1) % capacity;
        if (table[idx].recordBook.empty() || table[idx].recordBook == DELETED_MARKER) {
            table[idx] = Student{ recordBook, group, fio };
            ++size; //увеличение длины таблицы при удачной вставке
            return true;
        }
        size_t h2 = hash2(recordBook);

        //Поиск свободной ячейки с помощью пробирования
        for (size_t i = 1; i < capacity; ++i) {
            size_t idx = (h1 + i * h2) % capacity;
            if (table[idx].recordBook.empty() || table[idx].recordBook == DELETED_MARKER) {
                table[idx] = Student{recordBook, group, fio};
                ++size; //увеличение длины таблицы при удачной вставке
                cout << "Коллизия " << "изначальное место " << (h1) % capacity << endl;
                return true;
            }
            if (table[idx].recordBook == recordBook) {
                return false; //возвращаем false если такой ключ уже используется
            }
        }
        return false; //возвращаем false если не нашли свободного места
    }

    // Поиск элемента по ключу
    Student* search(const std::string& recordBook) {
        int idx = findIndex(recordBook); //вызов функции для поиска элемента по ключу
        if (idx == -1) return nullptr;
        return &table[idx];
    }

    // Удаление
    bool remove(const std::string& recordBook) {
        int idx = findIndex(recordBook); //вызов функции для поиска элемента по ключу
        if (idx == -1) return false;
        table[idx].recordBook = DELETED_MARKER; //помечаем ячейку как удаленную что бы не сломалось пробирование при текущей длине
        --size; //уменьшаем размер таблицы
        return true;
    }

    // Вывод всей таблицы, информации по вместимости и длине массива
    void print() const {
        for (size_t i = 0; i < capacity; ++i) {
            if (table[i].recordBook.empty()) {
                cout << "[" << i << "] <пусто>\n";
            } else if (table[i].recordBook == DELETED_MARKER) {
                cout << "[" << i << "] <удалено>\n";
            } else {
                cout << "[" << i << "] " << table[i].recordBook << " | " << table[i].group << " | " << table[i].fio << endl;
            }
        }
        std::cout << "Заполнено: " << size << " / " << capacity << "\n\n";
    }

    // Вывод всех существующих элементов таблицы
    void show() const {
        for (size_t i = 0; i < capacity; ++i) {
            if (!table[i].recordBook.empty() && table[i].recordBook != DELETED_MARKER) {
                cout << table[i].recordBook << " | " << table[i].group << " | " << table[i].fio << endl;
            }
        }
    }
};

// Текстовый интерфейс
int main() {
    SetConsoleCP(1251);
    setlocale(LC_ALL, "rus");

    //создание Хэш Таблицы с вместимостью 7
    HashTable ht(7);

    // Автозаполнение 5–ю записями
    ht.insert("1", "ИКБО-51-24", "Астанин Георгий Константинович");
    ht.insert("2", "ИКБО-02-23", "Петров Михаил Иванович");
    ht.insert("3", "ИКБО-03-25", "Сидоров Иван Юрьевич");
    ht.insert("4", "ИКБО-04-22", "Белов Степан Васильевич");
    //ht.insert("5", "ИКБО-05-24", "Смирнова Анна Владимировна");


    string cmd;
    while (true) {
        cout << "\nКоманды: add, get, remove, print, show, exit\n> ";
        cin >> cmd;

        if (cmd == "exit") break;

        if (cmd == "print") {
            ht.print();
        }
        else if (cmd == "show") {
            ht.show();
        }
        else if (cmd == "add") {
            string rb, gr, fio;
            cout << "Номер зач. книжки: "; std::cin >> rb;
            cout << "Группа: "; std::cin >> gr;
            cout << "ФИО: "; std::cin.ignore(); std::getline(std::cin, fio);
            if (ht.insert(rb, gr, fio)) {
                cout << "Запись добавлена.\n";
            } else {
                cout << "Ошибка: запись с таким ключом уже существует или таблица переполнена.\n";
            }
        }
        else if (cmd == "get") {
            string rb;
            cout << "Номер зач. книжки: "; std::cin >> rb;
            Student* s = ht.search(rb);
            if (s) {
                cout << "Найдено: " << s->recordBook << " | " << s->group << " | " << s->fio << "\n";
            } else {
                cout << "Запись не найдена.\n";
            }
        }
        else if (cmd == "remove") {
            string rb;
            cout << "Номер зач. книжки: ";
            cin >> rb;
            if (ht.remove(rb)) {
                cout << "Запись удалена.\n";
            } else {
                cout << "Запись не найдена.\n";
            }
        }
        else {
            cout << "Неизвестная команда.\n";
        }
    }

    return 0;
}
