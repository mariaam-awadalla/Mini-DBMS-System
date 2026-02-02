#include "Parser.h"
#include "Utils.h"

std::string Parser::upper(std::string s) {
    for (int i = 0; i < s.size(); i++)
        if (s[i] >= 'a' && s[i] <= 'z')
            s[i] -= 32;
    return s;
}

// ---------------------------
// CREATE TABLE
// ---------------------------
CreateTableCmd Parser::parseCreate(std::string sql) { //CREATE TABLE students (id INT, name STRING);

    CreateTableCmd cmd; //struct
    cmd.ok = false;

    std::string up = upper(sql);

    if (up.find("CREATE TABLE") != 0)//cmd.ok = false
        return cmd;

    int pos1 = up.find("TABLE") + 5;
    int pos2 = sql.find("(");

    cmd.tableName = trim(sql.substr(pos1, pos2 - pos1));

    
    int pos3 = sql.find("(");
    int pos4 = sql.rfind(")");

    std::string inside = sql.substr(pos3 + 1, pos4 - pos3 - 1);//"id INT, name STRING"

    std::vector<std::string> parts = split(inside, ','); //["id INT", "name STRING"]

    for (int i = 0; i < parts.size(); i++) {
        std::vector<std::string> p = split(parts[i], ' '); //p --> "id INT" → ["id", "INT"]
        if (p.size() == 2) {
            cmd.columns.push_back(p[0]);
            cmd.types.push_back(p[1]);
            /*cmd.columns = {"id", "name"}
              cmd.types   = {"INT", "STRING"}*/
        }
    }

    cmd.ok = true;
    return cmd;
}

// ---------------------------
// INSERT
// ---------------------------
InsertCmd Parser::parseInsert(std::string sql)
{
    InsertCmd cmd;
    cmd.ok = false;

    std::string up = upper(sql);

    if (up.find("INSERT INTO") != 0)
        return cmd;

    int posInto = up.find("INTO") + 4;
    int posValues = up.find("VALUES");

    std::string between =
        trim(sql.substr(posInto, posValues - posInto));

    // ---- table name + optional column list ----
    int parenOpen = between.find("(");

    if (parenOpen != std::string::npos)
    {
        cmd.tableName = trim(between.substr(0, parenOpen));

        int parenClose = between.find(")");
        std::string cols =
            between.substr(parenOpen + 1, parenClose - parenOpen - 1);

        cmd.columns = split(cols, ',');
    }
    else
    {
        cmd.tableName = trim(between);
    }

    // ---- parse multiple VALUES ----
    int i = sql.find("(", posValues);

    while (i != std::string::npos)
    {
        int j = sql.find(")", i);
        if (j == std::string::npos)
            break;

        std::string inside = sql.substr(i + 1, j - i - 1);
        std::vector<std::string> vals = split(inside, ',');

        for (auto &v : vals)
            v = removeQuotes(v);

        cmd.rows.push_back(vals);

        i = sql.find("(", j);
    }

    if (!cmd.rows.empty())
        cmd.ok = true;

    return cmd;
}



// ---------------------------
// SELECT
// ---------------------------
SelectCmd Parser::parseSelect(std::string sql)
{
    SelectCmd cmd;
    cmd.ok = false;
    cmd.selectAll = false;
    cmd.hasWhere = false;

    std::string up = upper(sql);

    if (up.find("SELECT") != 0)
        return cmd;

    int posFrom = up.find("FROM");
    int posWhere = up.find("WHERE");

    // ----- columns -----
    std::string cols =
        trim(sql.substr(6, posFrom - 6));

    if (cols == "*")
        cmd.selectAll = true;
    else
        cmd.columns = split(cols, ',');

    // ----- table name -----
    if (posWhere == std::string::npos)
    {
        cmd.tableName = trim(sql.substr(posFrom + 4));
    }
    else
    {
        cmd.tableName =
            trim(sql.substr(posFrom + 4, posWhere - (posFrom + 4)));

        // WHERE part
        std::string cond = sql.substr(posWhere + 5);
        std::vector<std::string> p = split(cond, '=');

        if (p.size() == 2)
        {
            cmd.hasWhere = true;
            cmd.whereColumn = trim(p[0]);
            cmd.whereValue = removeQuotes(trim(p[1]));
        }
    }

    cmd.ok = true;
    return cmd;
}

// ---------------------------
// DELETE
// ---------------------------
DeleteCmd Parser::parseDelete(std::string sql) {
    DeleteCmd cmd;
    cmd.ok = false;
    cmd.hasWhere = false;
//DELETE FROM students WHERE id = 3
    std::string up = upper(sql);

    if (up.find("DELETE FROM") != 0)
        return cmd;

    int posFrom = up.find("FROM") + 4;
    int posWhere = up.find("WHERE");

    if (posWhere == -1) {
        cmd.tableName = trim(sql.substr(posFrom));
    }
    else {
        cmd.tableName = trim(sql.substr(posFrom, posWhere - posFrom));

        std::string cond = sql.substr(posWhere + 5);
        std::vector<std::string> p = split(cond, '=');

        if (p.size() == 2) {
            cmd.hasWhere = true;
            cmd.whereColumn = trim(p[0]);
            cmd.whereValue = removeQuotes(trim(p[1]));
        }
    }
   //cmd.whereColumn = "id"
    //cmd.whereValue = "3"
   //cmd.hasWhere    = true


    cmd.ok = true;
    return cmd;
}

// ---------------------------
// UPDATE
// ---------------------------
UpdateCmd Parser::parseUpdate(std::string sql) {
    UpdateCmd cmd;
    cmd.ok = false;
    cmd.hasWhere = false;

    std::string up = upper(sql);

    if (up.find("UPDATE") != 0)
        return cmd;

    int posSet = up.find("SET");
    int posWhere = up.find("WHERE");

    cmd.tableName = trim(sql.substr(6, posSet - 6));

    std::string setPart;

    if (posWhere == -1)
        setPart = trim(sql.substr(posSet + 3));
    else
        setPart = trim(sql.substr(posSet + 3, posWhere - (posSet + 3)));

    // std::vector<std::string> p = split(setPart, '=');

    // cmd.setColumn = trim(p[0]);
    // cmd.setValue = removeQuotes(trim(p[1]));
    // split SET part by comma
    std::vector<std::string> assignments = split(setPart, ',');

    for (auto &a : assignments)
    {
        std::vector<std::string> p = split(a, '=');
        if (p.size() == 2)
        {
            cmd.setColumns.push_back(trim(p[0]));
            cmd.setValues.push_back(removeQuotes(trim(p[1])));
        }
    }

    if (posWhere != -1) {
        cmd.hasWhere = true;

        std::string cond = sql.substr(posWhere + 5);
        std::vector<std::string> pp = split(cond, '=');

        cmd.whereColumn = trim(pp[0]);
        cmd.whereValue = removeQuotes(trim(pp[1]));
    }

    cmd.ok = true;
    return cmd;
}
