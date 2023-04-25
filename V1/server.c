#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>

#define NUM_STUDENT_RECIPES 2
#define NUM_AZERI_RECIPES 2
#define NUM_FRENCH_RECIPES 2

// Define recipe arrays
char *student_recipes[] = {"Pizza", "Hamburger"};
char *azeri_recipes[] = {"Plov", "Dolma"};
char *french_recipes[] = {"Croissant", "Ratatouille"};

void handle_signal(int);

int main() {
    signal(SIGINT, handle_signal);
    signal(SIGQUIT, handle_signal);
    signal(SIGTERM, handle_signal);

    printf("Server waiting for signals...\n");

    while (1) {/*Wait for signals*/}

    return 0;
}

void handle_signal(int sig) {
    srand(time(NULL));  // Initialize random seed

    switch (sig) {
        case SIGINT:
            printf("Student recipe: %s\n", student_recipes[rand() % NUM_STUDENT_RECIPES]);
            break;
        case SIGQUIT:
            printf("Azeri recipe: %s\n", azeri_recipes[rand() % NUM_AZERI_RECIPES]);
            break;
        case SIGTERM:
            printf("French recipe: %s\n", french_recipes[rand() % NUM_FRENCH_RECIPES]);
            break;
        default:
            printf("Server received an unknown signal\n");
            break;
    }
}
