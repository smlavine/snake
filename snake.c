/*
 * snake - A simple, terminal snake game
 * Copyright (C) 2021 Sebastian LaVine <mail@smlavine.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>. 
 */

#include <assert.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "rogueutil.h"

#ifndef WIDTH
#define WIDTH 40
#endif /* WIDTH */

#ifndef HEIGHT
#define HEIGHT 40
#endif /* HEIGHT */

#define SNAKE_CHR '#'
#define BERRY_CHR 'B'

typedef struct {
	int x;
	int y;
} Point;

int cols, rows;
int xv, yv; /* X and Y velocities */
Point snake[WIDTH*HEIGHT]; /* snake[0] is the "head" of the snake. */
int snakelen;
Point berries[(WIDTH + HEIGHT)/8]; /* 1/4 the average of WIDTH and HEIGHT. */
int berrieslen = (WIDTH + HEIGHT)/8;

void down(void);
void draw_border(void);
void draw_snake(char);
void left(void);
void quit(int);
void right(void);
void up(void);

/* Change the snake's direction to down. */
void
down(void)
{
	xv = 0;
	yv = 1;
}

/* Draw a border around the parts of the play field that are open to the rest
 * of the terminal.
 */
void
draw_border(void)
{
	int i;
	char bottom[WIDTH + 1];

	gotoxy(1, HEIGHT + 1);
	memset(bottom, '-', WIDTH);
	bottom[WIDTH] = '+';
	setString(bottom);
	for (i = 1; i <= HEIGHT; i++) {
		gotoxy(WIDTH + 1, i);
		setChar('|');
	}

	fflush(stdout);
}

void
draw_snake(char ch) {
	int i;

	for (i = 0; i < snakelen; i++) {
		gotoxy(snake[i].x, snake[i].y);
		setChar(ch);
	}
	fflush(stdout);
}

/* Change the snake's direction to left. */
void
left(void)
{
	xv = -1;
	yv = 0;
}

/*
 * Cleans up terminal settings and the like, and quits snake with the given
 * exit code.
 */
void
quit(int code)
{
	system("stty sane");
	showcursor();
	gotoxy(1, HEIGHT + 1);
	/* Return the shell prompt to the beginning of the next line. */
	putchar('\n');
	fflush(stdout);
	switch (code) {
	case SIGINT:
	case SIGTERM:
	case SIGQUIT:
		exit(EXIT_SUCCESS);
		break;
	case SIGSEGV:
	case SIGHUP:
		exit(EXIT_FAILURE);
		break;
	default:
		exit(code);
		break;
	}
}

/* Change the snake's direction to right. */
void
right(void)
{
	xv = 1;
	yv = 0;
}

/* Change the snake's direction to up. */
void
up(void)
{
	xv = 0;
	yv = -1;
}

int
main(int argc, char *argv[])
{
	int i;
	Point tailend;

	signal(SIGINT, quit);
	signal(SIGTERM, quit);
	signal(SIGQUIT, quit);
	signal(SIGSEGV, quit);
	signal(SIGHUP, quit);

	cols = tcols(), rows = trows();
	assert(0 < cols && 0 < rows);
	assert(5 <= WIDTH && 5 <= HEIGHT);

	xv = 1, yv = 0;

	snake[0].x = WIDTH / 2;
	snake[0].y = HEIGHT / 2;
	snakelen = 5;

	system("stty -echo");
	hidecursor();
	cls();

	draw_border();
	draw_snake(SNAKE_CHR);

	for (;;) {

		switch (nb_getch()) {
		case '\033':
			/* Pressing an arrow key sends the sequence of
			 * ^[ (escape, '\033')
			 * [
			 * A B C D (for up, down, right, or left, respectively)
			 */

			/* Using blocking getch here also effectively pauses
			 * the game when 'ESC' is pressed on its own.
			 */
			if (getch() != '[') break;
			switch (getch()) {
			case 'A':
				up();
				break;
			case 'B':
				down();
				break;
			case 'C':
				right();
				break;
			case 'D':
				left();
				break;
			}
			break;
		case 'w':
		case 'k':
			up();
			break;
		case 's':
		case 'j':
			down();
			break;
		case 'd':
		case 'l':
			right();
			break;
		case 'a':
		case 'h':
			left();
			break;
		case 'q':
			quit(EXIT_SUCCESS);
			break;
		}

		tailend = snake[snakelen - 1];

		for (i = snakelen - 1; 0 < i; i--) {
			snake[i] = snake[i - 1];
		}
		snake[0].x += xv;
		snake[0].y += yv;
		
		if (WIDTH < snake[0].x || HEIGHT < snake[0].y ||
				snake[0].x < 1 || snake[0].y < 1)
			break;

		gotoxy(snake[0].x, snake[0].y);
		setChar(SNAKE_CHR);
		fflush(stdout);

		gotoxy(tailend.x, tailend.y);
		setChar(' ');
		fflush(stdout);

		msleep(100);
	}
	
	quit(EXIT_SUCCESS);
	return 0;
}
