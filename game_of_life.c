#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ncurses/ncurses.h>
#include <dirent.h> 

#define HEIGHT 25
#define WEIGHT 80
#define QUIT 113
#define PLUS 61
#define MINUS 45
#define SLEEP_GAIN 5000
#define MINSLEEP 1000000

/* Main game loop. */
void run(int drp[HEIGHT][WEIGHT], int *frame);
/* Initialization drp from stdin */
void init_drp(int drp[HEIGHT][WEIGHT]);
/* Conversion drp to char and output */
void output(int drp[HEIGHT][WEIGHT], int frame);
/* Evolution, make next drp condition, return cells count */
int evolution(int drp[HEIGHT][WEIGHT], int *status);
/* Copy drp */
int copy_drp(int drp[HEIGHT][WEIGHT], int new_drp[HEIGHT][WEIGHT]);
/* Handler of user input */
void user_input_handler(int *status, int *sleep);
void print_end_game(int drp[HEIGHT][WEIGHT], int frame);

int main() {
    // DRP - discrete working field
    int drp[HEIGHT][WEIGHT] = {0}, frame = 0;
    init_drp(drp);

    initscr();
    noecho();
    nodelay(stdscr, true);
    
    run(drp, &frame);
    endwin();
    print_end_game(drp, frame);
    return 0;
}

void run(int drp[HEIGHT][WEIGHT], int *frame) {
    int status = 1;
    int sleep = 20000;
    while (status) {
        usleep(sleep);
        clear();
        
        int cells = evolution(drp, &status);
        *frame += 1;
        output(drp, *frame);

        refresh();
        // If cells died the game is over
        if (!cells) status = 0;
        user_input_handler(&status, &sleep);
    }
}

void user_input_handler(int *status, int *sleep) {
    int user_input;
    user_input = getch();
    switch (user_input) {
        case MINUS:
            if (*sleep + SLEEP_GAIN < MINSLEEP) *sleep += SLEEP_GAIN;
            break;
        case PLUS:
            if (*sleep - SLEEP_GAIN > 0) *sleep -= SLEEP_GAIN;
            break;
        case QUIT:
            *status = 0;
            break;
    }
}

int count_neighbour(int drp[HEIGHT][WEIGHT], int x, int y) {
    int count = 0;
    for (int i = x - 1; i <= x + 1; i++)
        for (int j = y - 1; j <= y + 1; j++) {
            int x1 = (HEIGHT + i) % HEIGHT;
            int y1 = (WEIGHT + j) % WEIGHT;
            if (drp[x1][y1] == 1) count++;
        }
    if (drp[x][y] == 1) {
        count--;
    }
    return count;
}

int evolution(int drp[HEIGHT][WEIGHT], int *status) {
    int cells = 0;
    int next_drp[HEIGHT][WEIGHT];
    for (int i = 0; i < HEIGHT; i++)
        for (int j = 0; j < WEIGHT; j++) switch (drp[i][j]) {
                case 0: {
                    int live = count_neighbour(drp, i, j);
                    if (live == 3)
                        next_drp[i][j] = 1;
                    else
                        next_drp[i][j] = 0;
                    break;
                }
                case 1: {
                    int live = count_neighbour(drp, i, j);
                    if (live < 2 || live > 3)
                        next_drp[i][j] = 0;
                    else
                        next_drp[i][j] = 1;
                    cells++;
                    break;
                }
            }
    *status = copy_drp(drp, next_drp);
    return cells;
}

void init_drp(int drp[HEIGHT][WEIGHT]) {
    FILE *fptr = fopen("templates/cherepaha.txt", "r");
    if (fptr){
        for (int i = 0; i < HEIGHT; i++)
            for (int j = 0; j < WEIGHT; j++) 
                fscanf(fptr, "%d", &drp[i][j]);
    }
    else
        printf("File is not found\n");
    fclose(fptr);
}

int copy_drp(int drp[HEIGHT][WEIGHT], int new_drp[HEIGHT][WEIGHT]) {
    int same = 0;
    for (int i = 0; i < HEIGHT; i++)
        for (int j = 0; j < WEIGHT; j++) {
            if (drp[i][j] != new_drp[i][j]) same = 1;
            drp[i][j] = new_drp[i][j];
        }
    return same;
}

void output(int drp[HEIGHT][WEIGHT], int frame) {
    for (int i = 0; i < HEIGHT; i++) {;
        for (int j = 0; j < WEIGHT; j++) {
            switch (drp[i][j]) {
                case 1:
                    printw("$");
                    break;
                default:
                    printw(" ");
                    break;
            }
        }
        if ((i + 1) == HEIGHT) printw(" %d", frame);
        printw("\n");
    }
}

void print_end_game(int drp[HEIGHT][WEIGHT], int frame) {
    printf("\033c");
    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WEIGHT; j++) {
            switch (drp[i][j]) {
                case 1:
                    printf("$");
                    break;
                default:
                    printf(" ");
                    break;
            }
        }
        printf("\n");
    }
    printf("Game Over\t Frames: %d\n", frame);
}
