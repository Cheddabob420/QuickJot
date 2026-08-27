#include <asm-generic/errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>

#define FILE_PATH "maintlog.txt"

int main(void) {
  int choice = 0;
  char *note = NULL;
  size_t size = 0;
  printf("##### QuckJot #####\n");
  printf("1)Add Note\n2)View Shift Log\n3)Exit\n> ");
  scanf("%d", &choice);
  int c = 0;
  while ((c = getchar()) != '\n' && c != EOF)
    ;
  switch (choice) {
  case 1: {
    FILE *f_ptr = fopen(FILE_PATH, "a");
    if (f_ptr == NULL) {
      printf("Error: Could not find note file\n");
      return EXIT_FAILURE;
    }
    printf("Enter Completed Task: \n> ");
    ssize_t readc = getline(&note, &size, stdin);
    if (readc != -1) {
      fprintf(f_ptr, "%s", note);
      printf("Successfully Wrote Task!\n");
    } else {
      printf("Error Reading Input!\n");
    }
    free(note);
    note = NULL;
    fclose(f_ptr);
    break;
  }
  case 2: {
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
  case 3: {
    printf("Goodbye!\n");
    break;
  }
  }
  note = NULL;
  return EXIT_SUCCESS;
}
