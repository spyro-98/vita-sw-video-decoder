# vita-sw-video-decoder

PlayStation Vita video session module: CPU H.264, NV12 or optional planar presentation. API **3.0.0**, split for **VitaMediaDeck Beta 1.4.4** from `vita-sw-decoder`.

The module owns video decoding, presentation, independent demux cursors and A/V synchronization. Audio decoding and AudioOut are supplied by the caller through `VitaAudioPlaybackDriver`; this archive contains no audio decoder, resampler or PCM/output worker.

## Integration

Keep the `vita-hw-audio-decoder` repository beside this one for the shared header contract, or set `VITA_AUDIO_DECODER_INTERFACE_ROOT`. The video library only links the interface target, not its hardware audio implementation.

```cmake
set(VITA_SW_VIDEO_DECODER_FFMPEG_ROOT "/path/to/pinned-ffmpeg")
add_subdirectory(vita-sw-video-decoder)
target_link_libraries(my_app PRIVATE VitaSwVideoDecoder::VitaSwVideoDecoder)
```

```c
#include <vita_sw_video_decoder.h>
VitaSwVideoDecoderPlayerConfig config = {0};
config.stream = source;
config.volume_percent = 100;
config.audio_driver = app_audio_driver; /* NULL: video-only session */
```

The app audio driver owns codec objects, workers, PCM storage and AudioOut. It must publish readiness after decoded PCM, update the consumed audio clock, honor pause/stop/cancel/start-gate, and join all workers before returning from `join`. Its lifetime must cover the whole video session. VitaMediaDeck's single implementation is `src/media/audio_playback.c`, shared by both video modules.

## Migration and behavior

- Rebuild consumers: public headers, symbols, CMake targets and config names now include `video`. This is an ABI change.
- Select audio and seek through the video session API as before. Transactions retain independent cursors, readiness gates and failed-track rollback.
- Subtitle discovery remains available; text decoding and selected subtitle/language state belong to the embedding app.
- The HW module stays hardware-only for video; SW audio does not change that choice. The SW video module does not initialize HW video.
- `examples/local_file.c` demonstrates video-only playback. Supply a driver to enable sound.

Build with VitaSDK, CMake and the compatible FFmpeg prefix. `VITA_SW_VIDEO_DECODER_BUILD_EXAMPLE=ON` also compiles the example. Install the shared `VitaAudioDecoder` interface together with the video package. Older files under `docs/` describe the pre-split playback internals; the current ownership/API contract is this README and the public headers.

Host decoder, shared-audio and session regression tests live in VitaMediaDeck (`tests/decoder`, `tests/multiaudio`, `tests/playback_seek`). They do not replace audio output, A/V sync, seek and subtitle checks on a physical Vita.

GPL-3.0-only. See [LICENSE](LICENSE) and [third-party notices](THIRD_PARTY_NOTICES.md). Existing GitHub remote addresses are retained until their owner renames them.
