// main.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINE_LENGTH 1024
#define MAX_RECORDS 5000

// Structures to hold data
typedef struct {
    char county[100];
    char state[100];
    float education_high_school;
    float education_bachelors;
    float ethnicities_white;
    float ethnicities_black;
    float ethnicities_hispanic;
    int income_median;
    int income_per_capita;
    float income_below_poverty;
    int population_2014;
} CountyDemographics;

// Global data
CountyDemographics records[MAX_RECORDS];
int record_count = 0;

// Function prototypes
void load_data(const char *filename);
void execute_operations(const char *filename);
void display_records();
void filter_state(const char *state);
void filter_field(const char *field, const char *operator, float value);
void population_total();
void population_field(const char *field);
void percentage_field(const char *field);
float calculate_percentage(float value, float total);

// Helper function prototypes
int is_numeric_field(const char *field);
float get_field_value(CountyDemographics *record, const char *field);

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <data_file> <operations_file>\n", argv[0]);
        return 1;
    }

    const char *data_file = argv[1];
    const char *operations_file = argv[2];

    load_data(data_file);
    execute_operations(operations_file);

    return 0;
}

void load_data(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Error opening data file");
        exit(1);
    }

    char line[MAX_LINE_LENGTH];
    int line_number = 0;

    // Skip header
    fgets(line, sizeof(line), file);

    while (fgets(line, sizeof(line), file)) {
        line_number++;
        if (record_count >= MAX_RECORDS) {
            fprintf(stderr, "Error: Maximum record limit exceeded\n");
            break;
        }

        CountyDemographics record;
        char *token = strtok(line, ",");

        // Parse fields
        strcpy(record.county, token ? token + 1 : "");  // Remove surrounding quotes
        token = strtok(NULL, ",");
        strcpy(record.state, token ? token + 1 : "");
        token = strtok(NULL, ",");
        record.education_bachelors = atof(token ? token + 1 : "0");
        token = strtok(NULL, ",");
        record.education_high_school = atof(token ? token + 1 : "0");
        token = strtok(NULL, ",");
        record.ethnicities_white = atof(token ? token + 1 : "0");
        token = strtok(NULL, ",");
        record.ethnicities_black = atof(token ? token + 1 : "0");
        token = strtok(NULL, ",");
        record.ethnicities_hispanic = atof(token ? token + 1 : "0");
        token = strtok(NULL, ",");
        record.income_median = atoi(token ? token + 1 : "0");
        token = strtok(NULL, ",");
        record.income_per_capita = atoi(token ? token + 1 : "0");
        token = strtok(NULL, ",");
        record.income_below_poverty = atof(token ? token + 1 : "0");
        token = strtok(NULL, ",");
        record.population_2014 = atoi(token ? token + 1 : "0");

        if (!token) {
            fprintf(stderr, "Error parsing line %d\n", line_number);
            continue;
        }

        records[record_count++] = record;
    }

    fclose(file);
    printf("%d records loaded\n", record_count);
}

void execute_operations(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Error opening operations file");
        exit(1);
    }

    char line[MAX_LINE_LENGTH];
    int line_number = 0;

    while (fgets(line, sizeof(line), file)) {
        line_number++;
        if (line[0] == '\n' || line[0] == '\0') continue; // Skip blank lines

        char operation[MAX_LINE_LENGTH];
        sscanf(line, "%s", operation);

        if (strcmp(operation, "display") == 0) {
            display_records();
        } else if (strncmp(operation, "filter-state:", 13) == 0) {
            filter_state(line + 13);
        } else if (strncmp(operation, "filter:", 7) == 0) {
            char field[50], operator[3];
            float value;
            sscanf(line + 7, "%[^:]:%[^:]:%f", field, operator, &value);
            filter_field(field, operator, value);
        } else if (strcmp(operation, "population-total") == 0) {
            population_total();
        } else if (strncmp(operation, "population:", 11) == 0) {
            population_field(line + 11);
        } else if (strncmp(operation, "percent:", 8) == 0) {
            percentage_field(line + 8);
        } else {
            fprintf(stderr, "Invalid operation on line %d\n", line_number);
        }
    }

    fclose(file);
}

void display_records() {
    for (int i = 0; i < record_count; i++) {
        printf("%s, %s\n", records[i].county, records[i].state);
    }
}

void filter_state(const char *state) {
    int count = 0;
    for (int i = 0; i < record_count; i++) {
        if (strcmp(records[i].state, state) == 0) {
            count++;
        }
    }
    printf("Filter: state == %s (%d entries)\n", state, count);
}

void filter_field(const char *field, const char *operator, float value) {
    int count = 0;
    for (int i = 0; i < record_count; i++) {
        float field_value = get_field_value(&records[i], field);
        if ((strcmp(operator, "ge") == 0 && field_value >= value) ||
            (strcmp(operator, "le") == 0 && field_value <= value)) {
            count++;
        }
    }
    printf("Filter: %s %s %.2f (%d entries)\n", field, operator, value, count);
}

void population_total() {
    int total = 0;
    for (int i = 0; i < record_count; i++) {
        total += records[i].population_2014;
    }
    printf("2014 population: %d\n", total);
}

void population_field(const char *field) {
    float total = 0;
    for (int i = 0; i < record_count; i++) {
        total += records[i].population_2014 * get_field_value(&records[i], field) / 100;
    }
    printf("2014 %s population: %.2f\n", field, total);
}

void percentage_field(const char *field) {
    float total_population = 0;
    float sub_population = 0;
    for (int i = 0; i < record_count; i++) {
        total_population += records[i].population_2014;
        sub_population += records[i].population_2014 * get_field_value(&records[i], field) / 100;
    }
    printf("2014 %s percentage: %.2f\n", field, (sub_population / total_population) * 100);
}

float get_field_value(CountyDemographics *record, const char *field) {
    if (strcmp(field, "Education.High School or Higher") == 0) return record->education_high_school;
    if (strcmp(field, "Education.Bachelor's Degree or Higher") == 0) return record->education_bachelors;
    if (strcmp(field, "Ethnicities.White Alone") == 0) return record->ethnicities_white;
    if (strcmp(field, "Ethnicities.Black Alone") == 0) return record->ethnicities_black;
    if (strcmp(field, "Ethnicities.Hispanic or Latino") == 0) return record->ethnicities_hispanic;
    if (strcmp(field, "Income.Persons Below Poverty Level") == 0) return record->income_below_poverty;
    return -1; // Invalid field
}
