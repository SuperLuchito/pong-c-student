#include <ncurses.h>

#define BOARD_WIDTH 80
#define BOARD_HEIGHT 25
#define LEFT_RACKET_X 2
#define RIGHT_RACKET_X 77
#define RACKET_SIZE 3
#define RACKET_START 11
#define BALL_START_X 40
#define BALL_START_Y 12
#define SCORE_LIMIT 21
#define TICK_TIME 70
#define WINNER_TIME 5000

static int shift_racket(int top, int command, int up, int down) {
    if ((command == up || command == up - 'a' + 'A') && top > 1) {
        top--;
    } else if ((command == down || command == down - 'a' + 'A') && top + RACKET_SIZE < BOARD_HEIGHT - 1) {
        top++;
    }
    return top;
}

static int reflect_from_wall(int ball_y, int speed_y) {
    int next_y = ball_y + speed_y;
    if (next_y == 0 || next_y == BOARD_HEIGHT - 1) {
        speed_y = -speed_y;
    }
    return speed_y;
}

static int racket_contains(int ball_y, int racket_top) {
    int result = 0;
    if (ball_y >= racket_top && ball_y < racket_top + RACKET_SIZE) {
        result = 1;
    }
    return result;
}

static int reflect_from_racket(int ball_x, int ball_y, int speed_x, int left_top, int right_top) {
    int next_x = ball_x + speed_x;
    int left_hit = speed_x < 0 && next_x == LEFT_RACKET_X && racket_contains(ball_y, left_top);
    int right_hit = speed_x > 0 && next_x == RIGHT_RACKET_X && racket_contains(ball_y, right_top);
    if (left_hit || right_hit) {
        speed_x = -speed_x;
    }
    return speed_x;
}

static char board_character(int x, int y, int ball_x, int ball_y, int left_top, int right_top) {
    char character = ' ';
    if (y == 0 || y == BOARD_HEIGHT - 1) {
        character = '-';
    } else if (x == 0 || x == BOARD_WIDTH - 1) {
        character = '|';
    } else if (x == ball_x && y == ball_y) {
        character = '*';
    } else if ((x == LEFT_RACKET_X && racket_contains(y, left_top)) ||
               (x == RIGHT_RACKET_X && racket_contains(y, right_top))) {
        character = '|';
    }
    return character;
}

static void paint_board(int ball_x, int ball_y, int left_top, int right_top, int left_points,
                        int right_points) {
    erase();
    mvprintw(0, 0, "Score: %d - %d", left_points, right_points);
    for (int row = 0; row < BOARD_HEIGHT; row++) {
        for (int column = 0; column < BOARD_WIDTH; column++) {
            mvaddch(row + 1, column, board_character(column, row, ball_x, ball_y, left_top, right_top));
        }
    }
    refresh();
}

static void show_victor(int left_points) {
    erase();
    if (left_points == SCORE_LIMIT) {
        mvprintw(0, 0, "Congratulations! The left player wins!");
    } else {
        mvprintw(0, 0, "Congratulations! The right player wins!");
    }
    refresh();
    napms(WINNER_TIME);
}

int main(void) {
    int left_top = RACKET_START;
    int right_top = RACKET_START;
    int ball_x = BALL_START_X;
    int ball_y = BALL_START_Y;
    int speed_x = 1;
    int speed_y = 1;
    int left_points = 0;
    int right_points = 0;

    initscr();
    noecho();
    curs_set(0);
    timeout(0);
    while (left_points < SCORE_LIMIT && right_points < SCORE_LIMIT) {
        int command = getch();
        left_top = shift_racket(left_top, command, 'a', 'z');
        right_top = shift_racket(right_top, command, 'k', 'm');
        speed_y = reflect_from_wall(ball_y, speed_y);
        ball_y += speed_y;
        speed_x = reflect_from_racket(ball_x, ball_y, speed_x, left_top, right_top);
        ball_x += speed_x;
        if (ball_x == 0) {
            right_points++;
        } else if (ball_x == BOARD_WIDTH - 1) {
            left_points++;
        }
        if (ball_x == 0 || ball_x == BOARD_WIDTH - 1) {
            ball_x = BALL_START_X;
            ball_y = BALL_START_Y;
            speed_x = -speed_x;
            speed_y = -speed_y;
        }
        paint_board(ball_x, ball_y, left_top, right_top, left_points, right_points);
        napms(TICK_TIME);
    }
    show_victor(left_points);
    endwin();
    return 0;
}
