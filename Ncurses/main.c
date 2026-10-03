#include <dirent.h>
#include <errno.h>
#include <ncurses.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/stat.h>

#define MAX_NOTES 100
#define NAME_LEN 256
#define NOTES_DIR "notes"
#define PATH_LEN 1024

int compare_names(const void *a, const void *b);
void make_path(char *path, size_t size, const char *dir, const char *name);
void view_note(const char *path, const char *name);

int main(void) {
  const char *home = getenv("HOME");
  if (home == NULL) {
    home = ".";
  }
  char notes_dir[PATH_LEN];
  snprintf(notes_dir, sizeof(notes_dir), "%s/%s", home, NOTES_DIR);
  if (mkdir(notes_dir, 0700) != 0 && errno != EEXIST) {
    fprintf(stderr, "Failed to create directory: %s\n", strerror(errno));
    return EXIT_FAILURE;
  }
  initscr();
  cbreak(); // Instant Key Press
  noecho();
  keypad(stdscr, TRUE); // Turn on Keypresses
  int count = 0;
  int selected = 0;
  int list_top = 0;
  int ch = 0;
  char names[MAX_NOTES][NAME_LEN];
  while (ch != 'q') {
    clear();
    attron(A_BOLD); // Bold on
    printw("My Notes\n");
    attroff(A_BOLD); // Bold off
    printw("Note Menu: q:Quit | n:New | a:Add | d:Delete\n\n");
    DIR *dir = opendir(notes_dir);
    int i = 0;
    if (dir != NULL) {
      struct dirent *entry;
      while ((entry = readdir(dir)) != NULL) {
        if (entry->d_name[0] == '.') {
          continue;
        }
        if (i >= MAX_NOTES) {
          break;
        }
        snprintf(names[i], NAME_LEN, "%s", entry->d_name);
        i++;
      }
      closedir(dir);
    } else {
      printw("Error: Could Not Open Notes Folder!\nPress any key to exit");
      refresh();
      getch();
      break;
    }
    count = i;
    if (selected > (count - 1)) {
      selected = count - 1;
    }
    if (selected < 0) {
      selected = 0;
    }
    int rows = LINES - 3;
    if (selected < list_top) {
      list_top = selected;
    }
    if (selected >= list_top + rows) {
      list_top = selected - rows + 1;
    }
    qsort(names, count, NAME_LEN, compare_names);
    for (int n = list_top; n < count && n < list_top + rows; n++) {
      if (n == selected) {
        attron(A_REVERSE);
      }
      printw("%-*.*s", COLS, COLS, names[n]);
      if (n == selected) {
        attroff(A_REVERSE);
      }
    }
    if (count == 0) {
      printw("No Notes Yet!");
    }
    refresh();
    ch = getch();
    if (ch == 'n') {
      char new_name[NAME_LEN]; // Build new name instance
      clear();
      printw("New Note Name: ");
      refresh();
      echo();
      getnstr(new_name, NAME_LEN - 1); // Place user input into new name
      noecho();
      if (strchr(new_name, '/') != NULL) { // check for '/' in name
        printw("Error: No '/' in Note Names\nPress any key to return");
        refresh();
        getch();
      } else if (new_name[0] != '\0') { // if new name is not empty
        char path[PATH_LEN];
        make_path(path, sizeof(path), notes_dir, new_name);
        FILE *file = fopen(path, "a");
        if (file == NULL) {
          printw("Error: Could not create %s!\nPress any key to return",
                 new_name);
          refresh();
          getch();
        } else {
          fclose(file);
        }
      }
    }
    if (count > 0) {
      switch (ch) {
      case KEY_UP:
        if (selected > 0) {
          selected--;
        } else {
          selected = count - 1;
        }
        break;
      case KEY_DOWN:
        if (selected < count - 1) {
          selected++;
        } else {
          selected = 0;
        }
        break;
      case '\n':
      case KEY_ENTER: {
        char path[PATH_LEN];
        make_path(path, sizeof(path), notes_dir, names[selected]);
        view_note(path, names[selected]);
        break;
      }
      case 'a': {
        char path[PATH_LEN];
        make_path(path, sizeof(path), notes_dir, names[selected]);
        char text[512];
        clear();
        printw("Add to %s:\n", names[selected]);
        refresh();
        echo();
        getnstr(text, sizeof(text) - 1);
        noecho();
        if (text[0] != '\0') {
          FILE *file = fopen(path, "a");
          if (file != NULL) {
            fprintf(file, "%s\n", text);
            fclose(file);
          } else {
            clear();
            attron(A_BOLD);
            printw("Error: Could not write to %s | Press any key to return",
                   names[selected]);
            attroff(A_BOLD);
            refresh();
            getch();
          }
        }
        break;
      }
      case 'd': {
        char path[PATH_LEN];
        make_path(path, sizeof(path), notes_dir, names[selected]);
        clear();
        attron(A_BOLD);
        printw("Are you sure you want to DELETE %s?\nPress d again to remove "
               "it\nPress any key to return",
               names[selected]);
        attroff(A_BOLD);
        refresh();
        int del_ch = getch();
        if (del_ch == 'd') {
          if (remove(path) == 0) {
            clear();
            attron(A_BOLD);
            printw("Note Deleted | Press any key to return");
            attroff(A_BOLD);
            refresh();
            getch();
          } else {
            clear();
            attron(A_BOLD);
            printw("Error: Could not delete %s | Press any key to return",
                   names[selected]);
            attroff(A_BOLD);
            refresh();
            getch();
          }
        }
        break;
      }
      default:
        break;
      }
    }
  }
  endwin();
  return EXIT_SUCCESS;
}

int compare_names(const void *a, const void *b) {
  return strcasecmp((const char *)a, (const char *)b);
}

void make_path(char *path, size_t size, const char *dir, const char *name) {
  snprintf(path, size, "%s/%s", dir, name);
}

void view_note(const char *path, const char *name) {
  int top = 0;
  int key = 0;
  while (key != 'q') {
    FILE *file = fopen(path, "r");
    if (file == NULL) {
      clear();
      printw("Could not open %s\nPress any key to return", name);
      refresh();
      getch();
      return;
    }
    clear();
    attron(A_BOLD);
    printw("%s\n", name);
    attroff(A_BOLD);
    printw("Up/Down to scroll | q to go back\n\n");
    int rows = LINES - 3;
    int line_num = 0;
    int shown = 0;
    int more = 0;
    char line[512];
    while (fgets(line, sizeof(line), file) != NULL) {
      if (line_num >= top) {
        size_t len = strlen(line);
        if (len > 0 && line[len - 1] == '\n') {
          len--;
        }
        int needed = (int)(len / COLS) + 1;
        if (!more && shown + needed <= rows) {
          printw("%s", line);
          shown += needed;
        } else {
          more = 1;
        }
      }
      line_num++;
    }
    fclose(file);
    refresh();
    key = getch();
    if (key == KEY_UP && top > 0) {
      top--;
    }
    if (key == KEY_DOWN && more) {
      top++;
    }
    if (key == KEY_PPAGE && top > 0) {
      if (top > rows) {
        top = top - rows;
      } else {
        top = 0;
      }
    }
    if (key == KEY_NPAGE && more) {
      top = top + rows;
      if (top > line_num - rows) {
        top = line_num - rows;
      }
    }
  }
}
