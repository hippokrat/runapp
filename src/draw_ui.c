#include "draw_ui.h"
#include <ncurses.h>
#include <signal.h>
#include <sys/stat.h>

static const char *byteconv(size_t bytes) 
{
	if(!bytes) 
		return NULL;
	const char *bytes_size[] = {
		"B", "KiB", "MiB", "GiB", "TiB",
		"PiB", "EiB"
	};
	double n = (double) bytes;
	static char buffer[32];
	size_t i;

	for(i = 0; i < 6 && n >= 1024; ++i) 
		n /= 1024;
	snprintf(buffer, sizeof(buffer), "%.2f %s", n, bytes_size[i]);
	return buffer;
}

static size_t file_size(const char *path) 
{
	struct stat sb;
	if(stat(path, &sb) == -1)
		return 0;
	return (size_t)sb.st_size;
}

static void redraw(Draw *cfg, const char *input, Find *res, int selected, int scroll) 
{
	size_t visible, idx;
	int row, listrow;

	if(cfg->mode == M_LONG || cfg->mode == M_SHORT) {
		move(0,0);
		clrtoeol();
		if(res->count > 0) {
			size_t sz = file_size(res->item[selected].path);
			mvprintw(0, 0, "%s (%s) [%zu matches]", res->item[selected].path,
				byteconv(sz), res->count);
		} else {
			mvprintw(0, 0, "no matches");
		}
		mvhline(1, 0, ACS_HLINE, 50);
	}

	row = (cfg->mode == M_LINE) ? 0 : 2;
	move(row, 0);
	clrtoeol();
	mvprintw(row, 0, ": %s", input);

	if(cfg->mode != M_LINE) {
	listrow = row + 1;
	visible = cfg->max_result;
	
		for(size_t i = 0; i < visible; ++i) {
			move(listrow + (int)i, 0);
			clrtoeol();

			idx = (size_t)scroll + i;
			if(idx >= res->count)
				continue;
			if((int)idx == selected)
				attron(A_REVERSE);
			if(cfg->mode == M_LONG) {
				size_t sz = file_size(res->item[idx].path);
				mvprintw(listrow + (int)i, 0, "%-30s %s", res->item[idx].name, 
						byteconv(sz));
			} else {
				mvprintw(listrow + (int)i, 0, "%s", res->item[idx].name);
			}
			if((int)idx == selected)
				attroff(A_REVERSE);
		}
	}
	move(row, 2 + (int)strlen(input));
	refresh();
}

char *draw(Draw *cfg)
{
	int 	selected, scroll, id, ch;
	char 	input[MAX_INPUT];
	char 	*chosen;
	Find 	res;


	memset(input, 0, sizeof(input));
	res.count  = 0;
	res.item   = NULL;
	selected   = 0;
	chosen     = NULL;
	scroll     = 0;
	id 		   = 0;
	ch  	   = 0;

	setenv("TERM", "xterm-256color", 1);
	signal(SIGTSTP, SIG_IGN);
	initscr();
	cbreak();
	noecho();
	keypad(stdscr, TRUE);
	napms(500); 
	flushinp();
	
	memset(input, 0, sizeof(input));
	redraw(cfg, input, &res, selected, scroll);
	
	for(;;) {
		ch = getch();
		if(ch == '~' || ch == 26)  /* 26 - ctrl+z */
    		continue;
		if(ch == 27) 
			break;
		if(ch == '\n') {
			if(res.count > 0)
				chosen = strdup(res.item[selected].path);
			break;
		}
		if(ch == KEY_UP) {
			if(selected > 0) {
				selected--;
				if(selected < scroll)
					scroll--;
			}
			redraw(cfg, input, &res, selected, scroll);
			continue;
		}
		if(ch == KEY_DOWN) {
			if(res.count > 0 && selected < (int)cfg->max_result) {
				selected++;
				if(selected >= scroll + (int)cfg->max_result) 
					scroll++;
				
			}
			redraw(cfg, input, &res, selected, scroll);
			continue;
		}
		if(ch == KEY_BACKSPACE || ch == 127) {
			if(id > 0) 
				input[--id] = '\0';
		} else if(id < MAX_INPUT - 1 && ch >= 32) {
			input[id++] = (char)ch;
		}
		selected = 0;
		scroll = 0;

		find_free(&res);
		
		if(id > 0)
			res = findfile(input, cfg->d);
		redraw(cfg, input, &res, selected, scroll);
	}
	find_free(&res);
	endwin();
	return chosen;
}
