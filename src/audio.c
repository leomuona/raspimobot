#include "audio.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>

#include <sndfile.h>

#include <pulse/pulseaudio.h>

static const char *client_name = "raspimobot";
static const char *stream_name = "raspimobot_stream";
static const char *device = NULL;

static int playing = 0;

struct PAContext
{
	pa_context *context;
	pa_mainloop *main;
	pa_mainloop_api *mainloop_api;
	pa_stream *stream;
	pa_sample_spec sample_spec;

        SNDFILE *sndfile;
	sf_count_t (*readf_function)(SNDFILE *_sndfile, void *ptr, sf_count_t frames);
};

typedef struct PAContext PAContext;

static void quit(int ret, PAContext *ctx)
{
	fprintf(stdout, "Quitting (%d).\n", ret);
	assert(ctx->mainloop_api);
	ctx->mainloop_api->quit(ctx->mainloop_api, ret);
}

/* Connection draining complete */
static void context_drain_complete(pa_context *c, void *userdata)
{
	pa_context_disconnect(c);
}

/* Stream draining complete */
static void stream_drain_complete(pa_stream *s, int success, void *userdata)
{
	assert(userdata);
	PAContext *ctx = (PAContext *) userdata;

	pa_operation *o;

	if (!success) {
		fprintf(stderr, "Failed to drain stream: %s\n", pa_strerror(pa_context_errno(ctx->context)));
		quit(1, ctx);
	}

	fprintf(stdout, "Playback stream drained.\n");

	pa_stream_disconnect(ctx->stream);
	pa_stream_unref(ctx->stream);
	ctx->stream = NULL;

	if (!(o = pa_context_drain(ctx->context, context_drain_complete, NULL))) {
		pa_context_disconnect(ctx->context);
	} else {
		pa_operation_unref(o);
		fprintf(stdout, "Draining connection to server.\n");
	}
}

static void stream_write_callback(pa_stream *s, size_t length, void *userdata)
{
	sf_count_t bytes;
	void *data;
	assert(s && length);
	assert(userdata);
	PAContext *ctx = (PAContext *) userdata;

	if (!ctx->sndfile)
		return;

	data = pa_xmalloc(length);

	if (ctx->readf_function) {
		size_t k = pa_frame_size(&ctx->sample_spec);

		if ((bytes = ctx->readf_function(ctx->sndfile, data, (sf_count_t) (length/k))) > 0)
			bytes *= (sf_count_t) k;

	} else {
		bytes = sf_read_raw(ctx->sndfile, data, (sf_count_t) length);
	}

	if (bytes > 0) {
		pa_stream_write(s, data, (size_t) bytes, pa_xfree, 0, PA_SEEK_RELATIVE);
	} else {
		pa_xfree(data);
	}

	if (bytes < (sf_count_t) length) {
		sf_close(ctx->sndfile);
		ctx->sndfile = NULL;
		pa_operation_unref(pa_stream_drain(s, stream_drain_complete, userdata));
	}

}

static void stream_state_callback(pa_stream *s, void *userdata)
{
	assert(s);
	assert(userdata);
	PAContext *ctx = (PAContext *) userdata;

	switch (pa_stream_get_state(s)) {
		case PA_STREAM_CREATING:
		case PA_STREAM_TERMINATED:
			break;

		case PA_STREAM_READY:
			fprintf(stdout, "Stream successfully created\n");
			break;

		case PA_STREAM_FAILED:
		default:
			fprintf(stderr, "Stream error: %s\n", pa_strerror(pa_context_errno(pa_stream_get_context(s))));
			quit(1, ctx);
	}
}

/* This is called whenever the context status changes */
static void context_state_callback(pa_context *c, void *userdata)
{
	assert(c);
	assert(userdata);
	PAContext *ctx = (PAContext *) userdata;

	switch (pa_context_get_state(c)) {
		case PA_CONTEXT_CONNECTING:
		case PA_CONTEXT_AUTHORIZING:
		case PA_CONTEXT_SETTING_NAME:
			break;

		case PA_CONTEXT_READY:
			assert(!ctx->stream);
			fprintf(stdout, "Connection established.\n");

			ctx->stream = pa_stream_new(c, stream_name, &ctx->sample_spec, NULL);
			assert(ctx->stream);

			pa_stream_set_state_callback(ctx->stream, stream_state_callback, userdata);
			pa_stream_set_write_callback(ctx->stream, stream_write_callback, userdata);
			pa_stream_connect_playback(ctx->stream, device, NULL, 0, NULL, NULL);
			break;

		case PA_CONTEXT_TERMINATED:
			quit(0, ctx);
			break;

		case PA_CONTEXT_FAILED:
		default:
			fprintf(stderr, "Connection failure: %s\n", pa_strerror(pa_context_errno(c)));
			quit(1, ctx);
	}
}

static void exit_signal_callback(
	pa_mainloop_api *m, pa_signal_event *e, int sig, void *userdata)
{
	assert(userdata);
	PAContext *ctx = (PAContext *) userdata;

	fprintf(stderr, "Got SIGINT.\n");
	quit(0, ctx);
}

void init_sample_spec(PAContext *ctx, SF_INFO *sfinfo)
{
	pa_sample_spec *sample_spec = &ctx->sample_spec;

	sample_spec->rate = (uint32_t) sfinfo->samplerate;

	sample_spec->channels = (uint8_t) sfinfo->channels;
	//sample_spec->channels = 1; // hardcode mono.

	switch (sfinfo->format & 0xFF) {
		case SF_FORMAT_PCM_16:
		case SF_FORMAT_PCM_U8:
		case SF_FORMAT_PCM_S8:
			sample_spec->format = PA_SAMPLE_S16NE;
			ctx->readf_function = (sf_count_t (*)(SNDFILE *_sndfile, void *ptr, sf_count_t frames)) sf_readf_short;
			break;

		case SF_FORMAT_ULAW:
			sample_spec->format = PA_SAMPLE_ULAW;
			break;

		case SF_FORMAT_ALAW:
			sample_spec->format = PA_SAMPLE_ALAW;
			break;

		case SF_FORMAT_FLOAT:
		case SF_FORMAT_DOUBLE:
		default:
			sample_spec->format = PA_SAMPLE_FLOAT32NE;
			ctx->readf_function = (sf_count_t (*)(SNDFILE *_sndfile, void *ptr, sf_count_t frames)) sf_readf_float;
			break;
	}

	assert(pa_sample_spec_valid(sample_spec));

	char t[PA_SAMPLE_SPEC_SNPRINT_MAX];
	pa_sample_spec_snprint(t, sizeof(t), sample_spec);
	fprintf(stdout, "Using sample spec '%s'\n", t);
}

static void *play(void *arg)
{
	PAContext *ctx = (PAContext *) arg;

	if (pa_mainloop_run(ctx->main, NULL) < 0) {
		fprintf(stderr, "pa_mainloop_run() failed.\n");
	}

	if (ctx->stream)
		pa_stream_unref(ctx->stream);

	if (ctx->context)
		pa_context_unref(ctx->context);

	if (ctx->main) {
		pa_signal_done();
		pa_mainloop_free(ctx->main);
	}

	free(ctx);
	playing = 0;
}

int play_sound(const char *filename)
{
	int ret = -1, r;
	SF_INFO sfinfo;

	struct PAContext *ctx = malloc(sizeof(PAContext));
	memset(ctx, 0, sizeof(PAContext));

	memset(&sfinfo, 0, sizeof(sfinfo));
	ctx->sndfile = sf_open(filename, SFM_READ, &sfinfo);
	if (!ctx->sndfile) {
		fprintf(stderr, "Failed to open file '%s'\n", filename);
		goto quit;
	}

	init_sample_spec(ctx, &sfinfo);

	/* Set up a new main loop */
	if (!(ctx->main = pa_mainloop_new())) {
		fprintf(stderr, "pa_mainloop_new() failed.\n");
		goto quit;
	}

	ctx->mainloop_api = pa_mainloop_get_api(ctx->main);

	if ((r = pa_signal_init(ctx->mainloop_api)) != 0) {
		fprintf(stderr, "pa_signal_init() failed. (%d)\n", r);
		goto quit;
	}

	pa_signal_new(SIGINT, exit_signal_callback, ctx);

	/* Create a new connection context. */
	if (!(ctx->context = pa_context_new(ctx->mainloop_api, client_name))) {
		fprintf(stderr, "pa_context_new() failed.\n");
		goto quit;
	}

	pa_context_set_state_callback(ctx->context, context_state_callback, ctx);

	/* Connect the context */
	if (pa_context_connect(ctx->context, NULL, 0, NULL) < 0) {
		fprintf(stderr, "pa_context_connect() failed: %s", pa_strerror(pa_context_errno(ctx->context)));
		goto quit;
	}

	// create a thread that waits for the song to stop.
	pthread_t threadh;
	if (pthread_create(&threadh, NULL, play, (void *)ctx) != 0) {
		fprintf(stderr, "error creating thread.\n");
		goto quit;
	}

	ret = 0;
	playing = 1;
	return ret;

quit:
	if (ctx->stream)
		pa_stream_unref(ctx->stream);

	if (ctx->context)
		pa_context_unref(ctx->context);

	if (ctx->main) {
		pa_signal_done();
		pa_mainloop_free(ctx->main);
	}

	return ret;
}

int is_playing()
{
	return is_playing;
}
