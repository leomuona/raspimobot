#include "audio.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>

#include <sndfile.h>

#include <pulse/pulseaudio.h>

static const char *client_name = "raspimobot";
static const char *stream_name = "raspimobot_stream";
static const char *device = NULL;

static pa_context *context = NULL;
static pa_mainloop_api *mainloop_api = NULL;
static pa_stream *stream = NULL;

static SNDFILE *sndfile = NULL;

static sf_count_t (*readf_function)(SNDFILE *_sndfile, void *ptr, sf_count_t frames) = NULL;

static pa_sample_spec sample_spec = { 0, 0, 0 };

static void quit(int ret)
{
	fprintf(stdout, "Quitting (%d).\n", ret);
	assert(mainloop_api);
	mainloop_api->quit(mainloop_api, ret);
}

/* Connection draining complete */
static void context_drain_complete(pa_context *c, void *userdata)
{
	pa_context_disconnect(c);
}

/* Stream draining complete */
static void stream_drain_complete(pa_stream *s, int success, void *userdata)
{
	pa_operation *o;

	if (!success) {
		fprintf(stderr, "Failed to drain stream: %s\n", pa_strerror(pa_context_errno(context)));
		quit(1);
	}

	fprintf(stdout, "Playback stream drained.\n");

	pa_stream_disconnect(stream);
	pa_stream_unref(stream);
	stream = NULL;

	if (!(o = pa_context_drain(context, context_drain_complete, NULL))) {
		pa_context_disconnect(context);
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

	if (!sndfile)
		return;

	data = pa_xmalloc(length);

	if (readf_function) {
		size_t k = pa_frame_size(&sample_spec);

		if ((bytes = readf_function(sndfile, data, (sf_count_t) (length/k))) > 0)
			bytes *= (sf_count_t) k;

	} else {
		bytes = sf_read_raw(sndfile, data, (sf_count_t) length);
	}

	if (bytes > 0) {
		pa_stream_write(s, data, (size_t) bytes, pa_xfree, 0, PA_SEEK_RELATIVE);
	} else {
		pa_xfree(data);
	}

	if (bytes < (sf_count_t) length) {
		sf_close(sndfile);
		sndfile = NULL;
		pa_operation_unref(pa_stream_drain(s, stream_drain_complete, NULL));
	}

}

static void stream_state_callback(pa_stream *s, void *userdata)
{
	assert(s);

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
			quit(1);
	}
}

/* This is called whenever the context status changes */
static void context_state_callback(pa_context *c, void *userdata)
{
	assert(c);

	switch (pa_context_get_state(c)) {
		case PA_CONTEXT_CONNECTING:
		case PA_CONTEXT_AUTHORIZING:
		case PA_CONTEXT_SETTING_NAME:
			break;

		case PA_CONTEXT_READY:
			assert(!stream);
			fprintf(stdout, "Connection established.\n");

			stream = pa_stream_new(c, stream_name, &sample_spec, NULL);
			assert(stream);

			pa_stream_set_state_callback(stream, stream_state_callback, NULL);
			pa_stream_set_write_callback(stream, stream_write_callback, NULL);
			pa_stream_connect_playback(stream, device, NULL, 0, NULL, NULL);
			break;

		case PA_CONTEXT_TERMINATED:
			quit(0);
			break;

		case PA_CONTEXT_FAILED:
		default:
			fprintf(stderr, "Connection failure: %s\n", pa_strerror(pa_context_errno(c)));
			quit(1);
	}
}

static void exit_signal_callback(
	pa_mainloop_api *m, pa_signal_event *e, int sig, void *userdata)
{
	fprintf(stderr, "Got SIGINT.\n");
	quit(0);
}

void init_sample_spec(pa_sample_spec *sample_spec, SF_INFO *sfinfo)
{
	sample_spec->rate = (uint32_t) sfinfo->samplerate;

	sample_spec->channels = (uint8_t) sfinfo->channels;
	//sample_spec->channels = 1; // hardcode mono.

	switch (sfinfo->format & 0xFF) {
		case SF_FORMAT_PCM_16:
		case SF_FORMAT_PCM_U8:
		case SF_FORMAT_PCM_S8:
			sample_spec->format = PA_SAMPLE_S16NE;
			readf_function = (sf_count_t (*)(SNDFILE *_sndfile, void *ptr, sf_count_t frames)) sf_readf_short;
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
			readf_function = (sf_count_t (*)(SNDFILE *_sndfile, void *ptr, sf_count_t frames)) sf_readf_float;
			break;
	}

	assert(pa_sample_spec_valid(sample_spec));

	char t[PA_SAMPLE_SPEC_SNPRINT_MAX];
	pa_sample_spec_snprint(t, sizeof(t), sample_spec);
	fprintf(stdout, "Using sample spec '%s'\n", t);
}

int play_sound(const char *filename)
{
	pa_mainloop *m = NULL;
	int ret = -1, r;
	SF_INFO sfinfo;

	memset(&sfinfo, 0, sizeof(sfinfo));
	sndfile = sf_open(filename, SFM_READ, &sfinfo);
	if (!sndfile) {
		fprintf(stderr, "Failed to open file '%s'\n", filename);
		goto quit;
	}

	readf_function = NULL;
	init_sample_spec(&sample_spec, &sfinfo);

	/* Set up a new main loop */
	if (!(m = pa_mainloop_new())) {
		fprintf(stderr, "pa_mainloop_new() failed.\n");
		goto quit;
	}

	mainloop_api = pa_mainloop_get_api(m);

	if ((r = pa_signal_init(mainloop_api)) != 0) {
		fprintf(stderr, "pa_signal_init() failed. (%d)\n", r);
		goto quit;
	}

	pa_signal_new(SIGINT, exit_signal_callback, NULL);

	/* Create a new connection context. */
	if (!(context = pa_context_new(mainloop_api, client_name))) {
		fprintf(stderr, "pa_context_new() failed.\n");
		goto quit;
	}

	pa_context_set_state_callback(context, context_state_callback, NULL);

	/* Connect the context */
	if (pa_context_connect(context, NULL, 0, NULL) < 0) {
		fprintf(stderr, "pa_context_connect() failed: %s", pa_strerror(pa_context_errno(context)));
		goto quit;
	}

	/* Run the main loop */
	if (pa_mainloop_run(m, &ret) < 0) {
		fprintf(stderr, "pa_mainloop_run() failed.\n");
		goto quit;
	}

quit:
	if (stream)
		pa_stream_unref(stream);

	if (context)
		pa_context_unref(context);

	if (m) {
		pa_signal_done();
		pa_mainloop_free(m);
	}

	return ret;
}
