#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <dirent.h>
#include <string.h>
#include <time.h>
#include <errno.h>

#define MAX_TEXT_SIZE 1024
#define MAX_FILENAME_LENGTH 512

typedef struct message {
    long mtype;
    char mtext[MAX_TEXT_SIZE];
} message;

int azeri_mq_id, french_mq_id, student_mq_id;

void send_files_to_queue(const char *folder, long mq_type) {
    DIR *dir;
    struct dirent *entry;

    // Open the directory
    dir = opendir(folder);
    if (dir == NULL) {
        perror("opendir");
        return;
    }

    // Get the message queue ID
    key_t key = ftok(".", 'q');
    int msqid = msgget(key, IPC_CREAT | 0666);
    if (msqid == -1) {
        perror("msgget");
        closedir(dir);
        return;
    }

    // Iterate over the files in the folder
    while ((entry = readdir(dir)) != NULL) {
        // Skip directories and files starting with '.'
        if (entry->d_type == DT_DIR || entry->d_name[0] == '.')
            continue;

        // Prepare the file path
        char filepath[MAX_FILENAME_LENGTH];
        snprintf(filepath, MAX_FILENAME_LENGTH, "%s/%s", folder, entry->d_name);

        // Read the file
        FILE *file = fopen(filepath, "r");
        if (file == NULL) {
            perror("fopen");
            continue;
        }

        // Read the file contents
        char filedata[MAX_FILENAME_LENGTH];
        if (fgets(filedata, MAX_FILENAME_LENGTH, file) == NULL) {
            fclose(file);
            continue;
        }
        fclose(file);

        // Prepare the message
        struct message msg;
        msg.mtype = mq_type;
        strncpy(msg.mtext, filedata, MAX_FILENAME_LENGTH);

        // Send the message to the queue
        if (msgsnd(msqid, &msg, sizeof(struct message) - sizeof(long), 0) == -1) {
            perror("msgsnd");
            continue;
        }

        printf("Sent file '%s' to message queue of type %ld\n", entry->d_name, mq_type);
    }

    closedir(dir);
}

void read_messages_from_queue(long mq_type) {
    // Get the message queue ID
    key_t key = ftok(".", 'q');
    int msqid = msgget(key, IPC_CREAT | 0666);
    if (msqid == -1) {
        perror("msgget");
        exit(1);
    }

    // Read messages of the specified type from the queue
    struct message msg;
    while (msgrcv(msqid, &msg, sizeof(struct message) - sizeof(long), mq_type, IPC_NOWAIT) != -1) {
        printf("Received message from message queue of type %ld:\n", mq_type);
        printf("%s\n", msg.mtext);
    }
}

void signal_handler(int sig) {
    int mq_id;
    int mq_type;
    char* folder_name;
    if (sig == SIGINT) {
        mq_id = azeri_mq_id;
        mq_type = 1;
        folder_name = "azeri";
    } else if (sig == SIGQUIT) {
        mq_id = french_mq_id;
        mq_type = 2;
        folder_name = "french";
    } else if (sig == SIGTERM) {
        mq_id = student_mq_id;
        mq_type = 3;
        folder_name = "student";
    } else {
        printf("Invalid signal received\n");
        return;
    }

    printf("Signal %d received for folder %s, message queue %d\n", sig, folder_name, mq_id);

    // Check if message queue is empty
    message msg;
    int ret = msgrcv(mq_id, &msg, MAX_TEXT_SIZE, mq_type, IPC_NOWAIT);
    if (ret >= 0) {
        printf("Received message: %s\n", msg.mtext);
    } else if (errno == ENOMSG) {
        // Message queue is empty, read files and write messages
        send_files_to_queue(folder_name, mq_type);

        printf("Wrote messages to message queue %d for folder %s\n", mq_id, folder_name);

        // Read message from message queue
        read_messages_from_queue(mq_type);
    } else {
        perror("msgrcv error");
    }
}

int main() {
    srand(time(NULL));

    // Create message queues
    azeri_mq_id = msgget(IPC_PRIVATE, IPC_CREAT | 0666);
    french_mq_id = msgget(IPC_PRIVATE, IPC_CREAT | 0666);
    student_mq_id = msgget(IPC_PRIVATE, IPC_CREAT | 0666);
    if (azeri_mq_id == -1 || french_mq_id == -1 || student_mq_id == -1) {
        perror("msgget error");
        exit(EXIT_FAILURE);
    }

    // Register signal handlers
    struct sigaction act;
    act.sa_handler = signal_handler;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;
    sigaction(SIGINT, &act, NULL);
    sigaction(SIGQUIT, &act, NULL);
    sigaction(SIGTERM, &act, NULL);

    while (1) {/*Wait for signals*/}

    // Destroy message queues
    if (msgctl(azeri_mq_id, IPC_RMID, NULL) == -1 ||
        msgctl(french_mq_id, IPC_RMID, NULL) == -1 ||
        msgctl(student_mq_id, IPC_RMID, NULL) == -1) {
        perror("msgctl error");
        exit(EXIT_FAILURE);
    }

    return 0;
}

