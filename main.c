#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/ipc.h>
#include <sys/types.h>

#define SHM_KEY 42690
#define SEM_KEY 48630
#define SIZE 1024
#define STORY "story.txt"

union semun {

    int val;
    struct semod_ds *buf;
    unsigned short *array;
    struct seminfo *__buf;

};

int main() {

    int shm_id = shmget( SHM_KEY, SIZE, 0666 );
    int sem_id = semget( SEM_KEY, 1, 0666 );
    int fd = open( STORY, O_WRONLY | O_APPEND, 0666 );
    struct sembuf ss;
    char line[ SIZE ];

    if (shm_id == -1) {

        printf("SHM Error %d: %s\n", errno, strerror( errno ) );
        return 1;

    }
    else if (sem_id == -1) {

        printf("SEM Error %d: %s\n", errno, strerror( errno ) );
        return 2;

    }

    else if (fd == -1) {

        printf( "Error %d: %s\n", errno, strerror( errno ) );
        return 4;

    }

    ss.sem_op = -1;
    ss.sem_num = 0;
    ss.sem_flg = SEM_UNDO;
    semop( sem_id, &ss, 1);

    char* recent = shmat( shm_id, 0, 0);
    printf("Most recent edit: [%s]\n", recent);

    printf("Add a line: ");
    scanf(" %[^\n]s", line);
    strcpy( recent, line );
    strcat( line, "\n" );
    write( fd, line, strlen( line ) );
    close( fd );
    ss.sem_op = 1;
    semop( sem_id, &ss, 1 );

    return 0;

}
