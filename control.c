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


//When a new story is created while one already exists, all the contents are removed
//We were not able to get around this :(
void create_story() {

    int shm_id = shmget(SHM_KEY, SIZE, 0666 | IPC_CREAT);
  if (shm_id == -1){
    printf("yError %d: %s\n", errno, strerror(errno));
    return;
  }

  int sem_id = semget(SEM_KEY, 1, 0666 | IPC_CREAT);
  if (sem_id == -1){
    printf("gError %d: %s\n", errno, strerror(errno));
    return;
  }

  union semun sn;
  sn.val = 1;
  semctl(sem_id, 0, SETVAL, sn);

  int fd = open(STORY, O_CREAT | O_TRUNC, 0644);
  if (fd == -1){
    printf("Error %d: %s\n", errno, strerror(errno));
    return;
  }
  // write(fd, "\0", 1);
  close(fd);
  printf("Story created.\n");

}

void remove_story() {

    int shm_id = shmget( SHM_KEY, SIZE, 0644 );
    int sem_id = semget( SEM_KEY, 1, 0644 );
    int fd = open( STORY, O_RDONLY );
    struct sembuf ss;
    char s[ SIZE ];

    if (shm_id == -1 || sem_id == -1 || fd == -1) {

        printf( "Error %d: %s\n", errno, strerror( errno ) );
        return;

    }

    ss.sem_op = -1;
    ss.sem_num = 0;
    ss.sem_flg = 0;
    semop( sem_id, &ss, 1 );

    read( fd, s, sizeof( s ) );
    close( fd );
    printf( "Removing story: \n%s", s );

    shmctl( shm_id, IPC_RMID, 0 );
    semctl( sem_id, 0, IPC_RMID );
    char* args[] = { "rm", STORY, 0 };
    execvp( args[0], args );

}

void view_story() {

    int fd = open( STORY, O_RDONLY );
    char s[ SIZE ];

    if (fd == -1) {

        printf( "Error %d: %s\n", errno, strerror( errno ) );
        return;

    }

    read( fd, s, sizeof( s ) );
    close( fd );
    printf("Story: \n%s", s);

}

int main( int argc, char* argv[] ) {

    if ( argc < 2 ) {

        printf("Enter an argument! (-c to create, -r to remove, -v to view)\n");

    }
    else if ( !strcmp( argv[ 1 ], "-c" ) ) {

        //printf("Entered story creation...\n");
        create_story();

    }
    else if ( !strcmp( argv[ 1 ], "-r" ) ) {

        printf("Entered story removal...\n");
        remove_story();

    }
    else if ( !strcmp( argv[ 1 ], "-v" ) ) {

        printf("Entered story viewer...\n");
        view_story();

    }
    else {

        printf("Error! Please enter a valid argument.\n");

    }
    return 0;

}
