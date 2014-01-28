#ifndef REMOTE_H_
#define REMOTE_H_

int remote_init();
int remote_listen();
int remote_close();
int remote_take_pic();
int remote_play_sound(char* filename);
int remote_move(int x, int y);

#endif
