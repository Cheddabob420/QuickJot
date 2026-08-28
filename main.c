#include <asm-generic/errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <time.h>

#define FILE_PATH "maintlog.txt"

int main(int argc, char *argv[]) {
  /*** Variables ***/
  int choice = 0;
  char *note = NULL;
  size_t size = 0;
  char time_buf[64];
  time_t now = time(NULL);

  /*** Stucts ***/
  struct tm *t_info = localtime(&now);
  strftime(time_buf, sizeof(time_buf), "[%Y-%m-%d %H:%M:%S]", t_info);

  /*** Append CLI Args  ***/
  if (argv[1] != NULL) {
    FILE *arg_p = fopen(FILE_PATH, "a");
    if (arg_p == NULL) {
      perror("Error initializing log file");
      return EXIT_FAILURE;
    } else {
      fprintf(arg_p, "%s %s\n", time_buf, argv[1]);
      fclose(arg_p);
    }
  }

  /*** Log File Check ***/
  FILE *f_check = fopen(FILE_PATH, "r");
  if (f_check == NULL) {
    FILE *f_create = fopen(FILE_PATH, "a");
    if (f_create == NULL) {
      perror("Error initializing log file!");
      return EXIT_FAILURE;
    }
    printf("Log File Not Found! Created New Log File At %s\n!", FILE_PATH);
    fclose(f_create);
  } else {
    printf("Log File Found! Loading Prev Logs...\n");
    fclose(f_check);
  }

  /*** Start Menu ***/
  do {
    printf("##### QuckJot #####\n");
    printf("1)Add Note\n2)View Shift Log\n3)Exit\n> ");
    scanf("%d", &choice);
    int c = 0;
    while ((c = getchar()) != '\n' && c != EOF)
      ;

    /*** Main Logic ***/
    switch (choice) {
    case 1: { // Add Note
      FILE *f_ptr = fopen(FILE_PATH, "a");
      if (f_ptr == NULL) {
        printf("Error: Could not find note file\n");
        return EXIT_FAILURE;
      }
      printf("Enter Completed Task: \n> ");
      ssize_t readc = getline(&note, &size, stdin);
      if (readc != -1) {
        fprintf(f_ptr, "%s %s", time_buf, note);
        printf("Successfully Wrote Task!\n");
      } else {
        printf("Error Reading Input!\n");
      }
      free(note);
      note = NULL;
      fclose(f_ptr);

      break;
    }
    case 2: { // Veiw Log
      FILE *f_ptr = fopen(FILE_PATH, "r");
      if (f_ptr == NULL) {
        printf("Error: Could not find note file\n");
        return EXIT_FAILURE;
      } else {
        char buffer[1024];
        while (fgets(buffer, sizeof(buffer), f_ptr) != NULL) {
          printf("%s", buffer);
        }
      }
      fclose(f_ptr);
      f_ptr = NULL;
      break;
    }
    case 3: { // Exit
      printf("Goodbye!\n");
      break;
    }
    default: {
      printf("Invaild Choice, Try Again!\n");
      break;
    }
    }
    printf("\n");
  } while (choice != 3);
  note = NULL;
  return EXIT_SUCCESS;
}
