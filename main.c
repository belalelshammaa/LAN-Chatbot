#include "client.h"
#include "network.h"
#include <stdio.h>
#include <sys/select.h>
int main(int argc, char *argv[]) {
  int final_pipe = -1;
  if (argc > 1) {

    final_pipe = start_client(argv[1]);

  } else {
    printf("ERROR: Please specify <IP_ADDRESS>' as command "
           "line arguments.\n");
    return 1;
  }

  if (final_pipe == -1) {
    printf("ERROR: Failed to establish a connection.\n");
    return 1;
  }
  return client_loop(final_pipe);
}
