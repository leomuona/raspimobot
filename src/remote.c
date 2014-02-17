#include "remote.h"

#include "audio.h"
#include "bmp.h"
#include "camera.h"
#include "motor.h"
#include "logic.h"

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include <string.h>
#include <stdlib.h>

#define PORT 5555
#define BUFFER_SIZE 16

pthread_t thread;
int f_listen = 0;

int remote_init()
{
	if (f_listen){
		printf("ERROR initializing remote control, already listening");
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
		perror("ERROR opening socket");
		return -1;
	}

	struct sockaddr_in server_addr, client_addr;

	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(PORT);
	server_addr.sin_addr.s_addr = INADDR_ANY;

	if (bind(socket_desc, (struct sockaddr *) &server_addr, 
		sizeof(server_addr)) < 0){
		perror("ERROR binding socket");
		return -1;
	}

	// this function call blocks the thread
	while(f_listen){
		listen(socket_desc, 5);

		socklen_t clilen = sizeof(client_addr);
		int new_socket_desc = accept(socket_desc, 
			(struct sockaddr *) &client_addr, &clilen);

		if (new_socket_desc < 0){
			perror("ERROR accepting client");
			return -1;
		}

		char buffer[BUFFER_SIZE];
		int n = 0;

		int listen_client = 1;
		while (listen_client){
			memset(&buffer, 0, BUFFER_SIZE);
			n = read(new_socket_desc, buffer, BUFFER_SIZE);
			if (n < 1 || n == BUFFER_SIZE){
				listen_client = 0;
			}
			else if (strcmp(buffer, "take_pic\n") == 0){
				remote_take_pic();
			}
			else if (strcmp(buffer, "enable logic\n") == 0){
				enable_logic();
			}
			else if (strcmp(buffer, "disable logic\n") == 0){
				disable_logic();
			}
			else if (strcmp(buffer, "exit\n") == 0){
				listen_client = 0;
			}
			printf("%s", buffer);
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

	int width = 640;
	int height = 480;
	BMP *bmp = malloc(sizeof(BMP));
	memset(bmp, 0, sizeof(BMP));
	BMP_init(bmp, width, height);
	take_pic(bmp);
	BMP_free(bmp);
	free(bmp);
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

	rotate_x(x);
}
