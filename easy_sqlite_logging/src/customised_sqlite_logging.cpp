//
//  customised_sqlite_logging.cpp
//  GenesisPlus
//
//  Created by Alasdair Morrison on 11/07/2014.
//
//

#include "../include/customised_sqlite_logging.h"
#include "../include/easy_sqlite_logging.h"

easy_sqlite_table* calls_table = NULL;
easy_sqlite_table* jumps_table = NULL;
easy_sqlite_table* notes_table = NULL;
easy_sqlite_table* opcode_table = NULL;

void create_easy_sqlite_tables() {
    calls_table = create_table("Calls", "", "(Program_counter TEXT PRIMARY KEY, Value1 TEXT, Value2 TEXT)");
    jumps_table = create_table("Jumps", "", "(NewPC TEXT, OldPC TEXT, Value2 TEXT)");
    notes_table = create_table("Notes", "", "(Note TEXT, Value TEXT PRIMARY KEY, Value2 TEXT)");
    opcode_table = create_table("Opcodes", "", "(Note TEXT, Value TEXT PRIMARY KEY, Value2 TEXT)");
}

extern "C" {
    void log_opcode(char* opcode_name, char* value1, char* value2) {
        calls_table->insert_3_strings(opcode_name, value1, value2,"(Program_counter, Value1, Value2)");
    }
    void log_double_string_string(double double_value1, char* value2, char* value3) {
        calls_table->insert_3_strings(double_to_string(double_value1), value2, value3,"(Program_counter, Value1, Value2)");
    }

    void log_notes_string_double(char* str_value, double double_value) {
        notes_table->insert_3_strings(str_value, double_to_string(double_value), "","(Note, Value, Value2)");
    }

    void log_opcode_string_double(char* str_value, double double_value) {
        opcode_table->insert_3_strings(str_value, double_to_string(double_value), "","(Note, Value, Value2)");
    }
    
    void log_jump(int new_pc,int old_pc) {
        jumps_table->insert_3_strings(double_to_string(new_pc), double_to_string(new_pc), "","(Program_counter, Value1, Value2)");
    }
}