/**
 * Flappy Curses
 *
 * This is a really simple program for me to learn how to use the
 * ncurses library.
 *
 * NOTES TO PROGRAMMERS:
 *   Gravity is reverse! The bird naturally "falls" towards the ceiling
 **/

#include <ncurses.h>
#include <string.h>
#include <stdlib.h>

#define ESC_KEY          27

#define PIPE_X_GAP       30  // Not actually the value used... look at the code
#define PIPE_Y_GAP       10  // Is the actual value between pipe pairs
#define PIPE_WIDTH       10
#define GRAVITY          1.0
#define SPEED            60 // Time in ms it will wait for your input
#define FLAPPY_X_OFFSET  20

struct pipe_pair
{
    WINDOW *top_window;
    WINDOW *bottom_window;
    int pos_y;     // Center of the gap of the pipes
    int pos_x;     // Position on the x-axis
};

struct bird
{
    WINDOW *window;
    int height;
    int score;
    int distance;
    float upward_speed;
};


void welcome_message(int starty, int startx);
void death_message(int starty, int startx, unsigned int score);
WINDOW *draw_flappy_bird(int flappy_height, WINDOW *flappy_win);
void init_pipes(struct pipe_pair *pipes, unsigned int num_pipes,
                int lines, int coles);
int  draw_pipes(struct pipe_pair *pipes, unsigned int num_pipes,
                int lines, int cols);

int main(void)
{
    char ch;
    unsigned int num_pipes;
    struct pipe_pair *pipes;
    struct bird flappy;

    initscr(); cbreak(); noecho(); // Just the usual
    curs_set(0);  // Disables displaying the cursor
    keypad(stdscr, TRUE);

    num_pipes = COLS/PIPE_X_GAP;

    pipes = (struct pipe_pair *) malloc(sizeof(struct pipe_pair) * num_pipes);

    init_pipes(pipes, num_pipes, LINES, COLS);

    flappy.height = LINES/2;
    flappy.upward_speed = 0;
    flappy.distance = 0;
    flappy.score = 0;
    flappy.window = draw_flappy_bird(flappy.height, NULL);
    draw_pipes(pipes, num_pipes, LINES, COLS);

    refresh();

    welcome_message(LINES/2, COLS/2);

    timeout(SPEED);

    while((ch = getch()) != ESC_KEY)
    {
        if (ch != ERR)
            flappy.upward_speed = 3.0;

        flappy.upward_speed -= GRAVITY;

        if (flappy.upward_speed <= -5.0)
            flappy.upward_speed = -5.0;

        flappy.height -= flappy.upward_speed;

        flappy.window = draw_flappy_bird(flappy.height, flappy.window);

        if (flappy.height <= 0 || flappy.height >= LINES)
        {
            death_message(LINES/2, COLS/2, flappy.score);
            break;
        }
        flappy.score += draw_pipes(pipes, num_pipes, LINES, COLS);
    }
    timeout(0);

    endwin();
    return 0;
}

/* Would like a semi-open interval [min, max) */
/* Stolen from https://stackoverflow.com/questions/2509679/how-to-generate-a-random-number-from-within-a-range#6852396 */
int random_in_range (unsigned int min, unsigned int max)
{
    int base_random = rand(); /* in [0, RAND_MAX] */
    if (RAND_MAX == base_random) return random_in_range(min, max);

    /* now guaranteed to be in [0, RAND_MAX) */
    int range       = max - min,
        remainder   = RAND_MAX % range,
        bucket      = RAND_MAX / range;

    /* There are range buckets, plus one smaller interval
       within remainder of RAND_MAX */
    if (base_random < RAND_MAX - remainder)
    {
        return min + base_random/bucket;
    }
    else
    {
        return random_in_range (min, max);
    }
}

/* I know draw_pipes shouldn't have anything to do with scoring, it
 *   just makes it so much easier. Screw MVC.
 */
int draw_pipes(struct pipe_pair *pipes, unsigned int num_pipes,
                int lines, int cols)
{
    int i;
    int passed = 0;
    for (i = 0; i < num_pipes; i++)
    {
        werase(pipes[i].top_window);
        werase(pipes[i].bottom_window);
        wrefresh(pipes[i].top_window);
        wrefresh(pipes[i].bottom_window);
        delwin(pipes[i].top_window);
        delwin(pipes[i].bottom_window);

        pipes[i].pos_x -= 1;
        if (pipes[i].pos_x == 19)
        {
            passed = 1;
        }

        if (pipes[i].pos_x <= 0)
        {
            pipes[i].pos_x = cols + PIPE_X_GAP;
            pipes[i].pos_y = random_in_range(10, lines - 10);
        }
        pipes[i].top_window = newwin(
            pipes[i].pos_y - PIPE_Y_GAP/2,
            PIPE_WIDTH,
            0,
            pipes[i].pos_x - PIPE_WIDTH/2
        );

        pipes[i].bottom_window = newwin(
            lines - pipes[i].pos_y + PIPE_Y_GAP/2,
            PIPE_WIDTH,
            pipes[i].pos_y + PIPE_Y_GAP/2,
            pipes[i].pos_x - PIPE_WIDTH/2
        );
        box(pipes[i].top_window, 0, 0);
        box(pipes[i].bottom_window, 0, 0);
        wrefresh(pipes[i].top_window);
        wrefresh(pipes[i].bottom_window);
    }
    return passed;
}

void init_pipes(struct pipe_pair *pipes, unsigned int num_pipes,
                int lines, int cols)
{
    int i;
    for (i = 0; i < num_pipes; i++)
    {
        pipes[i].pos_x = cols/2 + (PIPE_X_GAP * i);
        pipes[i].pos_y = random_in_range(10, lines - 10);
        pipes[i].top_window = newwin(
            pipes[i].pos_y - PIPE_Y_GAP/2,
            PIPE_WIDTH,
            pipes[i].pos_y - (pipes[i].pos_y - PIPE_Y_GAP/2)/2,
            pipes[i].pos_x - PIPE_WIDTH/2
        );
        pipes[i].bottom_window = newwin(
            lines - pipes[i].pos_y - PIPE_Y_GAP/2,
            PIPE_WIDTH,
            pipes[i].pos_y - (pipes[i].pos_y - PIPE_Y_GAP/2)/2,
            pipes[i].pos_x - PIPE_WIDTH/2
        );
    }
}


WINDOW *draw_flappy_bird(int flappy_height, WINDOW *flappy_win)
{
    if (flappy_win != NULL)
    {
        werase(flappy_win);
        wrefresh(flappy_win);
        delwin(flappy_win);
    }
    flappy_win = newwin(3, 3, flappy_height, FLAPPY_X_OFFSET);
    mvwprintw(flappy_win, 0, 0, " A ");
    mvwprintw(flappy_win, 1, 0, "BRD");
    wrefresh(flappy_win);
    return flappy_win;
}

void death_message(int starty, int startx, unsigned int score)
{
    WINDOW *death_win;
    char *death_message[] = {
        "                 You have died              ",
        "               Your score was ",
      //"Press ESC to quit or any button to try again",
        "       Thanks for playing! ESC to quit      "
    };
    int width, height;

    timeout(-1);

    // Ugly compile time string lengths
    width = sizeof("Press ESC to quit or any button to try again") / sizeof(char);
    height = sizeof(death_message) / sizeof(death_message[0]);

    death_win = newwin(height + 2,
                       width + 1,
                       starty-(height/2),
                       startx-(width/2));

    mvwprintw(death_win, 1, 1, "%s", death_message[0]);
    mvwprintw(death_win, 2, 1, "%s", death_message[1]);
    wprintw(death_win, "%u", score);
    mvwprintw(death_win, 3, 1, "%s", death_message[2]);
    box(death_win, 0 , 0);
    wrefresh(death_win);

    while(getch() != ESC_KEY);

    werase(death_win);
    wrefresh(death_win);
    delwin(death_win);
}


void welcome_message(int starty, int startx)
{
    WINDOW *welcome_win;
    char *welcome_message[] = {
        "       Welcome to Flappy Curses",
        "Escape to quit and any other key to jump",
        "        Press any key to begin",
    };
    int width, height;

    // Ugly compile time string lengths
    width = sizeof("Escape to quit and any other key to jump") / sizeof(char);
    height = sizeof(welcome_message) / sizeof(welcome_message[0]);

    welcome_win = newwin(height + 2,
                         width + 1,
                         starty-(height/2),
                         startx-(width/2));
    mvwprintw(welcome_win, 1, 1, "%s", welcome_message[0]);
    mvwprintw(welcome_win, 2, 1, "%s", welcome_message[1]);
    mvwprintw(welcome_win, 3, 1, "%s", welcome_message[2]);
    box(welcome_win, 0 , 0);
    wrefresh(welcome_win);

    getch();

    werase(welcome_win);
    wrefresh(welcome_win);
    delwin(welcome_win);
}
