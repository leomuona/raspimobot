#include "remote.h"

#include "audio.h"
#include "camera.h"
#include "motor.h"

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include <string.h>

#define PORT 555
#define BUFFER_SIZE 64

pthread_t thread;
int f_listen = 0;

int remote_init()
{
	if (f_listen){
		printf("ERROR initializing remote control, already listening\n");
		return -1;
	}
	f_listen = 1;

	thread = pthread_create(&thread, NULL, (void *) remote_listen, NULL);

	return 0;
}

int remote_listen()
{
	int socket_desc = socket(AF_INET, SOCK_STREAM, 0);
	if (socket_desc < 0){
		printf("ERROR opening socket\n");
		return -1;
	}

	struct sockaddr_in server_addr, client_addr;

	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(PORT);
	server_addr.sin_addr.s_addr = INADDR_ANY;

	if (bind(socket_desc, (struct sockaddr *) &server_addr, 
		sizeof(server_addr)) < 0){
		printf("ERROR binding socket to port %d\n", PORT);
		return -1;
	}

	// this function call blocks the thread
	while(f_listen){
		listen(socket_desc, 5);

		socklen_t clilen = sizeof(client_addr);
		int new_socket_desc = accept(socket_desc, (struct sockaddr *) &client_addr, 
			&clilen);

		if (new_socket_desc < 0){
			printf("ERROR accepting client\n");
			return -1;
		}

		char buffer[BUFFER_SIZE];
		memset(&buffer, 0, BUFFER_SIZE);
		int n;

		// loop until eol
		while ((n == read(new_socket_desc, buffer, BUFFER_SIZE)) > 0){
			// TODO parse input to control command
			printf("%s\n", buffer);
		}

		close(new_socket_desc);
	}

	close(socket_desc);
	return 0;
}

int remote_close()
{
	f_listen = 0;
	pthread_join(thread, NULL);
	printf("Socket thread joined\n");
	return 0;
}

int remote_take_pic()
{
	printf("remote take pic\n");

	take_pic();

	return 0;
}

int remote_play_sound(char* filename)
{
	printf("remote play sound %s\n", filename);

	play_sound(filename);
}

int remote_move(int x, int y)
{
	printf("remote move x=%d, y=%d\n", x, y);

	move_x();
	move_y();
}
