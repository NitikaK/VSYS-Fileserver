/* myclient.c */
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <sys/uio.h>
#define BUF 1024
#define PORT 6543

int duration (struct timeval *start,struct timeval *stop, struct timeval *delta);
int create_client_socket (int port, char* ipaddr);

 struct sockaddr_in sock_serv;

int main (int argc, char **argv) 
{
  int create_socket, new_socket;
  char buffer[BUF], filename[128];
  char subbuff[4];
  int size;
  off_t count=0, msize, sz;
  //time variables for duration function
  struct timeval start, stop, delta;

  if( argc < 3 ){
     printf("Usage: %s ServerAdresse Port\n", argv[0]);
     exit(EXIT_FAILURE);
  }

  if ((create_socket = socket (AF_INET, SOCK_STREAM, 0)) == -1)
  {
     perror("Socket error");
     return EXIT_FAILURE;
  }


  if (connect ( create_socket, (struct sockaddr *) &sock_serv, sizeof (sock_serv)) == 0)
  {
     printf ("Connection with server (%s) established\n", inet_ntoa (sock_serv.sin_addr));
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
      memcpy(subbuff, &buffer[0], 3);
      memcpy(filename, &buffer[4], size);
      subbuff[3] = '\0';
     if (strcmp (buffer, "LIST\n") == 0) 
     {
       send(create_socket, buffer, strlen (buffer), 0);
       size=recv(create_socket, buffer, BUF, 0);
       if (size>0)
       {
          buffer[size]= '\0';
          printf("%s", buffer);
       }
     }
     else if (strcmp(subbuff, "PUT") == 0)
     {
      printf("%s", filename);
      
              gettimeofday(&start,NULL);
              size = read(new_socket, buffer, BUF);
              while(size)
              {
                if (size==-1)
                {
                  perror("read fail");
                  return EXIT_FAILURE;
                }

                msize=sendto(create_socket, buffer, size, 0, (struct sockaddr*)&sock_serv, sizeof(sock_serv));
                if(msize==-1)
                {
                  perror("send error");
                  return EXIT_FAILURE;
                }
                count+=msize;

                bzero(buffer, BUF);
                size=read(new_socket, buffer, BUF);
              }

              msize=sendto(create_socket,buffer,0,0,(struct sockaddr*)&sock_serv, sizeof(sock_serv));
              gettimeofday(&stop, NULL);
              duration(&start, &stop, &delta);

              printf("Octets transferred: %lld\n", count);
              printf("Total size: %lld\n", sz);
              printf("Duration: %ld.%d\n", delta.tv_sec, delta.tv_usec);

            }
     
     else if (strcmp(buffer, "QUIT\n") == 0)
     {
      close(create_socket);
      return EXIT_SUCCESS;
     }
     else
     {
      printf("Try again\n");
     }

   }
      while (strcmp (buffer, "QUIT\n") != 0);
  
  close (create_socket);
  return EXIT_SUCCESS;
}


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

int create_client_socket (int port, char* ipaddr){
    int l;
  int sfd;
    
  sfd = socket(PF_INET,SOCK_STREAM,0);
  if (sfd == -1){
        perror("socket fail");
        return EXIT_FAILURE;
  }
    
    //preparation de l'adresse de la socket destination
  l=sizeof(struct sockaddr_in);
  bzero(&sock_serv,l);
  
  sock_serv.sin_family=AF_INET;
  sock_serv.sin_port=htons(port);
    if (inet_pton(AF_INET,ipaddr,&sock_serv.sin_addr)==0){
    printf("Invalid IP adress\n");
    return EXIT_FAILURE;
  }
    
    return sfd;
}
