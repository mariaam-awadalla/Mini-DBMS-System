#include "Database.h"
#include "Utils.h"
#include <filesystem>
#include <iostream>
#include <sstream>

namespace fs = std::filesystem;

// helper: remove trailing semicolon
static std::string removeSemicolon(const std::string &s) {
    if (!s.empty() && s.back() == ';')
        return s.substr(0, s.size() - 1);
    return s;
}

Database::Database() { currentDB = ""; }

// ==================== DATABASE ====================
void Database::createDatabase(std::string dbName) {
    dbName = trim(removeSemicolon(dbName));
    std::string path = "data/" + dbName;

    if (!fs::exists(path)) {
        fs::create_directories(path);
        std::cout << "Database '" << dbName << "' created.\n";
    } else {
        std::cout << "Database already exists.\n";
    }
}

void Database::useDatabase(std::string dbName) {
    dbName = trim(removeSemicolon(dbName));
    std::string path = "data/" + dbName;

    if (!fs::exists(path)) {
        std::cout << "Database does not exist.\n";
        return;
    }

    currentDB = dbName;
    tables.clear();
    loadAllTables(path);

    std::cout << "Using database '" << currentDB << "'.\n";
}

std::string Database::getDBPath() {
    if (currentDB.empty())
        return "";
    return "data/" + currentDB;
}

// ==================== TABLE CONTROL ====================
void Database::createTable(std::string tableName) {
    tableName = trim(removeSemicolon(tableName));

    if (currentDB.empty()) {
        std::cout << "No database selected. Use: USE databaseName;\n";
        return;
    }

    Table t(tableName);
    tables[tableName] = t;

    std::string path = getDBPath() + "/" + tableName + ".json";
    tables[tableName].save(path);

    std::cout << "Table '" << tableName << "' created.\n";
}

Table *Database::getTable(std::string tableName) {
    tableName = trim(removeSemicolon(tableName));
    if (tables.count(tableName))
        return &tables[tableName];
    return nullptr;
}

void Database::loadAllTables(const std::string &folderPath) {
    if (!fs::exists(folderPath))
        return;

    for (const auto &file : fs::directory_iterator(folderPath)) {
        if (file.path().extension() == ".json") {
            std::string tableName = file.path().stem().string();
            Table t;
            t.load(file.path().string());
            tables[tableName] = t;
        }
    }
}

void Database::saveTable(const std::string &tableName,
                         const std::string &folderPath) {
    std::string tname = trim(removeSemicolon(tableName));
    if (tables.count(tname)) {
        std::string path = folderPath + "/" + tname + ".json";
        tables[tname].save(path);
    }
}

// ==================== EXECUTE SQL ====================
void Database::executeSQL(std::string sql) {
    Parser parser;

    sql = trim(removeSemicolon(sql));
    std::string up = parser.upper(sql);

    // ---------- CREATE DATABASE ----------
    if (up.rfind("CREATE DATABASE", 0) == 0) {
        createDatabase(sql.substr(16));
        return;
    }

    // ---------- USE ----------
    if (up.rfind("USE ", 0) == 0) {
        useDatabase(sql.substr(4));
        return;
    }

    // ---------- CREATE TABLE ----------
    if (up.rfind("CREATE TABLE", 0) == 0) {
        CreateTableCmd cmd = parser.parseCreate(sql);
        if (!cmd.ok) {
            std::cout << "Invalid CREATE TABLE syntax.\n";
            return;
        }

        createTable(cmd.tableName);
        Table *t = getTable(cmd.tableName);

        for (int i = 0; i < cmd.columns.size(); i++)
            t->addColumn(trim(cmd.columns[i]), trim(cmd.types[i]));

        saveTable(cmd.tableName, getDBPath());
        return;
    }

    if (currentDB.empty()) {
        std::cout << "No database selected.\n";
        return;
    }

    // ---------- DROP TABLE ----------
    if (up.rfind("DROP TABLE", 0) == 0) {
        if (currentDB.empty()) {
            std::cout << "No database selected.\n";
            return;
        }

        std::string tableName = trim(sql.substr(10)); // بعد DROP TABLE

        if (!tables.count(tableName)) {
            std::cout << "Table does not exist.\n";
            return;
        }

        // delete from hdd
        std::string path = getDBPath() + "/" + tableName + ".json";
        if (fs::exists(path))
            fs::remove(path);

        // delete from memory
        tables.erase(tableName);

        std::cout << "Table '" << tableName << "' dropped.\n";
        return;
    }
    // ---------- DROP DATABASE ----------
    if (up.rfind("DROP DATABASE", 0) == 0) {
        std::string dbName = trim(sql.substr(13));
        std::string path = "data/" + dbName;

        if (!fs::exists(path)) {
            std::cout << "Database does not exist.\n";
            return;
        }

        if (currentDB == dbName) {
            currentDB = "";
            tables.clear();
        }

        fs::remove_all(path);
        std::cout << "Database '" << dbName << "' dropped.\n";
        return;
    }

    // ---------- INSERT ----------

    if (up.rfind("INSERT INTO", 0) == 0) {
        InsertCmd cmd = parser.parseInsert(sql);
        if (!cmd.ok) {
            std::cout << "Invalid INSERT syntax.\n";
            return;
        }

        Table *t = getTable(cmd.tableName);
        if (!t) {
            std::cout << "Table does not exist.\n";
            return;
        }

        for (auto &vals : cmd.rows) {
            std::vector<std::string> row(t->columns.size(), "");

            if (cmd.columns.empty()) {
                if (vals.size() != t->columns.size()) {
                    std::cout << "Error: number of values does not match columns.\n";
                    return;
                }
                row = vals;
            } else {
                if (cmd.columns.size() != vals.size()) {
                    std::cout << "Error: columns count does not match values.\n";
                    return;
                }

                for (int i = 0; i < cmd.columns.size(); i++) {
                    int idx = t->getColumnIndex(cmd.columns[i]);
                    if (idx == -1) {
                        std::cout << "Unknown column '" << cmd.columns[i] << "'.\n";
                        return;
                    }
                    row[idx] = vals[i];
                }
            }

            // 🔒 enforce full row
            for (int i = 0; i < row.size(); i++) {
                if (row[i].empty()) {
                    std::cout << "Error: column '" << t->columns[i]
                              << "' has no value.\n";
                    return;
                }
            }

            t->insertRow(row);
        }

        saveTable(cmd.tableName, getDBPath());
        std::cout << "Rows inserted.\n";
        return;
    }

    // ---------- SELECT ----------
    // ---------- SELECT ----------
    if (up.rfind("SELECT", 0) == 0) {
        SelectCmd cmd = parser.parseSelect(sql);
        if (!cmd.ok) {
            std::cout << "Invalid SELECT syntax.\n";
            return;
        }

        Table *t = getTable(cmd.tableName);
        if (!t) {
            std::cout << "Table does not exist.\n";
            return;
        }

        std::cout << "__TABLE__\n";

        std::vector<int> colIndexes;

        // columns
        if (cmd.selectAll) {
            for (int i = 0; i < t->columns.size(); i++)
                colIndexes.push_back(i);
        } else {
            for (auto &c : cmd.columns) {
                int idx = t->getColumnIndex(c);
                if (idx == -1) {
                    std::cout << "Unknown column '" << c << "'.\n";
                    return;
                }
                colIndexes.push_back(idx);
            }
        }

        // header
        for (int i = 0; i < colIndexes.size(); i++) {
            std::cout << t->columns[colIndexes[i]];
            if (i + 1 != colIndexes.size())
                std::cout << " ";
        }
        std::cout << "\n";

        // rows
        int whereIdx = -1;
        if (cmd.hasWhere) {
            whereIdx = t->getColumnIndex(cmd.whereColumn);
            if (whereIdx == -1) {
                std::cout << "Unknown column in WHERE.\n";
                return;
            }
        }

        for (auto &r : t->rows) {
            if (cmd.hasWhere && r[whereIdx] != cmd.whereValue)
                continue;

            for (int i = 0; i < colIndexes.size(); i++) {
                std::cout << r[colIndexes[i]];
                if (i + 1 != colIndexes.size())
                    std::cout << " ";
            }
            std::cout << "\n";
        }

        return;
    }

    // ---------- UPDATE (MULTI COLUMN) ----------
    if (up.rfind("UPDATE", 0) == 0) {
        UpdateCmd cmd = parser.parseUpdate(sql);
        if (!cmd.ok) {
            std::cout << "Invalid UPDATE syntax.\n";
            return;
        }

        Table *t = getTable(cmd.tableName);
        if (!t) {
            std::cout << "Table does not exist.\n";
            return;
        }

        std::vector<int> setIndexes;
        for (int i = 0; i < cmd.setColumns.size(); i++) {
            int idx = t->getColumnIndex(cmd.setColumns[i]);
            if (idx == -1 || idx == 0) {
                std::cout << "Invalid SET column.\n";
                return;
            }
            setIndexes.push_back(idx);
        }

        int whereIdx = -1;
        if (cmd.hasWhere) {
            whereIdx = t->getColumnIndex(cmd.whereColumn);
            if (whereIdx == -1) {
                std::cout << "Unknown column in WHERE.\n";
                return;
            }
        }

        for (auto &r : t->rows) {
            if (!cmd.hasWhere || r[whereIdx] == cmd.whereValue)
                for (int i = 0; i < setIndexes.size(); i++)
                    r[setIndexes[i]] = cmd.setValues[i];
        }

        saveTable(cmd.tableName, getDBPath());
        std::cout << "Update done.\n";
        return;
    }

    // ---------- DELETE ----------
    if (up.rfind("DELETE FROM", 0) == 0) {
        DeleteCmd cmd = parser.parseDelete(sql);
        if (!cmd.ok) {
            std::cout << "Invalid DELETE syntax.\n";
            return;
        }

        Table *t = getTable(cmd.tableName);
        if (!t) {
            std::cout << "Table does not exist.\n";
            return;
        }

        if (!cmd.hasWhere)
            t->rows.clear();
        else {
            int idx = t->getColumnIndex(cmd.whereColumn);
            std::vector<std::vector<std::string>> newRows;
            for (auto &r : t->rows)
                if (r[idx] != cmd.whereValue)
                    newRows.push_back(r);
            t->rows = newRows;
        }

        saveTable(cmd.tableName, getDBPath());
        std::cout << "Delete done.\n";
        return;
    }

    std::cout << "Unknown SQL command.\n";
}

// ==================== QT SUPPORT ====================
std::string Database::executeSQLWithResult(std::string sql) {
    std::stringstream buffer;
    std::streambuf *old = std::cout.rdbuf(buffer.rdbuf());
    executeSQL(sql);
    std::cout.rdbuf(old);
    return buffer.str();
}
