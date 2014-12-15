//
//  easy_sqlite_logging.c
//  GenesisPlus
//
//  Created by Alasdair Morrison on 11/07/2014.
//
//

#ifdef __cplusplus
    #include <vector>
    
    #include <sstream>
    

    // # Warn: Mac Only imports
    #import <iostream>
    #import <time.h>
    #import <mach/mach.h> 
    #include <execinfo.h>
    #include <signal.h>
    #include "sqlite3.h"
#endif

#include "../include/easy_sqlite_logging.h"

// # Global variables
    std::string global_database_name = "";
    bool global_created_database = false;
    sqlite3 *global_database_for_the_easy_sqlite_library;
    std::vector<easy_sqlite_table*> vector_of_tables;
    

// # Execute Query
//  * Executes the query defined in the query parameter
    void execute_query(const char* query) {
        std::cout << "Query:" << query;
        char * sErrMsg = 0;
        sqlite3_exec(global_database_for_the_easy_sqlite_library, query, NULL, NULL, &sErrMsg);
    }

// # Create Table
//  *  Creates a new table in the sqlite database with the provided table_name and schema
    easy_sqlite_table* create_table(std::string table_name, std::string table_name_suffix, std::string schema) {
        
        std::ostringstream oss;
        oss << "CREATE TABLE IF NOT EXISTS \"" << table_name << table_name_suffix  << "\" "<< schema;
        char * sErrMsg = 0;
        sqlite3_exec(global_database_for_the_easy_sqlite_library, oss.str().c_str(), NULL, NULL, &sErrMsg);
        return new easy_sqlite_table(table_name,table_name_suffix, schema);
    }

// # Start Insert
// * Starts a new insert statement
    sqlite3_stmt * start_insert(std::string table_name, std::string table_name_suffix, std::string schema) {
        sqlite3_stmt * stmt;
        const char * tail = 0;
        
        std::ostringstream oss;
        oss << "INSERT INTO \"" << table_name << table_name_suffix  << "\" " << schema;
        sqlite3_prepare_v2(global_database_for_the_easy_sqlite_library,  oss.str().c_str(), oss.str().length(), &stmt, &tail);
        return stmt;
    }

// # Step Stmt
// * Step statement must be used after start_insert
    int step_stmt(sqlite3_stmt * stmt) {
        int result = sqlite3_step(stmt);
        sqlite3_clear_bindings(stmt);
        sqlite3_reset(stmt);
        return result;
    }


int bind_change_sql(sqlite3_stmt * stmt, std::string unique_id, int col_start, int col_end, int line_num, int tim ) {
    sqlite3_bind_text(stmt, 1, unique_id.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 2, col_start);
    sqlite3_bind_int(stmt, 3, col_end);
    sqlite3_bind_int(stmt, 4, line_num);
    sqlite3_bind_int(stmt, 5, tim);
    
    return step_stmt(stmt);
}

int bind_execution_sql(sqlite3_stmt * stmt, std::string unique_id, int start_time, int end_time) {
    sqlite3_bind_text(stmt, 1, unique_id.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 2, start_time);
    sqlite3_bind_int(stmt, 3, end_time);
    return step_stmt(stmt);
}

// # Open Sqlite 
//  * Opens or creates a new sqlite database with the provided name
    void open_sqlite(std::string db_name) {
        // ## Set the database name depending on platform
            #ifdef IPHONE
                db_name = get_dbpath_path();
            #endif
            global_database_name = db_name.c_str();

        // ## Open database 
            int rc = sqlite3_open(db_name.c_str(), &global_database_for_the_easy_sqlite_library);
            if( rc ){
                fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(global_database_for_the_easy_sqlite_library));
                //exit(0);
            } else {
                global_created_database=true;
                create_table("alang_screenshots", "", " (Time INTEGER PRIMARY KEY, Path TEXT) ");
            }
    }

void create_easy_sqlite_tables();

bool initialised_easy_sqlite_library = false;

extern "C" {
    void initialise_easy_sqlite_library() {
        printf("Actually going to initialise wish me luck!");
        if (initialised_easy_sqlite_library) return;
        open_sqlite("easy_sqlite_library.db");
        std::string table_name = "OpCodeCalls";
        std::string table_name_suffix = "";
        std::string schema = "";
        create_table(table_name, table_name_suffix, schema);
        create_easy_sqlite_tables();
        initialised_easy_sqlite_library=true;
    }
}

void easy_sqlite_table::insert_3_strings(std::string str1, std::string str2, std::string str3, std::string schema) {
    std::ostringstream oss;
    oss << "( ";
    oss << "'" << str1 << "', ";
    oss << "'" << str2 << "', ";
    oss << "'" << str3 << "'";
    oss <<")";
    insert_data(oss.str());
}

void easy_sqlite_table::setup_table() {
    vector_of_tables.push_back(this);
}

// # Insert data adds data to the Map before flushing to the database
void easy_sqlite_table::insert_data(std::string data) {
    int count = 0;
    map_of_values.insert(TemplateOfStrStrPair(data, double_to_string(count)));
}

void easy_sqlite_table::insert_data_to_database(std::string data, std::string schema) {
    std::ostringstream oss;
    oss << "INSERT INTO \"" << table_name << table_name_suffix  << "\" " /*<< schema*/ << " VALUES " << data;
    execute_query(oss.str().c_str());
}

void easy_sqlite_table::flush_data() {
    TemplateOfStrStrMap::iterator p;
    for(p = map_of_values.begin(); p!=map_of_values.end(); ++p)
        {
            // printf("Flushing all sqlite tables");
            std::string strValue = p->second;
            std::string strKey= p->first;
            insert_data_to_database(strKey, schema);
        }
}

extern "C" {
const char* double_to_string(double double_value) {
    std::ostringstream s;
    s << double_value;
    return s.str().c_str();
    
}

void flush_all_sqlite_tables() {
    
    for (std::vector<easy_sqlite_table*>::iterator it = vector_of_tables.begin(); it != vector_of_tables.end(); ++it)
       { 
        
        easy_sqlite_table* table = *it;
        
        table->flush_data();
        
        }
}

}
