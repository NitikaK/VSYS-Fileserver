/* myclient.c */
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#define BUF 1024
#define PORT 6543

int main (int argc, char **argv) {
  int create_socket;
<<<<<<< HEAD
  char buffer[BUF];
  memset(buffer, 0, BUF);
=======
  char buffer[BUF], filename[256], subbuff[4];
>>>>>>> origin/Experiment
  struct sockaddr_in address;
  int size;

  if( argc < 3 ){
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
     memset(buffer, 0, BUF);
     printf ("Send request: ");
     fgets (buffer, BUF, stdin);
     if (strncmp (buffer, "LIST", 4) == 0)
     {
       send(create_socket, buffer, strlen (buffer), 0);
       size=recv(create_socket, buffer, BUF, 0);
       if (size>0)
       {
          buffer[size]= '\0';
          printf("%s", buffer);

          //bzero(buffer,BUF);
       }
     }
<<<<<<< HEAD
     else if (strncmp (buffer, "PUT", 3) == 0) 
=======
      else if (buffer[0] == 'P' && buffer[1] == 'U' && buffer[2] == 'T')
      {
            printf("%s\n", buffer);     

           // bzero(buffer, BUF);
      }
     else if (strcmp(buffer, "QUIT\n") == 0)
>>>>>>> origin/Experiment
     {
       send(create_socket, buffer, strlen (buffer), 0);
       /*size=recv(create_socket, buffer, BUF, 0);
       if (size>0)
       {
          buffer[size]= '\0';
          printf("%s", buffer);
       }*/
     }
     else if (strncmp (buffer, "GET", 3) == 0) 
     {
       send(create_socket, buffer, strlen (buffer), 0);
       /*size=recv(create_socket, buffer, BUF, 0);
       if (size>0)
       {
          buffer[size]= '\0';
          printf("%s", buffer);
       }*/
     }
     else if (strncmp(buffer, "QUIT", 4) == 0)
     {
      printf("Quitting...\n");
     }
     else
     {
      printf("Try again\n");
     }
  }
  while (strncmp (buffer, "QUIT", 4) != 0);
  close (create_socket);
  return EXIT_SUCCESS;
}

