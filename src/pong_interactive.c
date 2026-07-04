#include <ncurses.h>

#define FIELD_WIDTH 80
#define FIELD_HEIGHT 25
#define LEFT_PADDLE_X 2
#define RIGHT_PADDLE_X 77
#define PADDLE_HEIGHT 3
#define START_PADDLE_Y 11
#define START_BALL_X 40
#define START_BALL_Y 12
#define WIN_SCORE 21
#define FRAME_DELAY 70

static int move_paddle(int paddle_y, int action, int up_key, int down_key);
static int vertical_direction(int ball_y, int direction_y);
static int horizontal_direction(int ball_x, int ball_y, int direction_x, int left_y, int right_y);
static char field_symbol(int x, int y, int ball_x, int ball_y, int left_y, int right_y);
static void draw_field(int ball_x, int ball_y, int left_y, int right_y, int left_score, int right_score);
static void show_winner(int left_score);

int main(void) {
    int left_y = START_PADDLE_Y;
    int right_y = START_PADDLE_Y;
    int ball_x = START_BALL_X;
    int ball_y = START_BALL_Y;
    int direction_x = 1;
    int direction_y = 1;
    int left_score = 0;
    int right_score = 0;

    initscr();
    noecho();
    curs_set(0);
    timeout(0);
    draw_field(ball_x, ball_y, left_y, right_y, left_score, right_score);
    while (left_score < WIN_SCORE && right_score < WIN_SCORE) {
        int action = getch();
        left_y = move_paddle(left_y, action, 'a', 'z');
        right_y = move_paddle(right_y, action, 'k', 'm');
        direction_y = vertical_direction(ball_y, direction_y);
        ball_y += direction_y;
        direction_x = horizontal_direction(ball_x, ball_y, direction_x, left_y, right_y);
        ball_x += direction_x;
        if (ball_x == 0) {
            right_score++;
        } else if (ball_x == FIELD_WIDTH - 1) {
            left_score++;
        }
        if (ball_x == 0 || ball_x == FIELD_WIDTH - 1) {
            ball_x = START_BALL_X;
            ball_y = START_BALL_Y;
            direction_x = (left_score + right_score) % 2 == 0 ? 1 : -1;
            direction_y = (left_score + right_score) % 2 == 0 ? -1 : 1;
        }
        draw_field(ball_x, ball_y, left_y, right_y, left_score, right_score);
        napms(FRAME_DELAY);
    }
    show_winner(left_score);
    endwin();
    return 0;
}

static int move_paddle(int paddle_y, int action, int up_key, int down_key) {
    if ((action == up_key || action == up_key - 'a' + 'A') && paddle_y > 1) {
        paddle_y--;
    } else if ((action == down_key || action == down_key - 'a' + 'A') &&
               paddle_y + PADDLE_HEIGHT < FIELD_HEIGHT - 1) {
        paddle_y++;
    }
    return paddle_y;
}

static int vertical_direction(int ball_y, int direction_y) {
    if (ball_y + direction_y == 0 || ball_y + direction_y == FIELD_HEIGHT - 1) {
        direction_y = -direction_y;
    }
    return direction_y;
}

static int horizontal_direction(int ball_x, int ball_y, int direction_x, int left_y, int right_y) {
    int next_x = ball_x + direction_x;
    int hits_left =
        direction_x < 0 && next_x == LEFT_PADDLE_X && ball_y >= left_y && ball_y < left_y + PADDLE_HEIGHT;
    int hits_right =
        direction_x > 0 && next_x == RIGHT_PADDLE_X && ball_y >= right_y && ball_y < right_y + PADDLE_HEIGHT;
    if (hits_left || hits_right) {
        direction_x = -direction_x;
    }
    return direction_x;
}

static char field_symbol(int x, int y, int ball_x, int ball_y, int left_y, int right_y) {
    char symbol = ' ';
    if (y == 0 || y == FIELD_HEIGHT - 1) {
        symbol = '-';
    } else if (x == 0 || x == FIELD_WIDTH - 1) {
        symbol = '|';
    } else if (x == ball_x && y == ball_y) {
        symbol = 'o';
    } else if ((x == LEFT_PADDLE_X && y >= left_y && y < left_y + PADDLE_HEIGHT) ||
               (x == RIGHT_PADDLE_X && y >= right_y && y < right_y + PADDLE_HEIGHT)) {
        symbol = '|';
    } else if (x == FIELD_WIDTH / 2 && y % 2 == 0) {
        symbol = ':';
    }
    return symbol;
}

static void draw_field(int ball_x, int ball_y, int left_y, int right_y, int left_score, int right_score) {
    erase();
    mvprintw(0, 0, "Score: %d - %d", left_score, right_score);
    for (int y = 0; y < FIELD_HEIGHT; y++) {
        for (int x = 0; x < FIELD_WIDTH; x++) {
            mvaddch(y + 1, x, field_symbol(x, y, ball_x, ball_y, left_y, right_y));
        }
    }
    refresh();
}

static void show_winner(int left_score) {
    erase();
    if (left_score == WIN_SCORE) {
        mvprintw(0, 0, "Congratulations! The left player wins!");
    } else {
        mvprintw(0, 0, "Congratulations! The right player wins!");
    }
    refresh();
    napms(2000);
}
