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
#include <sys/uio.h>
#include <sys/time.h>
// File function and bzero
#include <fcntl.h>
#include <strings.h>

using namespace std;
#define BUF 1024
#define BACKLOG 1

int duration (struct timeval *start,struct timeval *stop,struct timeval *delta);
int create_server_socket (int port);

struct sockaddr_in sock_serv,sock_clt;

std::ifstream::pos_type filesize(const char* filename)
{
    std::ifstream in(filename, std::ifstream::ate | std::ifstream::binary);
    return in.tellg();
}

int main (int argc, char **argv) {
  int sfd, fd;
  unsigned int length=sizeof(struct sockaddr_in);
  //socklen_t addrlen;
  char buffer[BUF];
  //use subbuffer for first 3 characters of input string
  char subbuff[4];
  char filename[256];
  char sfile[128];
  char dst[INET_ADDRSTRLEN];
  ushort clt_port;
  int size;
  long int n,m,count=0;
  unsigned int nsid;
  
  ofstream fileStream;
  DIR *dir;
  struct dirent *ent;
  struct stat file_stats;

  //time variables
  time_t intps;
  struct tm* tmi;

  if( argc < 3 ){
     printf("Usage: %s Port Directory\n", argv[0]);
     exit(EXIT_FAILURE);
  }

    sfd = create_server_socket(atoi(argv[1]));

    bzero(buffer, BUF);
    listen(sfd,BACKLOG);

   
  while (1) {
     printf("Waiting for connections...\n");
      nsid=accept(sfd,(struct sockaddr*)&sock_clt,&length);
     if (fd > 0)
     {
        printf ("Client connected from %s:%d...\n", inet_ntoa (sock_clt.sin_addr),ntohs(sock_clt.sin_port));
        strcpy(buffer,"Welcome to insane Fileserver, Please enter your request:\n");
        //überprüfen ob alles gsendet worden is
        send(fd, buffer, strlen(buffer),0);
     }
     do
     {
        size = recv (fd, buffer, BUF-1, 0);
        if( size > 0)
        {
            buffer[size] = '\0';
            printf ("Message received: %s\n", buffer);
            memcpy(subbuff, &buffer[0], 3);
            memcpy(filename, &buffer[4], size);
            subbuff[3] = '\0';
            if (strcmp (buffer, "LIST\n") == 0)
            {
          
                if ((dir = opendir (argv[2])) != NULL)
                {
                    //printf("test1");
                    /* print all the files within directory */
                    while ((ent = readdir (dir)) != NULL)
                    {
                        //printf("test2");
                        stat(ent->d_name, &file_stats);
                        if(ent->d_type == DT_REG)
                        {
                            //stat(ent->d_name, &file_stats);
                            //printf("test3");
                            // memset(buffer, 0, sizeof(int));
                            stat(ent->d_name, &file_stats);
                            printf (buffer, sizeof(buffer), "%s %u Bytes\n", ent->d_name, (unsigned int)file_stats.st_size);
                            snprintf (buffer, sizeof(buffer), "%s %u Bytes\n", ent->d_name, (unsigned int)file_stats.st_size);
                            send(fd, buffer, strlen(buffer), 0);
                        }
                  }
                  closedir (dir);

                     bzero(buffer, BUF);

                }
                else
                {
                    /* could not open directory */
                    perror ("123");
                    printf("Could not open directory!");
                    return EXIT_FAILURE;
                }
            }
            if (strcmp (subbuff, "GET") == 0)
            {
                fileStream.open(filename);
                fileStream << "123";
                fileStream.close();

            }
            if (strcmp (subbuff, "PUT") == 0)
            {
              printf("%s", filename);

              bzero(sfile, 256);
              intps = time(NULL);
              tmi = localtime(&intps);
              bzero(sfile,256);
              sprintf(sfile,"file.%d.%d.%d.%d.%d.%d",tmi->tm_mday,tmi->tm_mon+1,1900+tmi->tm_year,tmi->tm_hour,tmi->tm_min,tmi->tm_sec);
              printf("Creating the copied output file : %s\n",sfile);

              if((fd=open(sfile, O_CREAT|O_WRONLY,0600))==-1)
              {
                perror("open fail");
                exit(3);
              }

              bzero(buffer,BUF);

              n=recv(nsid, buffer, BUF,0);
              while(n)
              {
                if(n==-1)
                {
                  perror("recv fail");
                  exit(5);
                }
                if((m=write(fd,buffer,n))==-1)
                {
                  perror("write fail");
                  exit(6);
                }
                count=count+m;
                bzero(buffer,BUF);
                n=recv(nsid,buffer,BUF,0);
              }

              close(sfd);
              close(fd);

              printf("Fin de la transmission avec %s.%d\n",dst,clt_port);
              printf("Number of octets received: %ld \n",count);

              return EXIT_SUCCESS;

            }
            if (strcmp (buffer, "QUIT\n") == 0)
            {
              close(fd);
              close (sfd);
              return EXIT_SUCCESS;
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
     close (fd);
  }
  close (sfd);
  return EXIT_SUCCESS;
}


/* Fonction permettant le calcul de la durée de l'envoie */
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

int create_server_socket (int port){
    int l;
    int sfd;
    int yes=1;
    
  sfd = socket(PF_INET,SOCK_STREAM,0);
  if (sfd == -1){
        perror("socket fail");
        return EXIT_SUCCESS;
  }
    
    if(setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR,&yes,sizeof(int)) == -1 ) {
        perror("setsockopt error");
        exit(5);
    }
    
    //preparation de l'adresse de la socket destination
  l=sizeof(struct sockaddr_in);
  bzero(&sock_serv,l);
  
  sock_serv.sin_family=AF_INET;
  sock_serv.sin_port=htons(port);
  sock_serv.sin_addr.s_addr=htonl(INADDR_ANY);
    
  //Affecter une identité au socket
  if(bind(sfd,(struct sockaddr*)&sock_serv,l)==-1){
    perror("bind fail");
    return EXIT_FAILURE;
  }
    
    
    return sfd;
}
