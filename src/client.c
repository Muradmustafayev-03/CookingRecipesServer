#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>
#include <unistd.h>

void send_signal(int signal, int pid) {
    if (kill(pid, signal) == -1) {
        printf("Error: Could not send signal.\n");
        exit(EXIT_FAILURE);
    }
}

int main(int argc, char* argv[]) {
    int server_pid;

    if (argc != 2) {
        printf("Usage: %s <server pid>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    server_pid = atoi(argv[1]);

    srand(time(NULL));
    for (int i = 0; i < 100; i++) {
        int signal = (rand() % 3 == 0) ? SIGINT : (rand() % 3 == 1) ? SIGQUIT : SIGTERM;
        send_signal(signal, server_pid);
        sleep(1);
    }

    return 0;
}
