#include <netinet/in.h>
#include <sys/socket.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define BACKLOG 5 /* Number of connections to queue up */
#define CLIENT_REQUEST_BUFFER_SIZE 2048


int main(void) {
  int socket_fd;

  socket_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (socket_fd < 0) {
    perror("Couldn't create socket: ");
    return 1;
  }

  struct sockaddr_in socket_address = {
    .sin_family = AF_INET,
    .sin_port   = htons(8080),
  };

  if (bind(socket_fd, (struct sockaddr*)&socket_address, sizeof(socket_address)) < 0) {
    perror("Couldn't bind socket to port 8080: ");
    return 1;
  }


  if (listen(socket_fd, BACKLOG) < 0) {
    perror("Couldn't listen on socket: ");
    return 1;
  }

  int quit = 0;
  int client_fd;
  char client_request_buffer[CLIENT_REQUEST_BUFFER_SIZE] = {0};
  ssize_t bytes_read;
  const char *client_error = "<h1>Client Error. Invalid Request</h1>";
  const char *hello_world_response = "<h1>Hello World!</h1>";
  while (!quit) {
    client_fd = accept(socket_fd, NULL, NULL);
    if (client_fd < 0) {
      perror("Issue accepting connection from client: ");
      continue;
    }
  
    bytes_read = recv(client_fd, client_request_buffer, CLIENT_REQUEST_BUFFER_SIZE - 1, 0);
    if (bytes_read == 0) {
      printf("Client has shutdown connection.\n");
      close(client_fd);
      continue;
    }
    if (bytes_read == -1) {
      perror("Issue reading from client: ");
      close(client_fd);
      continue;
    }

    if (memcmp(client_request_buffer, "GET ", 4)) {
      write(client_fd, client_error, (size_t)strlen(client_error)); 

      close(client_fd);
      memset(client_request_buffer, 0, CLIENT_REQUEST_BUFFER_SIZE);
    } else {
      write(client_fd, hello_world_response, (size_t)strlen(hello_world_response)); 
    }
    
    close(client_fd);
    memset(client_request_buffer, 0, CLIENT_REQUEST_BUFFER_SIZE);
  }
}
