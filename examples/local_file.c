#include <psp2/kernel/processmgr.h>
#include <vita2d.h>

#include "vita_sw_video_decoder.h"

/* This function assumes that the host application has initialized vita2d. */
/* No audio driver: video-only example. */
int play_local_h264_video_file(const char *path) {
	VitaSwVideoDecoderStreamFactory stream;
	vita_sw_video_decoder_file_stream_factory(path, &stream);

	VitaSwVideoDecoderPlayer *player = vita_sw_video_decoder_create();
	if (!player) return -1;
	VitaSwVideoDecoderPlayerConfig config = {
		.stream = stream,
		.volume_percent = 100
	};
	int result = vita_sw_video_decoder_open(player, &config);
	while (result == 0) {
		VitaSwVideoDecoderPlayerStatus status;
		vita_sw_video_decoder_get_status(player, &status);
		if (status.error || status.eof) break;

		vita2d_start_drawing();
		vita2d_clear_screen();
		vita_sw_video_decoder_present(player, 0);
		vita2d_end_drawing();
		vita2d_wait_rendering_done();
		vita_sw_video_decoder_render_complete(player);
		vita2d_swap_buffers();
		sceKernelDelayThread(1000);
	}
	vita_sw_video_decoder_destroy(player);
	return result;
}
