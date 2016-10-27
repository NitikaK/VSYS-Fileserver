/* myclient.c */
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <fcntl.h>
#include <unistd.h>
#include <strings.h>
#define BUF 1024
#define PORT 6543

int duration (struct timeval *start,struct timeval *stop,struct timeval *delta)
{
    suseconds_t microstart, microstop, microdelta;
    
    microstart = (suseconds_t) (100000*(start->tv_sec))+ start->tv_usec;
    microstop = (suseconds_t) (100000*(stop->tv_sec))+ stop->tv_usec;
    microdelta = microstop - microstart;
    
    delta->tv_usec = microdelta%100000;
    delta->tv_sec = (time_t)(microdelta/100000);
    
    if((*delta).tv_sec < 0 || (*delta).tv_usec < 0)
        return -1;
    else
        return 0;
}


int main (int argc, char **argv) 
{
  struct timeval start, stop, delta;
  int create_socket, new_socket;
  char buffer[BUF];
  //char subbuff[4];
  char filename[128];
  struct sockaddr_in address;
  int size;
  off_t count=0, m,sz;
  long int n;
  struct stat buffert;

    if( argc < 3 )
    {
       printf("Usage: %s ServerAdresse Port\n", argv[0]);
       exit(EXIT_FAILURE);
    }

    if ((create_socket = socket (AF_INET, SOCK_STREAM, 0)) == -1)
    {
       perror("Socket error");
       return EXIT_FAILURE;
    }

  memset(&address,0,sizeof(address));
  address.sin_family = AF_INET;
  address.sin_port = htons (atoi(argv[2]));
  inet_aton (argv[1], &address.sin_addr);

  if (connect ( create_socket, (struct sockaddr *) &address, sizeof (address)) == 0)
  {
     printf ("Connection with server (%s) established\n", inet_ntoa (address.sin_addr));
     size=recv(create_socket,buffer,BUF, 0);
     if (size>0)
     {
        buffer[size]= '\0';
        printf("%s",buffer);
     }
  }
  else
  {
     perror("Connect error - no server available");
     return EXIT_FAILURE;
  }

  do {
      printf ("Send request: ");
      fgets (buffer, BUF, stdin);
      //memcpy(subbuff, &buffer[0], 3);
      memcpy(filename, &buffer[4], size);
      //subbuff[3] = '\0';
     if (strcmp(buffer, "LIST\n") == 0) {
       send(create_socket, buffer, strlen (buffer), 0);
       size=recv(create_socket, buffer, BUF, 0);
       if (size>0)
       {
          buffer[size]= '\0';
          printf("%s", buffer);
       }
     }
     else if (strcmp(buffer, "PUT")== 0)
     {  
      if ((new_socket = open(filename,O_RDONLY))==-1)
      {
        perror("open fail");
        return EXIT_FAILURE;
      }
    
  //file size
      if (stat(filename,&buffert)==-1)
      {
        perror("stat fail");
        return EXIT_FAILURE;
      }
      else
        sz=buffert.st_size;
    
  //preparation de l'envoie
      bzero(&buffer,BUF);
    
    if(connect(create_socket,(struct sockaddr*)&address,sizeof(address))==-1){
        perror("conection error\n");
        exit (3);
    }
  gettimeofday(&start,NULL);
    n=read(new_socket,buffer,BUF);
  while(n){
    if(n==-1){
      perror("read fails");
      return EXIT_FAILURE;
    }
    m=sendto(create_socket,buffer,n,0,(struct sockaddr*)&address,sizeof(address));
    if(m==-1){
      perror("send error");
      return EXIT_FAILURE;
    }
    count+=m;
    //fprintf(stdout,"----\n%s\n----\n",buf);
    bzero(buffer,BUF);
        n=read(new_socket,buffer,BUF);
  }
  //read vient de retourner 0 : fin de fichier
  
  //pour debloquer le serv
  m=sendto(create_socket,buffer,0,0,(struct sockaddr*)&address,sizeof(address));
  gettimeofday(&stop,NULL);
  duration(&start,&stop,&delta);
    
  printf("Number of octets transferred: %lld\n",count);
  printf("Total size: %lld \n",sz);
  printf("Duration: %ld.%d \n",delta.tv_sec,delta.tv_usec);
    
    close(create_socket);
  return EXIT_SUCCESS;
      }
      else
      {
        printf("Try again\n");
      }
  
    }
    while (strcmp (buffer, " QUIT\n") != 0);
  
  close (create_socket);
  return EXIT_SUCCESS;
}

