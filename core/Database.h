#ifndef DATABASE_H
#define DATABASE_H

#include <string>
#include <map>
#include "Table.h"
#include "Parser.h"

class Database {
public:
    std::map<std::string, Table> tables;
    std::string currentDB;

    Database();

    // databases
    void createDatabase(std::string dbName);
    void useDatabase(std::string dbName);
    std::string getDBPath();

    // tables
    void createTable(std::string tableName);
    Table* getTable(std::string tableName);
    void loadAllTables(const std::string& folderPath);
    void saveTable(const std::string& tableName, const std::string& folderPath);

    // executor (console)
    void executeSQL(std::string sql);

    // 🔥 NEW: executor for GUI (Qt)
    std::string executeSQLWithResult(std::string sql);
};

#endif
