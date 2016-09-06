#include "CsvReader.h"

#define BUFFER_SIZE 1024
#define DATA_SIZE 4096
#define FIELDS_SIZE 10

static inline int readchar(struct csv_parser* csv) {
    if (csv->buffer_char >= 0) {
        char c = csv->buffer_char;
        csv->buffer_char = -1;
        return c;
    }

    if (csv->buffer_pos == csv->buffer_size) {
        size_t length = fread(csv->buffer, 1, BUFFER_SIZE, csv->file);
        if (length == 0) {
            return EOF;
        }
        csv->buffer_pos = 0;
        csv->buffer_size = (int)length;
    }
    return csv->buffer[csv->buffer_pos++];
}

static inline void unreadchar(struct csv_parser *csv, int c) {
    csv->buffer_char = c;
}

static inline bool isspace(int c) {
    return (c >= 9 && c <= 13) || c == 32 || c == 133 || c == 160;
}

static inline void writechar(struct csv_parser* csv, char c) {
    if (csv->data_ptr == csv->data + csv->data_size) {
        csv->data = realloc(csv->data, 2 * csv->data_size);
        csv->data_ptr = csv->data + csv->data_size;
        csv->data_size = 2 * csv->data_size;
    }

    if (csv->trim_whitespace && csv->data_ptr == csv->field_ptr && isspace(c)) {
        return;
    }

    *csv->data_ptr++ = c;
}

static inline void writefield(struct csv_parser* csv) {
    if (csv->fields_count == csv->fields_size) {
        csv->fields = realloc(csv->fields, 2 * csv->fields_size * sizeof(char*));
        csv->fields_size = 2 * csv->fields_size;
    }

    if (csv->trim_whitespace) {
        while (csv->data_ptr > csv->field_ptr) {
            if (isspace(*(csv->data_ptr - 1))) {
                csv->data_ptr--;
            } else {
                break;
            }
        }
    }

    writechar(csv, '\0');
    csv->fields[csv->fields_count++] = csv->field_ptr;
    csv->field_ptr = csv->data_ptr;
}

struct csv_parser* csv_init(const char* path) {
    FILE* file = fopen(path, "rb");
    if (!file) {
        return NULL;
    }

    struct csv_parser* csv = malloc(sizeof(struct csv_parser));
    csv->file = file;
    csv->buffer = (char*)malloc(BUFFER_SIZE);
    csv->buffer_pos = 0;
    csv->buffer_size = 0;
    csv->buffer_char = -1;
    csv->data = (char*)malloc(DATA_SIZE);
    csv->data_size = DATA_SIZE;
    csv->fields = (char**)malloc(FIELDS_SIZE * sizeof(char*));
    csv->fields_size = FIELDS_SIZE;
    csv->fields_count = 0;
    csv->delimiter = ',';
    csv->text_qualifier = '"';
    csv->trim_whitespace = true;
    csv->skip_empty_records = true;
    return csv;
}

bool csv_read_record(struct csv_parser* csv) {
    csv->fields_count = 0;
    csv->data_ptr = csv->data;
    csv->field_ptr = csv->data;

    int c;
    bool quoted = false;
    while ((c = readchar(csv)) != EOF) {
        if (c == '\n' || c == '\r') {
            if (quoted) {
                writechar(csv, c);
            } else {
                if (c == '\r') {
                    int next = readchar(csv);
                    if (next != '\n') {
                        unreadchar(csv, next);
                    }
                }
                writefield(csv);
                if (csv->fields_count == 1 && csv->skip_empty_records && csv->fields[0][0] == 0) {
                    continue;
                }
                return true;
            }
        } else if (c == csv->delimiter) {
            if (quoted) {
                writechar(csv, c);
            } else {
                writefield(csv);
            }
        } else if (c == csv->text_qualifier) {
            if (quoted) {
                int next = readchar(csv);
                if (next == csv->text_qualifier) {
                    writechar(csv, csv->text_qualifier);
                } else {
                    unreadchar(csv, next);
                    quoted = false;
                }
            } else {
                quoted = true;
            }
        } else {
            writechar(csv, c);
        }
    }

    return false;
}

int csv_get_field_count(struct csv_parser* csv) {
    return csv->fields_count;
}

const char* csv_get_field(struct csv_parser* csv, int index) {
    static const char* dummy = "";
    if (index >= 0 && index < csv->fields_count) {
        return csv->fields[index];
    }
    return dummy;
}

void csv_free(struct csv_parser* parser) {
    free(parser->buffer);
    free(parser->data);
    free(parser->fields);
    free(parser);
}
