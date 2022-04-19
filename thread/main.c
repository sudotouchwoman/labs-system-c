/* Compilation: gcc -o prg prg.c -lpthread -lrt */

#ifndef _REENTRANT
#define _REENTRANT
#endif

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <pthread.h>
#include <sched.h>

#define BUF_SIZE 256

extern int errno;

pthread_t tid, ptid, itid, ctid;
pthread_mutex_t mutx1, mutx2;
pthread_cond_t condx1, condx2;
int msglen1 = 0, msglen2 = 0, done = 0;
char msgbuf1[BUF_SIZE], msgbuf2[BUF_SIZE];

void invert(int sig)
{
    char c, buf[BUF_SIZE];
    int i, l;

    write(1, "Sig caught\n", 13);

    if (done)
    {
        pthread_mutex_unlock(&mutx2);
        pthread_kill(ctid, SIGUSR1);
        pthread_exit(0);
    };

    pthread_mutex_lock(&mutx1);
    strncpy(buf, msgbuf1, msglen1);
    l = msglen1;
    msglen1 = 0;
    pthread_mutex_unlock(&mutx1);
    pthread_cond_signal(&condx1);
    for (i = 0; i < (l / 2); i++)
    {
        c = buf[i];
        buf[i] = buf[l - 2 - i];
        buf[l - 2 - i] = c;
    };
    write(1, buf, l);
    strncpy(msgbuf2, buf, l);
    msglen2 = l;
    if ((i = pthread_kill(ctid, SIGUSR1)))
    {
        fprintf(stderr, "Ret for pthread_kill = %d\n", i);
    };
    pthread_cond_wait(&condx2, &mutx2);
}

void *
producer(void *arg_p)
{
    write(1, "P start\n", 8);
    pthread_mutex_lock(&mutx1);
    while (1)
    {
        msglen1 = read(0, msgbuf1, BUF_SIZE);
        if (msglen1 == 0)
            break;
        pthread_kill(itid, SIGUSR1);
        pthread_cond_wait(&condx1, &mutx1);
    }; // End while(1)
    done = 1;
    pthread_mutex_unlock(&mutx1);
    pthread_kill(itid, SIGUSR1);
    pthread_exit(0);
}

void *
invertor(void *arg_p)
{
    struct sigaction act;

    write(1, "I start\n", 8);

    act.sa_handler = invert;
    sigaction(SIGUSR1, &act, NULL);
    pthread_mutex_lock(&mutx2);
    while (1)
    { // Этот цикл имитирует собственную функциональность
        sleep(1);
        write(1, "I working\n", 10);
    }; // End while(1)
}

void *
consumer(void *arg_p)
{
    sigset_t ss;
    int i;

    write(1, "C start\n", 8);
    sigemptyset(&ss);
    sigaddset(&ss, SIGUSR1);
    //  pthread_sigmask(SIG_UNBLOCK, &ss, NULL);

    while (1)
    {
        while (!msglen2)
        {
            if (done)
            {
                pthread_exit(0);
            };
            sigwait(&ss, &i);
        };
        pthread_mutex_lock(&mutx2);
        write(1, msgbuf2, msglen2);
        msglen2 = 0;
        pthread_mutex_unlock(&mutx2);
        pthread_cond_signal(&condx2);
    }; // End while(1)
}

int main(int argc, char *argv[])
{
    pthread_attr_t pattr;
    int ret;

    pthread_attr_init(&pattr);
    pthread_attr_setscope(&pattr, PTHREAD_SCOPE_SYSTEM);
    pthread_attr_setdetachstate(&pattr, PTHREAD_CREATE_JOINABLE);

    pthread_mutex_init(&mutx1, NULL);
    pthread_cond_init(&condx1, NULL);
    pthread_mutex_init(&mutx2, NULL);
    pthread_cond_init(&condx2, NULL);

    if ((ret = pthread_create(&ptid, &pattr, producer, NULL)))
        perror("pthread_create");
    fprintf(stderr, "ptid = %lu\n", ptid);
    if ((ret = pthread_create(&itid, &pattr, invertor, NULL)))
        perror("pthread_create");
    fprintf(stderr, "itid = %lu\n", itid);
    if ((ret = pthread_create(&ctid, NULL, consumer, NULL)))
        perror("pthread_create");
    fprintf(stderr, "ctid = %lu\n", ctid);

    pthread_join(ctid, NULL);

    pthread_mutex_destroy(&mutx1);
    pthread_cond_destroy(&condx1);
    pthread_mutex_destroy(&mutx2);
    pthread_cond_destroy(&condx2);

    pthread_exit(0);
}