#include <iostream>
#include <cstdlib>
#include <Windows.h>
#include <bitset>
#include <vector>
#include <ctime>
#include <set>
#include <unordered_set>
#include <fstream>
#include <sstream>
#include <random>
#include <chrono>

using namespace std;
using namespace chrono;

string gen_number() {
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<int> dist(0, 9);
    string result;
    for (int i = 0; i < 9; ++i) {
        result += to_string(dist(gen));
    }
    return result;
}

string gen_address() {
    const string streets[] = { "Lenina", "Gagarina", "Pushkina", "Mira", "Sovetskaya" };
    const string cities[] = { "Moscow", "SPb", "Novosibirsk", "Ekaterinburg", "Kazan" };
    static random_device rd;
    static mt19937 gen(rd());
    uniform_int_distribution<int> dist_num(1, 100);
    uniform_int_distribution<int> dist_street(0, 4);
    uniform_int_distribution<int> dist_city(0, 4);
    return "ul. " + streets[dist_street(gen)] + " " +
        to_string(dist_num(gen)) + ", " + cities[dist_city(gen)];
}

void create(int n) {
    vector<string> info;
    ofstream file("data.txt");
    for (int i = 0; i < n; i++) {
        string num = "89" + gen_number();
        string address = gen_address();
        info.push_back(num + " " + address);
    }
    sort(info.begin(), info.end());
    for (const auto& line : info) {
        file << line << endl;
    }
    file.close();
}

void createbin() {
    ifstream txtfile("data.txt");
    ofstream binfile("data.bin", ios::binary);
    string line;

    while (getline(txtfile, line)) {
        size_t length = line.size();
        binfile.write(reinterpret_cast<const char*>(&length), sizeof(size_t));
        binfile.write(line.c_str(), length);
    }

    txtfile.close();
    binfile.close();
}

void readbin() {
    ifstream binfile("data.bin", ios::binary);

    cout << "\nBinary file:" << endl;
    cout << "============================" << endl;

    while (binfile) {
        size_t length;
        binfile.read(reinterpret_cast<char*>(&length), sizeof(size_t));

        if (!binfile) break;

        string line(length, '\0');
        binfile.read(&line[0], length);

        cout << line << endl;
        cout << "---------------------------" << endl;
    }

    binfile.close();
}

void linear_search(const string& search_phone) {
    ifstream binfile("data.bin", ios::binary);
    size_t length;

    while (binfile.read(reinterpret_cast<char*>(&length), sizeof(size_t))) {
        string line(length, '\0');
        binfile.read(&line[0], length);

        if (line.substr(0, line.find(' ')) == search_phone) {
            cout << "FOUND: " << line << endl;
            binfile.close();
            return;
        }
    }

    cout << "NOT FOUND: " << search_phone << endl;
    binfile.close();
}

void time_decorator(int n, string num) {
    auto start_time = high_resolution_clock::now();

    linear_search(num);

    auto end_time = high_resolution_clock::now();
    duration<double> duration = end_time - start_time;
    cout << n << " strings ---> " << duration.count() << " sec" << endl;
}

struct IndexEntry {
    string key;
    streampos offset;
};

vector<IndexEntry> build_index() {
    ifstream binfile("data.bin", ios::binary);
    vector<IndexEntry> index;

    streampos current_offset = 0;
    size_t length;

    while (binfile.read(reinterpret_cast<char*>(&length), sizeof(size_t))) {
        string key(length, '\0');
        binfile.read(&key[0], length);

        // Извлекаем номер телефона (первое слово до пробела)
        size_t space_pos = key.find(' ');
        string phone = key.substr(0, space_pos);

        index.push_back({ phone, current_offset });

        // Обновляем смещение для следующей записи
        current_offset = binfile.tellg();
    }

    binfile.close();
    return index;
}

streampos binary_search_index(const vector<IndexEntry>& index, const string& search_key) {
    int left = 0;
    int right = index.size() - 1;

    while (left <= right) {
        int mid = left + (right - left) / 2;

        if (index[mid].key == search_key) {
            return index[mid].offset;
        }
        else if (index[mid].key < search_key) {
            left = mid + 1;
        }
        else {
            right = mid - 1;
        }
    }

    return -1;
}

string read_info(streampos offset) {
    ifstream binfile("data.bin", ios::binary);
    binfile.seekg(offset);

    size_t length;
    binfile.read(reinterpret_cast<char*>(&length), sizeof(size_t));

    string record(length, '\0');
    binfile.read(&record[0], length);

    binfile.close();
    return record;
}

int main() {
    int n;
    cin >> n;
    create(n);
    createbin();
    readbin();
    string num;
    cin >> num;

    cout << "Choose search algorithm (1-linear, 2-binary): ";
    int choice;
    cin >> choice;
    if (choice == 1) time_decorator(n, num);
    else {
        auto start_time = high_resolution_clock::now();
        vector<IndexEntry> index = build_index();
        streampos offset = binary_search_index(index, num);
        if (offset != -1) {
            string result = read_info(offset);
            cout << "FOUND: " << result << endl;
        }
        else {
            cout << "NOT FOUND: " << num << endl;
        }
        auto end_time = high_resolution_clock::now();
        duration<double> duration = end_time - start_time;
        cout << n << " strings ---> " << duration.count() << " sec" << endl;
    }

    return 0;
}
