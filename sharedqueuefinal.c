/*C program to create a shared queue between 5 threads */
/*This program demonstrates a shared queue where multiple reader threads receive string messages from a writer thread*/

/*mutex is used for synchronization between reader and writer threads */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h> // for usleep

#define NUM_THREADS 5
#define MESSAGES_PER_SECOND 5
#define QUEUE_SIZE 100
#define MESSAGE_MAX_LENGTH 256

struct Queue {
    char messages[QUEUE_SIZE][MESSAGE_MAX_LENGTH];
    int size;
    int front;
    int rear;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
};

void initQueue(struct Queue *q) {
    q->size = QUEUE_SIZE;
    q->front = -1;
    q->rear = -1;
    pthread_mutex_init(&q->mutex, NULL);
    pthread_cond_init(&q->cond, NULL);
}

void enqueue(struct Queue *q, const char *message) {
    pthread_mutex_lock(&q->mutex);
    
    while ((q->rear + 1) % q->size == q->front) {
        // Queue is full, wait for a reader to dequeue
        pthread_cond_wait(&q->cond, &q->mutex);
    }

    if (q->front == -1) {
        q->front = 0;
        q->rear = 0;
    } else {
        q->rear = (q->rear + 1) % q->size;
    }

    strncpy(q->messages[q->rear], message, MESSAGE_MAX_LENGTH - 1);
    q->messages[q->rear][MESSAGE_MAX_LENGTH - 1] = '\0';
    
    pthread_cond_signal(&q->cond);
    pthread_mutex_unlock(&q->mutex);
}

void dequeue(struct Queue *q, char *message) {
    pthread_mutex_lock(&q->mutex);

    while (q->front == -1) {
        // Queue is empty, wait for a writer to enqueue
        pthread_cond_wait(&q->cond, &q->mutex);
    }

    strncpy(message, q->messages[q->front], MESSAGE_MAX_LENGTH - 1);
    message[MESSAGE_MAX_LENGTH - 1] = '\0';

    if (q->front == q->rear) {
        q->front = -1;
        q->rear = -1;
    } else {
        q->front = (q->front + 1) % q->size;
    }
    
    pthread_cond_signal(&q->cond);
    pthread_mutex_unlock(&q->mutex);
}

// Writer thread function
void *writer(void *arg) {
    struct Queue *q = (struct Queue *)arg;
    char message[MESSAGE_MAX_LENGTH];

    while (1) {
        for (int i = 0; i < MESSAGES_PER_SECOND; i++) {
            snprintf(message, MESSAGE_MAX_LENGTH, "Message from writer %ld, message #%d", pthread_self(), i);
            enqueue(q, message);
        }
        usleep(1000000 / MESSAGES_PER_SECOND); // Sleep for 1 second
    }

    return NULL;
}

// Reader thread function
void *reader(void *arg) {
    struct Queue *q = (struct Queue *)arg;
    char message[MESSAGE_MAX_LENGTH];

    while (1) {
        dequeue(q, message);
        printf("Reader: Received message: %s\n", message);
    }

    return NULL;
}

int main() {
    pthread_t writerThread;
    pthread_t readerThreads[NUM_THREADS];
    struct Queue q;

    initQueue(&q);

    // Create writer thread
    pthread_create(&writerThread, NULL, writer, &q);

    // Create reader threads
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_create(&readerThreads[i], NULL, reader, &q);
    }

    // Wait for threads to finish (this program runs indefinitely)
    pthread_join(writerThread, NULL);
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(readerThreads[i], NULL);
    }

    // Clean up and exit
    pthread_mutex_destroy(&q.mutex);
    pthread_cond_destroy(&q.cond);

    return 0;
}
