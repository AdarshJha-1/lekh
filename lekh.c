/*** includes ***/

#include <unistd.h>
#include <stdio.h>
#include <termios.h>
#include <stdlib.h>
#include <ctype.h>
#include <errno.h>

/*** data ***/

struct termios og_termios; // original one to restore terminal back to it og form

/*** terminal ***/

void die(char const *s)
{
    perror(s);
    exit(EXIT_FAILURE);
}

void disable_raw_mode()
{
    if (tcsetattr(STDERR_FILENO, TCSAFLUSH, &og_termios) == -1)
    {
        die("tcsetattr");
    }
}

void enable_raw_mode()
{
    if (tcgetattr(STDIN_FILENO, &og_termios) == -1)
    {
        die("tcgetattr");
    }
    atexit(disable_raw_mode);
    struct termios raw = og_termios;
    raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
    raw.c_cflag |= (CS8);
    raw.c_oflag &= (OPOST);
    raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
    raw.c_cc[VMIN] = 0;
    raw.c_cc[VTIME] = 1;
    if (tcsetattr(STDERR_FILENO, TCSAFLUSH, &raw) == -1)
    {
        die("tcsetattr");
    }
}

/*** init ***/

int main()
{
    enable_raw_mode();
    while (1)
    {
        char c = '\0';
        if (read(STDIN_FILENO, &c, 1) == -1 && errno != EAGAIN)
        {
            die("read");
        }
        if (iscntrl(c))
        {
            printf("%d\r\n", c);
        }
        else
        {
            printf("%d ('%c')\r\n", c, c);
        }
        if (c == 'q')
        {
            break;
        }
    }
    printf("\nquit.");
    return 0;
}