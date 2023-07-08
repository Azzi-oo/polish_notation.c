/* Компиляция:
* gcc -o rpn rpn.c
*
* Использование:
* ./rpn <выражение>
*
* Пример:
* ./rpn 3 5 +
*
* Замечание: знак умножения `*' заменен на `x', т.к. символ `*' используется
* командной оболочкой.
**/
 
#include <errno.h>
#include <math.h>
#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
 
#define STKDPTH 32 /* Глубина стека */
 
#define STATUS_OK 0
#define STATUS_ERR_READING 1
 
#define HEIGHT 25
#define WIDTH 80
 
#define GRAPH '*'
#define EMPTY '.'
 
#define VAL 0 /* В стек занесено новое значение */
#define ADD 1 /* Сложение */
#define SUB 2 /* Вычитание */
#define MUL 3 /* Умножение */
#define DIV 4 /* Деление */
#define COS 5 /* Косинус */
#define SIN 6 /* Синус */
#define SQR 7 /* Возведение в квадрат */
#define ROU 8 /* Возведение в 3 степень */
#define SOF -1 /* Переполнение стека */
#define SUF -2 /* В стеке недостаточно операндов */
#define UNK -3 /* Неопознанное значение */
 
int scount;
double stack[STKDPTH];
 
int ParseArgument(char *); /* Функция распознавания аргументов */
int GetArgument(int argc, char **argv);
int PrintGraf(int field[HEIGHT][WIDTH]);
int InitField(int field[HEIGHT][WIDTH]);
void UpdateField(int field[HEIGHT][WIDTH]);
void PrintField(const int field[HEIGHT][WIDTH]);
 
int main() {
int status_code = STATUS_OK;
 
char term_id[256];
ctermid(term_id);
int argc;
char **argv;
int field[HEIGHT][WIDTH] = {EMPTY};
status_code = InitField(field);
if (status_code == STATUS_OK && freopen(term_id, "r", stdin)) {
// Setup ncurses
initscr();
nodelay(stdscr, TRUE);
curs_set(0); // Make cursor invisible
 
char input = -1;
while (input != 'q') {
input = getch();
PrintField(field);
GetArgument(argc, *argv);
ParseArgument(argc);
GetGraph(field[HEIGHT][WIDTH], args, **argv);
UpdateField(field);
}
 
endwin();
} else
printf("Error reading file. See '/README.md'.\n");
 
return status_code;
}
 
int InitField(int field[HEIGHT][WIDTH]) {
int status_code = STATUS_OK;
 
// HEIGHT lines, each line contains
// WIDTH + 1 characters (because of '\n')
for (int y = 0; y != HEIGHT && status_code == STATUS_OK; ++y) {
for (int x = 0; x != WIDTH && status_code == STATUS_OK;) {
char c = getchar();
 
if (c == EMPTY) {
field[y][x] = EMPTY;
++x;
} else if (c == GRAPH) {
field[y][x] = GRAPH;
++x;
} else if (c != '\n')
status_code = STATUS_ERR_READING;
}
}
 
return status_code;
}
 
int GetArgument(int argc, char **argv) {
/* Организуем цикл для перебора аргументов командной строки */
while (*++argv) {
/* Пытаемся распознать текущий аргумент как число или
* символ арифметической операции */
switch (ParseArgument(*argv)) {
case VAL:
continue;
 
/* Вычисляем */
case ADD:
stack[scount - 1] += stack[scount];
break;
 
case SUB:
stack[scount - 1] -= stack[scount];
break;
 
case MUL:
stack[scount - 1] *= stack[scount];
break;
 
case DIV:
if (stack[scount] != 0) {
stack[scount - 1] /= stack[scount];
break;
} else {
fprintf(stderr, "Деление на ноль!\n");
return (1);
}
 
/* Обработка ошибок */
case SUF:
fprintf(stderr, "Недостаточно операндов!\n");
return (1);
 
case SOF:
fprintf(stderr, "Переполнение стека!\n");
return (1);
 
case UNK:
fprintf(stderr, "Неопознанный аргумент!\n");
return (1);
}
}
 
/* Вывести результат */
auto int i;
for (i = 0; i < scount; i++) printf("%0.3f\n", stack[i]);
 
return (0);
}
 
int ParseArgument(char *s) {
double tval = 0;
char *endptr;
 
/* Распознаем знаки арифметических операций */
switch (*s) {
case '-':
/* Если минус является первым и не последним символом аргумента,
* значит пользователь ввел отрицательное число и опознавать его
* как операцию вычитания не нужно */
if (*(s + 1) != '\0') break;
if (scount >= 2) {
scount -= 1;
return (SUB);
} else
return (SUF);
 
case '+':
if (scount >= 2) {
scount -= 1;
return (ADD);
} else
return (SUF);
 
case 'x':
if (scount >= 2) {
scount -= 1;
return (MUL);
} else
return (SUF);
 
case '/':
if (scount >= 2) {
scount -= 1;
return (DIV);
} else
return (SUF);
}
 
errno = 0;
 
/* Пытаемся сконвертировать строковый аргумент в число */
tval = strtod(s, &endptr);
 
/* Вернуть ошибку `неопознанный аргумент' в случае неудачи */
if (errno != 0 || *endptr != '\0') return (UNK);
 
/* Проверяем, есть ли свободное место в стеке
* и сохраняем в нем операнд, иначе возвращаем ошибку переполнения */
if (scount < STKDPTH)
stack[scount++] = tval;
else
return (SOF);
 
return (VAL);
}
 
int GetGraph(const int field[HEIGHT][WIDTH], const int y, const int x) {
for (int i = -1; i < 2; ++i) {
for (int j = -1; j < 2; ++j) {
int dy = y + i < 0 ? HEIGHT - 1 : (y + i) % HEIGHT;
int dx = x + j < 0 ? WIDTH - 1 : (x + j) % WIDTH;
if (field[dy][dx] && (i != 0 || j != 0))
;
}
}
 
return 0;
}
 
void UpdateField(int field[HEIGHT][WIDTH]) {
int field_old[HEIGHT][WIDTH];
for (int y = 0; y != HEIGHT; ++y) {
for (int x = 0; x != WIDTH; ++x) {
int *cell = &(field[y][x]);
int graphic = GetGraph(field, y, x);
}
}
}
 
void PrintField(const int field[HEIGHT][WIDTH]) {
clear();
 
for (int y = 0; y != HEIGHT; ++y) {
for (int x = 0; x != WIDTH; ++x) {
if (field[y][x] == EMPTY)
addch(EMPTY);
else
addch(GRAPH);
}
printw("\n");
}
 
refresh();
}
