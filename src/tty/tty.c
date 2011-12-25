/* tty.c
 * 
 * Copyright (c) 2011 The tty project.
 *
 * This work is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by the Free
 * Software Foundation; either version 2.1 of the License, or (at your option)
 * any later version.
 * 
 * This work is distributed in the hope that it will be useful, but without 
 * any warranty; without even the implied warranty of merchantability or
 * fitness for a particular purpose. See the GNU Lesser General Public License
 * for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation,
 * Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 *
 *
 *  Created on: Dec 22, 2011
 *      Author: Franziskus Domig <fdomig@gmail.com>
 */

#include "tty.h"
#include <stdio.h>
#include <string.h>

// keep track of the current working directory
// start with the users $HOME directory
static char tty_cwd[MAX_PATH_LENGTH] = HOME_DIRECTORY;

// functions which may be moved to the ottos project
char *strtrim_right(char *p, char c) {
  char *end;
  int len;

  len = strlen(p);
  while (*p && len) {
    end = p + len - 1;
    if (c == *end) {
      *end = 0;
    } else {
      break;
    }
    len = strlen(p);
  }
  return (p);
}

// private functions
static void tty_username(char* buffer) {
  sprintf(buffer, "%s", USERNAME);
}

static void tty_hostname(char* buffer) {
  sprintf(buffer, "%s", HOSTNAME);
}

static void tty_print_prefix() {
  char username[16];
  char hostname[16];

  tty_username(username);
  tty_hostname(hostname);

  printf("%s@%s:%s%s ", username, hostname, tty_cwd, "#");
}

static BOOLEAN tty_find_binary(const char* name) {
  return FALSE;
}

// TODO (fdomig@gmail.com) move the CMDs to a separate file
static void tty_cmd_change_cwd(char* args) {
  // TODO (fdomig@gmail.com) ensure, the directory to change to exists

  // change to home directory if no path is given
  if (args == NULL) {
    sprintf(tty_cwd, "%s", HOME_DIRECTORY);
    return;
  }

  // remove trailing DIRECTORY_SEPARATOR
  if (strlen(args) > 1) {
    strtrim_right(args, DIRECTORY_SEPARATOR);
  }

  // change to a directory with an absolute path
  if (*args == DIRECTORY_SEPARATOR) {
    sprintf(tty_cwd, "%s", args);

    // change to parent directory or stay in current directory
  } else if (*args == '.') {
    // change to parent directory
    if (*(args + 1) == '.' && strlen(tty_cwd) > 1) {
      int i = strlen(tty_cwd);
      while (tty_cwd[--i] != DIRECTORY_SEPARATOR);
      if (i > 0) {
        tty_cwd[i] = '\0';
      } else {
        tty_cwd[i+1] = '\0';
      }
    }

    // change to a child directory
  } else {
    strtrim_right(tty_cwd, DIRECTORY_SEPARATOR);
    sprintf(tty_cwd, "%s%c%s", tty_cwd, DIRECTORY_SEPARATOR, args);
  }
}

void tty_run() {
  while (TRUE) {
    // XXX: how do we ensure, we do not read more than 1025 characters?
    char line[1024 + 1];
    char* tokens;
    char* cmd;

    // print prefix
    tty_print_prefix();

    // read one line
    int rc = scanf("%1024[^\n]%*[^\n]", line);
    if (rc == EOF) {
      // TODO (fdomig@gmail.com) line was already at EOF while trying to get
      // the first character
    } else if (rc == 0) {
      // TODO (fdomig@gmail.com) the user entered an empty line of code
    }

    if (!feof(stdin) && !ferror(stdin)) getchar();

    // split input string into tokens
    tokens = strtok(line, SPLIT_CHARS);
    cmd = tokens;
    tokens = strtok(NULL, SPLIT_CHARS);

    // check for a built in command
    // TODO (fdomig@gmail.com) check for built in command
    if (strcmp(cmd, "cd") == 0) {
      tty_cmd_change_cwd(tokens);

      // finally, is there a application with the entered name?
    } else if (!tty_find_binary(cmd)) {
      printf("-%s: %s command not found\n", TTY_NAME, line);
      continue;
    }

    // TODO (fdomig@gmail.com) start the found application
  }
}
