#ifndef CsvReader_h
#define CsvReader_h

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

struct csv_parser {
    FILE* file;
    char* buffer;
    int buffer_pos;
    int buffer_size;
    int buffer_char;
    char* data;
    char* data_ptr;
    int data_size;
    char** fields;
    char* field_ptr;
    int fields_size;
    int fields_count;
    char delimiter;
    char text_qualifier;
    bool trim_whitespace;
    bool skip_empty_records;
};

struct csv_parser* csv_init(const char* path);
bool csv_read_record(struct csv_parser* csv);
int csv_get_field_count(struct csv_parser* csv);
const char* csv_get_field(struct csv_parser* csv, int index);
void csv_free(struct csv_parser* csv);

#endif /* CsvReader_h */
