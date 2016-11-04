/* myserver.c */
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <iostream>
#include <sys/stat.h>
#include <regex.h>
#include <fstream>

using namespace std;
#define BUF 1024

std::ifstream::pos_type filesize(const char* filename)
{
    std::ifstream in(filename, std::ifstream::ate | std::ifstream::binary);
    return in.tellg();
}

int main (int argc, char **argv) {
  int create_socket, new_socket;
  socklen_t addrlen;
  char buffer[BUF];
  char filename[128];
  int size;
  struct sockaddr_in address, cliaddress;
  ofstream fileStream;
  DIR *dir;
  struct dirent *ent;
  struct stat file_stats;

  if( argc < 3 ){
     printf("Usage: %s Port Directory\n", argv[0]);
     exit(EXIT_FAILURE);
  }

  create_socket = socket (AF_INET, SOCK_STREAM, 0);

  memset(&address,0,sizeof(address));
  address.sin_family = AF_INET;
  address.sin_addr.s_addr = INADDR_ANY;
  address.sin_port = htons (atoi(argv[1]));


  if (bind ( create_socket, (struct sockaddr *) &address, sizeof (address)) != 0) {
     perror("bind error");
     return EXIT_FAILURE;
  }
  listen (create_socket, 5);

  addrlen = sizeof (struct sockaddr_in);

  while (1) {
     printf("Waiting for connections...\n");
     new_socket = accept ( create_socket, (struct sockaddr *) &cliaddress, &addrlen );
     if (new_socket > 0)
     {
        printf ("Client connected from %s:%d...\n", inet_ntoa (cliaddress.sin_addr),ntohs(cliaddress.sin_port));
        strcpy(buffer,"Welcome to insane Fileserver, Please enter your request:\n");
        //überprüfen ob alles gsendet worden is
        send(new_socket, buffer, strlen(buffer),0);
     }
     do
     {
        memset(buffer, 0, BUF);
        memset(filename, 0, 128);
        size = recv (new_socket, buffer, BUF-1, 0);
        if(size > 0)
        {
            buffer[size] = '\0';
            printf ("Message received: %s\n", buffer);
            if (strcmp (buffer, "LIST\n") == 0)
            {
          
                if ((dir = opendir (argv[2])) != NULL)
                {
                    /* print all the files within directory */
                    while ((ent = readdir (dir)) != NULL)
                    {
                        stat(ent->d_name, &file_stats);
                        if(ent->d_type == DT_REG)
                        {
                            //stat(ent->d_name, &file_stats);
                            // memset(buffer, 0, sizeof(int));
                            //stat(ent->d_name, &file_stats);
                            printf ("%s %u Bytes\n", ent->d_name, (unsigned int)file_stats.st_size);
                            snprintf (buffer, sizeof(buffer), "%s %u Bytes\n", ent->d_name, (unsigned int)file_stats.st_size);
                            send(new_socket, buffer, strlen(buffer), 0);
                        }
                  }
                  closedir (dir);
                  bzero(buffer, sizeof(buffer));
                }
                else
                {
                    /* could not open directory */
                    perror ("123");
                    printf("Could not open directory!");
                    return EXIT_FAILURE;
                }
            }
            else if (strcmp (buffer, "QUIT\n") == 0)
            {
              close(new_socket);
              close (create_socket);
              return EXIT_SUCCESS;
            }
            else
            {
		    memcpy(filename, &buffer[4], size);
		    filename[127] = '\0';
		    if (strncmp (buffer, "GET", 3) == 0)
		    {
		        /*fileStream.open(filename);
		        fileStream << "123";
		        fileStream.close();*/
			printf("%s", filename);

		    }
		    else if (strncmp (buffer, "PUT", 3) == 0)
		    {
			printf("%s", filename);
		    }
            }

        }
        else if (size == 0)
        {
           printf("Client closed remote socket\n");
           break;
        }
        else
        {
           perror("recv error");
           return EXIT_FAILURE;
        }
     } while (strncmp (buffer, "QUIT", 4)  != 0);
     close (new_socket);
  }
  close (create_socket);
  return EXIT_SUCCESS;
}
