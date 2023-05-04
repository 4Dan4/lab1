#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ftw.h>
#include <getopt.h>

const char *target_sequence; // глобальная переменная для хранения целевой последовательности

void print_version() {
    printf("Version 1.0\n"); // здесь можно указать актуальную версию программы
    printf("Author: Daniil Pinigin\n");
    printf("Group: N32452\n");
    printf("Lab: #1, variant 3\n");
}

void print_help() {
    printf("Usage: search_sequence [OPTIONS] directory target_sequence\n");
    printf("Search for files containing the specified byte sequence.\n");
    printf("\n");
    printf("Options:\n");
    printf("-v, --version     Print version information and exit.\n");
    printf("-h, --help        Print this help message and exit.\n");
}

int search_file(const char *filepath, const struct stat *file_info, int typeflag, struct FTW *ftwbuf) {
    if (typeflag == FTW_F) { // если это файл (а не директория)
        FILE *fp;
        char buffer[strlen(target_sequence)];
        fp = fopen(filepath, "rb"); // открываем файл для чтения в бинарном режиме
        if (fp == NULL) { // если не удалось открыть файл
            printf("Could not open file %s\n", filepath);
            return 1;
        }
        int found_sequence = 0; // флаг, указывающий, найдена ли целевая последовательность в файле
        size_t bytes_read = fread(buffer, sizeof(char), sizeof(buffer), fp); // читаем файл блоками по 1024 байта
        while (bytes_read > 0) { // продолжаем чтение, пока не достигнем конца файла
            if(memmem(buffer, bytes_read, target_sequence, strlen(target_sequence)) != NULL){
                printf("%s\n", filepath);
                break;
            }
            bytes_read = fread(buffer, sizeof(char), sizeof(buffer), fp); // читаем следующий блок данных
        }
        fclose(fp); // закрываем файл
    }
    return 0;
}

int main(int argc, char *argv[]) {
    if (getenv("LAB11DEBUG") != NULL) {
        printf("Debug option\n");
    }

    int option_index = 0;
    static struct option long_options[] = {
            {"version", no_argument, 0, 'v'},
            {"help", no_argument, 0, 'h'},
            {0, 0, 0, 0}
    };
    int c;
    while ((c = getopt_long(argc, argv, "vh", long_options, &option_index)) != -1) {
        switch (c) {
            case 'v':
                print_version();
                return 0;
            case 'h':
                print_help();
                return 0;
            case '?':
                printf("Unknown option: %s\n", argv[optind - 1]);
                return 1;
        }
    }
    if (argc != 3) { // если количество аргументов неверно
        printf("Usage: %s directory target_sequence\n", argv[0]);
        return 1;
    }
    target_sequence = argv[2]; // сохраняем целевую последовательность в глобальной переменной
    int flags = FTW_PHYS | FTW_DEPTH; // флаги для функции ftw()
    if (ftw(argv[1], search_file, 10) == -1) { // рекурсивно обходим указанный каталог и его подкаталоги
        printf("Error searching directory\n");
        return 1;
    }
    return 0;
}