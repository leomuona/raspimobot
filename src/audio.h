#ifndef AUDIO_H
#define AUDIO_H

/**
 * Play given sound file (WAV).
 *
 * @param filename fo the file.
 *
 * @return 0 on success, anything else on error.
 */
int play_sound(const char *filename);

/**
 * Are we currently playing some sound.
 *
 * @return 0 on false, 1 on true.
 */
int is_playing();

#endif // AUDIO_H
