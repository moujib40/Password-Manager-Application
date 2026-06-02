#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <string>

using namespace std;

struct Entry {
    string title;
    string encPassword;
    string url;
    string notes;
};

const string DATA_FILE = "passwords.csv";
const string ENC_FILE  = "passwords.enc";

// simple XOR "encryption" (replace with AES for real assignment)
string xorCrypt(const string &s) {
    string r = s;
    char key = 0x5A;
    for (char &c : r) c ^= key;
    return r;
}

void encryptFileOnExit() {
    ifstream fin(DATA_FILE, ios::binary);
    if (!fin.good()) return;
    string plain((istreambuf_iterator<char>(fin)), {});
    fin.close();
    string enc = xorCrypt(plain);
    ofstream fout(ENC_FILE, ios::binary);
    fout << enc;
    fout.close();
    remove(DATA_FILE.c_str());
}

void decryptFileIfExists() {
    ifstream fin(ENC_FILE, ios::binary);
    if (!fin.good()) return;
    string enc((istreambuf_iterator<char>(fin)), {});
    fin.close();
    string plain = xorCrypt(enc);
    ofstream fout(DATA_FILE, ios::binary);
    fout << plain;
    fout.close();
}

vector<Entry> loadEntries() {
    vector<Entry> v;
    ifstream f(DATA_FILE);
    if (!f.good()) return v;
    string line;
    while (getline(f, line)) {
        if (line.empty()) continue;
        stringstream ss(line);
        Entry e;
        getline(ss, e.title, ';');
        getline(ss, e.encPassword, ';');
        getline(ss, e.url, ';');
        getline(ss, e.notes, ';');
        v.push_back(e);
    }
    return v;
}

void saveEntries(const vector<Entry> &v) {
    ofstream f(DATA_FILE);
    for (auto &e : v) {
        f << e.title << ";" << e.encPassword << ";" << e.url << ";" << e.notes << "\n";
    }
}

int findByTitle(const vector<Entry> &v, const string &t) {
    for (int i = 0; i < (int)v.size(); ++i)
        if (v[i].title == t) return i;
    return -1;
}

void addEntry(vector<Entry> &v) {
    Entry e;
    cout << "Title: "; getline(cin, e.title);
    string pwd;
    cout << "Password: "; getline(cin, pwd);
    e.encPassword = xorCrypt(pwd); // encrypt before saving
    cout << "URL/app: "; getline(cin, e.url);
    cout << "Notes: "; getline(cin, e.notes);
    v.push_back(e);
    cout << "Added.\n";
}

void searchEntry(const vector<Entry> &v) {
    cout << "Title to search: ";
    string t; getline(cin, t);
    int idx = findByTitle(v, t);
    if (idx < 0) { cout << "Not found.\n"; return; }
    auto &e = v[idx];
    cout << "Title: " << e.title << "\nURL: " << e.url << "\nNotes: " << e.notes << "\n";
    cout << "Show password? (y/n): ";
    char c; cin >> c; cin.ignore();
    if (c == 'y') cout << "Password: " << xorCrypt(e.encPassword) << "\n";
}

void updateEntry(vector<Entry> &v) {
    cout << "Title to update: ";
    string t; getline(cin, t);
    int idx = findByTitle(v, t);
    if (idx < 0) { cout << "Not found.\n"; return; }
    string pwd;
    cout << "New password: "; getline(cin, pwd);
    v[idx].encPassword = xorCrypt(pwd);
    cout << "Updated.\n";
}

void deleteEntry(vector<Entry> &v) {
    cout << "Title to delete: ";
    string t; getline(cin, t);
    int idx = findByTitle(v, t);
    if (idx < 0) { cout << "Not found.\n"; return; }
    v.erase(v.begin() + idx);
    cout << "Deleted.\n";
}

int main() {
    decryptFileIfExists(); // decrypt file on start (if exists)
    vector<Entry> entries = loadEntries();

    while (true) {
        cout << "\n1) Add  2) Search  3) Update  4) Delete  0) Exit\nChoice: ";
        int c; 
        if (!(cin >> c)) break; 
        cin.ignore();
        if (c == 0) break;
        if (c == 1) addEntry(entries);
        else if (c == 2) searchEntry(entries);
        else if (c == 3) updateEntry(entries);
        else if (c == 4) deleteEntry(entries);
        saveEntries(entries);
    }

    encryptFileOnExit(); // encrypt file on close
    return 0;
}
