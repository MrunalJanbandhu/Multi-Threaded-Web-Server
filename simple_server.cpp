/* run using ./server <port> */
#include "http_server.hh"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <pthread.h>
#include <queue>

#define SIZE_THREAD_POOL 20

pthread_t thread_pool[SIZE_THREAD_POOL];
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t condition_variable = PTHREAD_COND_INITIALIZER;
pthread_cond_t condition_queue_count = PTHREAD_COND_INITIALIZER;
queue<int> thread_queue;

void error(char *msg) {
  perror(msg);
  exit(1);
}

void *serv_function(void *arg){
  int sockfd, portno;
  socklen_t clilen;
  char buffer[50000];
  struct sockaddr_in serv_addr, cli_addr;
  int n;
  int newsockfd;

  // int newsockfd = *(int *) arg;

  /* read message from client */

  while(1){    

    pthread_mutex_lock(&mutex);

    while ( (thread_queue.empty() )){
      pthread_cond_wait(&condition_variable, &mutex);
    }

    newsockfd= thread_queue.front();
    thread_queue.pop();
      
    pthread_mutex_unlock(&mutex);

    bzero(buffer, 50000);
    n = read(newsockfd, buffer, 49999);
    if (n < 0)
      error("ERROR reading from socket");

    if(n == 0){
      close(newsockfd);
      continue;
    }
    // cout<<"---Msg from client: "<< buffer<<endl;

    HTTP_Response *response_obj;

    response_obj = handle_request(string(buffer));

    string res = response_obj->get_string();

    /* send reply to client */

    n = write(newsockfd, &res[0], res.length());
    if (n < 0)
      error("ERROR writing to socket");

    delete response_obj;
    close(newsockfd);

  }

  return NULL;

}

int main(int argc, char *argv[]) {
  int sockfd, newsockfd, portno;
  socklen_t clilen;
  char buffer[5000];
  struct sockaddr_in serv_addr, cli_addr;
  int n;


  if (argc < 2) {
    fprintf(stderr, "ERROR, no port provided\n");
    exit(1);
  }

  /* create socket */

 
  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd < 0)
    error("ERROR opening socket");

  /* fill in port number to listen on. IP address can be anything (INADDR_ANY)
   */

  bzero((char *)&serv_addr, sizeof(serv_addr));
  portno = atoi(argv[1]);
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = INADDR_ANY;
  serv_addr.sin_port = htons(portno);

  /* bind socket to this port number on this machine */

  if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    error("ERROR on binding");

  /* listen for incoming connection requests */

  for (int i; i < SIZE_THREAD_POOL; i++){
    // wait
    // pthread_create(&serv_thread, NULL, serv_function, &newsockfd); 
    pthread_create(&thread_pool[i], NULL, serv_function, &newsockfd);
  }

  listen(sockfd,512);
  clilen = sizeof(cli_addr);

  /* accept a new request, create a newsockfd */
  while(1){
    newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen);
    if (newsockfd < 0)
      error("ERROR on accept");
    else{
      pthread_t serv_thread;

      // mutex signal
      pthread_mutex_lock(&mutex);
      thread_queue.push(newsockfd);
      pthread_cond_signal(&condition_variable);
      pthread_mutex_unlock(&mutex);
      
      }
    }   
  
  return 0;
}