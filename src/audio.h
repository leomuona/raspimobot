#ifndef AUDIO_H
#define AUDIO_H

/**
 * Initializes the samples with the files from given directory.
 *
 * @param samples directory
 */
void init_samples(const char *dirname);

/**
 * Play one random sample from the initialized sample list.
 *
 * Uses c rand for selecting the sample, seeding should be done elsewhere.
 *
 * Doesn't block, just returns straight and doesn't tell the result.
 */
void play_random_sample();

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
