#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <libpq-fe.h>

#define MAX_MESSAGE_LENGTH 17
#define MAX_QUEUE_LENGTH 10
#define CONSUMPTION_RATE 2

// Struct for a message
typedef struct {
    int id;
    char data[MAX_MESSAGE_LENGTH];
} Message;

// Parameters for the producer and consumer
typedef struct {
    float message_delay;
    int consumption_delay;
    int threshold;
    int max_queue_length;
} Parameters;

// The message queue
Message *messageQueue;
int queue_length = 0;
int next_message_id = 0;

// Synchronization constructs
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
sem_t items;
sem_t spaces;


// Global variable for the database connection
PGconn *global_conn = NULL;

// Function to initialize connection to PostgreSQL
void initializePostgresConnection(const char *host, const char *dbname, const char *user, const char *password) {
    // Create a connection to the database
    global_conn = PQsetdbLogin(host, "5432", NULL, NULL, dbname, user, password);

    // Check for connection success
    if (PQstatus(global_conn) != CONNECTION_OK) {
        fprintf(stderr, "Connection to database failed: %s", PQerrorMessage(global_conn));
        PQfinish(global_conn);
        exit(EXIT_FAILURE);
    }
}

void closePostgresConnection() {
    if (global_conn != NULL) {
        PQfinish(global_conn);
        global_conn = NULL;
    }
}

// Modified function to insert timestamped data using global connection
void insertDataToPostgres(const char *table, double value) {
    // Ensure global connection is initialized
    if (!global_conn) {
        fprintf(stderr, "Global database connection is not initialized.\n");
        exit(EXIT_FAILURE);
    }

    // Get the current timestamp
    time_t current_time;
    time(&current_time);

    // Construct the SQL query
    char query[200];
    snprintf(query, sizeof(query), "INSERT INTO %s (timestamp, metric_value) VALUES (to_timestamp(%ld), %f)", table,
             current_time, value);

    // Execute the query
    PGresult *res = PQexec(global_conn, query);

    // Check for query execution success
    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        fprintf(stderr, "Query execution failed: %s", PQerrorMessage(global_conn));
        PQclear(res);
        exit(EXIT_FAILURE);
    }

    // Clean up
    PQclear(res);
}

void printUsage(char *programName) {
    fprintf(stderr,
            "Usage: %s [--message-delay <value>] [--consumption-delay <value>] [--threshold <value>] [--max-queue-length <value>]\n",
            programName);
}

int parseParameters(int argc, char *argv[], Parameters *params) {
    params->message_delay = -1;
    params->consumption_delay = CONSUMPTION_RATE;
    params->threshold = -1;
    params->max_queue_length = MAX_QUEUE_LENGTH;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--message-delay") == 0 || strcmp(argv[i], "-md") == 0) {
            i++;
            params->message_delay = atoi(argv[i]);
        } else if (strcmp(argv[i], "--consumption-delay") == 0 || strcmp(argv[i], "-cd") == 0) {
            i++;
            params->consumption_delay = atoi(argv[i]);
        } else if (strcmp(argv[i], "--threshold") == 0 || strcmp(argv[i], "-t") == 0) {
            i++;
            params->threshold = atoi(argv[i]);
        } else if (strcmp(argv[i], "--max-queue-length") == 0 || strcmp(argv[i], "-mql") == 0) {
            i++;
            params->max_queue_length = atoi(argv[i]);
        } else {
            return 0;  // Unknown parameter
        }
    }

    // Check if required parameters are provided
    if (params->message_delay == -1 || params->threshold == -1) {
        return -1;  // Missing required parameters
    }

    return 1;  // Parameters parsed successfully
}


char random_char(int index) {
    char charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    return charset[index];
}

// Function to produce a message
Message produce_random_message(int id) {
    Message msg = {.id = id};
    srand(time(NULL));
    int i, index;
    for (i = 0; i < MAX_MESSAGE_LENGTH - 1; i++) {
        index = rand() % 62;
        msg.data[i] = random_char(index);
    }
    msg.data[MAX_MESSAGE_LENGTH - 1] = '\0';

    return msg;
}

// Producer consumes `spaces`, produces an item, then posts to `items`.
void *producer(void *params) {
    Parameters *p = (Parameters *) params;

    while (1) {
        sem_wait(&spaces); // Wait for space
        pthread_mutex_lock(&mutex);

        // Produce an item
        Message new_msg = produce_random_message(next_message_id++);
        messageQueue[queue_length++] = new_msg;
        printf("Produced: %d\n", new_msg.id);
        insertDataToPostgres("metrics", queue_length);

        pthread_mutex_unlock(&mutex);
        sem_post(&items); // Signal that an item is available

        // Sleep according to the current production delay
        usleep(p->message_delay * 1000000);
    }
    return NULL;
}

void printMessageQueueState() {
    printf("Message Queue State: ");
    for (int i = 0; i < MAX_QUEUE_LENGTH; i++) {
        if (i < queue_length) {
            printf("%d ", messageQueue[i].id);
        } else {
            printf("- ");
        }
    }
    printf("\n");
}


// Consumer waits for `items`, consumes an item, then posts to `spaces`.
void *consumer(void *params) {
    Parameters *p = (Parameters *) params;

    while (1) {
        sem_wait(&items); // Wait for an item
        pthread_mutex_lock(&mutex);

        // Consume the first item
        Message msg_to_consume = messageQueue[0];
        for (int i = 0; i < queue_length - 1; i++) {
            messageQueue[i] = messageQueue[i + 1]; // Shift left
        }
        queue_length--;

        printf("Consumed: %d %s\n", msg_to_consume.id, msg_to_consume.data);
        insertDataToPostgres("metrics", queue_length);
        printMessageQueueState();

        pthread_mutex_unlock(&mutex);
        sem_post(&spaces); // Signal that there's space

        // Sleep to simulate consumption delay
        sleep(p->consumption_delay);
    }
    return NULL;
}

// Controller adjusts the production rate
void *controller(void *params) {
    Parameters *p = (Parameters *) params;

    while (1) {
        pthread_mutex_lock(&mutex);
        // Check if messages are below threshold to increase production rate
        if (queue_length < p->threshold) {
            p->message_delay =
                    p->message_delay > 1 ? p->message_delay - 1 : (0.9) * p->message_delay; // Speed up the producer
        } else if (queue_length > p->threshold) { // Decrease production rate
            p->message_delay += 1;
        }
        insertDataToPostgres("delay_metrics", p->message_delay);
        printf("Controller: Message delay adjusted to %.2f seconds\n", p->message_delay);
        pthread_mutex_unlock(&mutex);
        sleep(1); // Controller checks every second
    }
    return NULL;
}

int main(int argc, char *argv[]) {
    Parameters params;

    int result = parseParameters(argc, argv, &params);

    if (result == 0) {
        fprintf(stderr, "Error: Unknown parameter.\n");
        printUsage(argv[0]);
        return 1;
    } else if (result == -1) {
        fprintf(stderr, "Error: Missing required parameters.\n");
        printUsage(argv[0]);
        return 1;
    }

    // Print the parsed parameter values
    printf("Program Parameters:\n");
    printf("Message Delay: %.2f\n", params.message_delay);
    printf("Consumption Delay: %d\n", params.consumption_delay);
    printf("Threshold: %d\n", params.threshold);
    printf("Max Queue Length: %d\n", params.max_queue_length);
    printf("============================: %d\n\n", params.max_queue_length);

    const char *host = "localhost";
    const char *dbname = "crtp_metrics";
    const char *user = "postgres";
    const char *password = "crtp";
    const char *table = "metrics";

    // Initialize global database connection
    initializePostgresConnection(host, dbname, user, password);


    // Initialize the message queue
    messageQueue = malloc(params.max_queue_length * sizeof(Message));
    if (!messageQueue) {
        perror("Failed to allocate message queue");
        return 1;
    }

    // Initialize semaphores
    sem_init(&items, 0, 0); // No items at the start
    sem_init(&spaces, 0, params.max_queue_length); // Max queue length spaces

    // Initialize threads
    pthread_t producer_thread, consumer_thread, controller_thread;

    // Create threads
    pthread_create(&producer_thread, NULL, producer, &params);
    pthread_create(&consumer_thread, NULL, consumer, &params);
    pthread_create(&controller_thread, NULL, controller, &params);

    // Join threads (these joins will effectively never happen due to infinite loops)
    pthread_join(producer_thread, NULL);
    pthread_join(consumer_thread, NULL);
    pthread_join(controller_thread, NULL);

    // Clean up (this code is unreachable, for a real system you'd need a way to gracefully shut down)
    sem_destroy(&items);
    sem_destroy(&spaces);
    free(messageQueue);

    return 0;
}
