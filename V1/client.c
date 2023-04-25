#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>
#include <unistd.h>

#define NUM_SIGNALS 100
#define SLEEP_TIME 1

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s <pid>\n", argv[0]);
        return 1;
    }

    pid_t pid = atoi(argv[1]);

    srand(time(NULL));  // Initialize random seed

    for (int i = 0; i < NUM_SIGNALS; i++) {
        int signal_num = rand() % 3;  // Generate a random number between 0 and 2

        switch (signal_num) {
            case 0:
                if (kill(pid, SIGINT) == -1) {
                    perror("kill");
                    return 1;
                }
                // printf("Sent SIGINT signal to server with PID %d\n", pid);
                break;
            case 1:
                if (kill(pid, SIGQUIT) == -1) {
                    perror("kill");
                    return 1;
                }
                // printf("Sent SIGQUIT signal to server with PID %d\n", pid);
                break;
            case 2:
                if (kill(pid, SIGTERM) == -1) {
                    perror("kill");
                    return 1;
                }
                // printf("Sent SIGTERM signal to server with PID %d\n", pid);
                break;
            default:
                printf("Invalid signal number\n");
                return 1;
        }
        sleep(SLEEP_TIME); // sleep 1 second
    }

    return 0;
}
