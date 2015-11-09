#include <fcntl.h>
#include <pthread.h>
const char * usage =
"                                                               \n"
"daytime-server:                                                \n"
"                                                               \n"
"Simple server program that shows how to use socket calls       \n"
"in the server side.                                            \n"
"                                                               \n"
"To use it in one window type:                                  \n"
"                                                               \n"
"   daytime-server <port>                                       \n"
"                                                               \n"
"Where 1024 < port < 65536.             \n"
"                                                               \n"
"In another window type:                                       \n"
"                                                               \n"
"   telnet <host> <port>                                        \n"
"                                                               \n"
"where <host> is the name of the machine where daytime-server  \n"
"is running. <port> is the port number you used when you run   \n"
"daytime-server.                                               \n"
"                                                               \n"
"Then type your name and return. You will get a greeting and   \n"
"the time of the day.                                          \n"
"                                                               \n";


#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
int QueueLength = 5;
pthread_mutex_t mutex;

// Processes time request
void processTimeRequest( int socket );
void poolRequest(int socket);
/*{  
  time_t now;
  time(&now);
  char*timeString = ctime(&now);

  const char* hi = "\nHi";
  const char* timeIs = " the time is:\n";

  write(socket,hi,strlen(hi));
  write(socket,name,strlen(name));
  write(socket,timeIs,strlen(timeIs));

  write(socket,timeString,strlen(timeString));

  const char* newline = "\n";
  write(socket,newline,strlen(newline));
}*/

void handel(int pro){
  while (waitpid((pid_t)(-1), 0, WNOHANG) > 0) {}
}
void handel1(int h){
  return;
}
int isnumber(char* a){
  char * b = a;
 // int i = 0;
  int k =  0;
  for (int i = 0; i < strlen(a); i++)
  {
    if(isdigit(b[i])){
      k = 1;
    }else{
      k = 0;
    }
  }
 
  if(k == 0){
    return 0;
  }  

  return 1;

}

int
main( int argc, char ** argv )
{
  // Print usage if not enough arguments
  signal(SIGCHLD,handel);
  signal(SIGPIPE,handel1); 
  if ( argc < 2 ) {
    fprintf( stderr, "%s", usage );
    exit( -1 );
  }
  int port;
  // Get the port from the arguments
  int cases = 0;
  if(argc == 3){
    port = atoi(argv[2]);
    if(strcmp(argv[1],"-f") == 0){
      cases = 1;
    }else if(strcmp(argv[1],"-t") == 0){
      cases = 2;
    }else{
      cases = 3;
    }
  }else if(argc == 2){
    if(isnumber(argv[1]) == 1){
      port = atoi( argv[1] );
      cases = 0;
    }else {
      if(strcmp(argv[1],"-f") == 0){
      cases = 1;
      }else if(strcmp(argv[1],"-t") == 0){
      cases = 2;
      }else if(strcmp(argv[1],"-p") == 0){
      cases = 3;
      }
      port = 8000;
    }
  }else if(argc == 1){
    cases = 0;
    port = 8000;
  }
  // Set the IP address and port for this server
  struct sockaddr_in serverIPAddress; 
  memset( &serverIPAddress, 0, sizeof(serverIPAddress) );
  serverIPAddress.sin_family = AF_INET;
  serverIPAddress.sin_addr.s_addr = INADDR_ANY;
  serverIPAddress.sin_port = htons((u_short) port);
  
  // Allocate a socket
  int masterSocket =  socket(PF_INET, SOCK_STREAM, 0);
  if ( masterSocket < 0) {
   // perror("socket");
    exit( -1 );
  }

  // Set socket options to reuse port. Otherwise we will
  // have to wait about 2 minutes before reusing the sae port number
  int optval = 1; 
  int err = setsockopt(masterSocket, SOL_SOCKET, SO_REUSEADDR, 
		       (char *) &optval, sizeof( int ) );
   
  // Bind the socket to the IP address and port
  int error = bind( masterSocket,
		    (struct sockaddr *)&serverIPAddress,
		    sizeof(serverIPAddress) );
  if ( error ) {
   // perror("bind");
    exit( -1 );
  }
  
  // Put socket in listening mode and set the 
  // size of the queue of unprocessed connections
  error = listen( masterSocket, QueueLength);
  if ( error ) {
   // perror("listen");
    exit( -1 );
  }
  if(cases == 0){
    while ( 1 ) {

      // Accept incoming connections
      struct sockaddr_in clientIPAddress;
      int alen = sizeof( clientIPAddress );
      int slaveSocket = accept( masterSocket,
  			      (struct sockaddr *)&clientIPAddress,
  			      (socklen_t*)&alen);

     
     /* if ( slaveSocket < 0  ) {
        perror( "accept" );
        exit( -1 );
      }*/
       
      // Process request.
      processTimeRequest( slaveSocket );

      // Close socket
      close( slaveSocket );
      }
  } else if(cases == 1){
    printf("case 1\n"); 

    while ( 1 ) {

      // Accept incoming connections
      struct sockaddr_in clientIPAddress;
      int alen = sizeof( clientIPAddress );
      int slaveSocket = accept( masterSocket,
              (struct sockaddr *)&clientIPAddress,
              (socklen_t*)&alen);

      if ( slaveSocket < 0 ) {
      //  perror( "accept" );
        exit( -1 );
      }
      pid_t f = fork();
      if(f == 0){
      // Process request.
      processTimeRequest( slaveSocket );
      close( slaveSocket );
      exit(EXIT_SUCCESS);
      }
      /* if(slaveSocket == -1 && errno == EINTR){
          continue;
        }*/

      close(slaveSocket);
      
      }
  }
  else if(cases == 2){
    printf("case 2\n"); 

    while ( 1 ) {

      // Accept incoming connections
      struct sockaddr_in clientIPAddress;
      int alen = sizeof( clientIPAddress );
      int slaveSocket = accept( masterSocket,
              (struct sockaddr *)&clientIPAddress,
              (socklen_t*)&alen);

      if ( slaveSocket < 0 ) {
      //  perror( "accept" );
        exit( -1 );
      }else{
        pthread_t t1;      
        pthread_attr_t attr;

        pthread_attr_init( &attr ); 
        pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
      // Process request.
        pthread_create(&t1, &attr, (void * (*) (void*)) processTimeRequest,(void *) slaveSocket);
//      processTimeRequest( slaveSocket );
       // close( slaveSocket );
       // pthread_join(t1,NULL);    
    	}
      
      }
  }else if(cases == 3){
    printf("case3\n");
    pthread_t tid[5];
    pthread_attr_t attr;

    pthread_attr_init( &attr ); 
    pthread_attr_setscope(&attr, PTHREAD_SCOPE_SYSTEM);
    for (int i = 0; i < 5; i++)
    {
       pthread_create(&tid[i], &attr, (void * (*) (void*)) poolRequest,(void *) masterSocket);
    }

    pthread_join(tid[0],NULL);
  }
}

void poolRequest(int fd){
  while(1){
      struct sockaddr_in clientIPAddress;
      int alen = sizeof( clientIPAddress );
      pthread_mutex_lock(&mutex);
      int slaveSocket = accept( fd,
              (struct sockaddr *)&clientIPAddress,
              (socklen_t*)&alen);

      if ( slaveSocket < 0 ) {
       // perror( "accept" );
        exit( -1 );
      }
      pthread_mutex_unlock(&mutex);
      
      // Process request.
      processTimeRequest( slaveSocket );
      
      close(slaveSocket);
      
  }
}

void
processTimeRequest( int fd )
{
  // Buffer used to store the name received from the client
  //const int MaxName = 1024;
 // char name[ MaxName + 1 ];
  char * name = (char*)malloc(1024);
  int nameLength = 0;
  int n;
  char newChar;
  int gotGet = 0;
  int DOC = 0;
  int count = 0;
  int f = 0;
  char ff[1024];
  char * docpath = (char*)malloc(1024);
  char oldChar;
  char oldChar1;
  char oldChar2;
  int counter = 0;
  while( read(fd,&newChar,sizeof(newChar)) ){
    
    printf("%c", newChar);
    //if(counter == 1){

      //printf(" %d \n",f++);
    //}else{
    if(newChar == ' '){
     // printf(" g1\n");
      if(DOC == 0){
        if(gotGet == 0){
           gotGet = 1;
        }else if(gotGet == 1){
        //name[nameLength-1] = '\n';
       // nameLength++;

        name[nameLength] = 0;
        
        strcpy(docpath,name);
       // printf("name aaa %s\n",docpath );
        DOC = 1;
        gotGet = 0;
       // break;
        }
      }
    }else if(newChar == '\n' && oldChar == '\r' && oldChar1 == '\n' && oldChar2 == '\r' ){
          //counter = 1;
        printf("break\n" );
      break;
    }else if(gotGet == 1 ){
      //printf("name : %c \n",newChar );
      oldChar = newChar;
      name[nameLength] = newChar;
      nameLength++;
    }else{
      //printf("%c\n",newChar );
      //if(counter == 0){

        oldChar2 = oldChar1;
        oldChar1 = oldChar;
        oldChar = newChar;

     //   printf(" counter %d oldChar2 %c\n",counter,oldChar2 );
    //  }
      //if(counter == 1){
        //oldChar1 = newChar;
         //printf(" counter1 %d oldChar1 %c\n",counter,oldChar1 );
      //}
      //if(counter == 2){
        //oldChar = newChar;
        //printf(" counter1 %d oldChar %c\n",counter,oldChar );
        //counter = 0;
      //}else{ 
       
      //}
    }
  //}
  }

  /*char br[1024];
  int hh = -1;
  while((n = read(fd,&newChar,sizeof(newChar))) > 0){
      if(br[hh-3] == '\r' && br[hh-2] == '\n' && br[hh-1] == '\r' && br[hh] == '\n' ){
        printf("aaa\n");
        break;
      }
      else{
        
        br[++hh] = newChar;
        if(newChar == '\n'){
          printf("nnnn");
        }else if( newChar == '\r'){
          printf("rrrrrr");
        }else
        printf("%c",br[hh]);
      
      }
  }*/

//printf("%s\n",docpath );
  char *cwd = {0};
  cwd = getcwd(cwd,256);
  char*miin1 = (char*)malloc(1024);
  int cgi = 0;
  if(docpath[0] == '/' && docpath[1] != '\0'){
        if(docpath[1] == 'i' && docpath[2] == 'c' && docpath[3] == 'o' && docpath[4] == 'n' && docpath[5] == 's'){
          strcat(cwd,"/http-root-dir/");
          strcat(miin1,cwd);
          strcat(miin1,docpath);
        }else if(strstr(docpath,"cgi-bin")){
              strcat(cwd,"/http-root-dir");
              strcat(miin1,cwd);
              strcat(miin1,docpath);
         // cgi = 1;
              setenv("QUERY_STRING","", 1);
          //printf("aaaa %s \n",miin1 );
            char array[100];
            char path[100];
            int mm = 0;
            if(strstr(miin1,"?") != NULL ){
              for (int i = 0; i < strlen(miin1); i++)
              {
                
                if(miin1[i] == '?'){
                  path[mm] = '\0';
                  int ll = 0;
                  int no = 0;
                  int j;
                   for ( j = i+1; j < strlen(miin1);j++ ){
                        array[ll] = miin1[j];
                        ll++;      
                   }            
                   array[ll] = '\0';   
                   i = j;                         
                }else{
                  path[mm] = miin1[i];
                  mm++; 
                }
              }
              printf("pathhhh  %s\n",path );
              char *qs = (char*)malloc(100);
              //strcat(qs, "QUERY_STRING");
              //strcat(qs, array);

              printf("%s\n",array );
              setenv("QUERY_STRING",array, 1);
              setenv("REQUEST_METHOD","GET",1);
              strcpy(miin1,path);

              int f = fork();
              
              if(f == 0){
                dup2(fd,1);
                printf("HTTP/1.0 200 Document follows\r\n");
                printf("Server: cs252/lab4 \r\n");
                execlp(path,path,NULL);
              }
            }/*else if(strstr(miin1,"?") != NULL && strstr(miin1,"+") != NULL){
              
              // cgi = 1;
              setenv("QUERY_STRING","", 1);
              //printf("aaaa %s \n",miin1 );
              char *array[100];
              char array1[100];
              char path[100];
              int mm = 0;
              char *temp;
              strcpy(temp,miin1);

               for (int i = 0; i < strlen(miin1); i++)
              {
                
                if(miin1[i] == '?'){
                  path[mm] = '\0';
                  int ll = 0;
                  int no = 0;
                  int j;
                   for ( j = i+1; j < strlen(miin1);j++ ){
                        array1[ll] = miin1[j];
                        ll++;      
                   }            
                   array1[ll] = '\0';   
                   i = j;                         
                }else{
                  path[mm] = miin1[i];
                  mm++; 
                }
              }


              mm = 0;
              printf("hhhhhhhh %s\n",temp );
              int no = 0;
              for (int i = 0; i < strlen(temp); i++)
              {
                
                if(temp[i] == '?'){
                  path[mm] = '\0';
                  int ll = 0;
                 
                  int j;

                   for ( j = i+1; j < strlen(temp);j++ ){
                        if(temp[j] == '+'){
                          array[no][ll] = '\0';
                          no++;
                          ll = 0;
                        }else{
                          array[no][ll] = temp[j];
                          ll++; 
                        }     
                   }            
                  // array[ll] = '\0';   
                   i = j;                         
                }else{
                  path[mm] = temp[i];
                  mm++; 
                }
              }
              printf("pathhhh  %s\n",path );
              char *qs = (char*)malloc(100);
              //strcat(qs, "QUERY_STRING");
              //strcat(qs, array);
              for (int i = 0; i < no; i++)
              {
                   printf("lll %s\n",array[i] );
              }
             
              printf("aaaaaaaaa%s\n",path );
              
              setenv("QUERY_STRING",array1, 1);
              setenv("REQUEST_METHOD","GET",1);
              //strcpy(miin1,path);

              int f = fork();
              
              if(f == 0){
                dup2(fd,1);
                printf("HTTP/1.0 200 Document follows\r\n");
                printf("Server: cs252/lab4 \r\n");
              
                execlp(path,path,array1[0],NULL);
              }
            }*/
            else{
             // putenv(strdup("REQUEST_METHOD = GET"));
              int f = fork(); 
               if(f == 0){
                dup2(fd,1);
                printf("HTTP/1.0 200 Document follows\r\n");
                printf("Server: cs252/lab4 \r\n");
                execlp(miin1,miin1,NULL);
              }
           
           }
           //fl
          close(fd);
           return;
        }
        else if(strstr(docpath,"/htdocs")){
          strcat(cwd,"/http-root-dir/");
          strcat(miin1,cwd);
          strcat(miin1,docpath);
        }else{
          strcat(cwd,"/http-root-dir/htdocs");
          strcat(miin1,cwd);
          strcat(miin1,docpath);
        }
  }else if(docpath[0] == '/' && docpath[1] == '\0'){
       strcat(cwd,"/http-root-dir/htdocs/index.html");
       strcat(miin1,cwd);
  }
  int i = 0;
  //printf("miin1: %s\n",miin1 );
  char *contentType = (char*)malloc(1028); 
  if(strstr(miin1,".html") || strstr(miin1,".html/")){
    strcpy(contentType,"text/html");
  }else
  if(strstr(miin1,".gif") || strstr(miin1,".gif/")){
    strcpy(contentType,"image/gif");
    i = 1;
  }else{
    strcpy(contentType, "text/plain");
  }

 /* while((n = read(fd,&newChar,sizeof(newChar))) > 0){
    if(Buffer[0] == '\n' && Buffer[1] == '\r' && Buffer[2] == '\n' && Buffer[3] == '\r'){
      break;
    }else{
      Buffer[i] = newChar;
      i++; 
    }
  }*/
  //write(fd,miin1,strlen(miin1));  
 // printf("%s\n",miin1 );
  //FILE* g;
  int k;
  /*if(cgi == 1 ){
      printf("aaaa\n");
     g = popen(miin1,"r");
  }*/
  //else{
     k = open(miin1,O_RDONLY);
  //} 
  printf(" cgi : %s\n",miin1);
  const char * a1  = "HTTP/1.0 404 File Not Found \r\n";
  const char * a2  = "Server: cs252/lab4 \r\n";
  const char * a3  = "Content-type: "; 
  const char * a4  = (const char*)contentType;
  const char * a5  = "\r\n\r\nERROR"; 

  const char * b1 = "HTTP/1.0 200 Document follows\r\n";
  const char * b2 = "Server: CS 252 lab4\r\n";
  const char * b3  = "Content-type: "; 
  const char * b4  = (const char*)contentType; 
  const char * b5  = "\r\n\r\n";
  

  /*if(cgi == 1){
    if(g == NULL){
      printf("ERRRR\n");
    	write(fd,a1,strlen(a1));
    	write(fd,a2,strlen(a2));
    	write(fd,a3,strlen(a3));
    	write(fd,a4,strlen(a4));
    	write(fd,a5,strlen(a5)); 
    }else{
      int kk;
      kk = fileno(g); 
        
      write(fd,b1,strlen(b1));
      write(fd,b2,strlen(b2));
      write(fd,b3,strlen(b3));
      printf("%s\n",contentType );
      write(fd,b5,strlen(b5));
      //printf("%s\n",miin1 );
      int l = 0;
      char car;
      while(( l = read(kk,&car,sizeof(car))) > 0){
        if(write(fd,&car,sizeof(car)) != l){
          perror("write");
        }
      }
      pclose(g);

     // sleep(100);
    }
  }else{*/
    if(k <= 0){
      write(fd,a1,strlen(a1));
      write(fd,a2,strlen(a2));
      write(fd,a3,strlen(a3));
      write(fd,a4,strlen(a4));
      write(fd,a5,strlen(a5));
    
    }else{
      
      write(fd,b1,strlen(b1));
      write(fd,b2,strlen(b2));
      write(fd,b3,strlen(b3));
      printf("%s\n",contentType );
      write(fd,b5,strlen(b5));
      //printf("%s\n",miin1 );
      int l = 0;
      char car;
      while(( l = read(k,&car,sizeof(car))) > 0){
        if(write(fd,&car,sizeof(car)) != l){
         // perror("write");
        }
      }
      close(k);
     // sleep(100);
    }
 // }
 // write( fd, docpath, strlen( docpath ) );
  // Send prompt
 /* char newbuf[1024];
  read(fd,&newbuf,sizeof(newbuf));
  printf("%s\n",newbuf);
  const char * prompt = "\nType your name:";
  write( fd, prompt, strlen( prompt ) );

  // Currently character read
  unsigned char newChar;

  // Last character read
  unsigned char lastChar = 0;

  //
  // The client should send <name><cr><lf>
  // Read the name of the client character by character until a
  // <CR><LF> is found.
  //
    
  while ( nameLength < MaxName &&
	  ( n = read( fd, &newChar, sizeof(newChar) ) ) > 0 ) {

    if ( lastChar == '\015' && newChar == '\012' ) {
      // Discard previous <CR> from name
      nameLength--;
      break;
    }

    name[ nameLength ] = newChar;
    nameLength++;

    lastChar = newChar;
  }

  // Add null character at the end of the string
  name[ nameLength ] = 0;

  printf( "name=%s\n", name );

  // Get time of day
  time_t now;
  time(&now);
  char	*timeString = ctime(&now);

  // Send name and greetings
  const char * hi = "\nHi ";
  const char * timeIs = " the time is:\n";
  write( fd, hi, strlen( hi ) );
  write( fd, name, strlen( name ) );
  write( fd, timeIs, strlen( timeIs ) );
  
  // Send the time of day 
  write(fd, timeString, strlen(timeString));

  // Send last newline
  const char * newline="\n";
  write(fd, newline, strlen(newline));*/
  close(fd);

}

