/*

Copyright (c) 2016 Julian Lobe

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

*/

#include "MediaCloudDecoder.h"

using namespace MediaCloud;

Decoder::Decoder()
{
	av_register_all();
	ao_initialize();
	
	container = avformat_alloc_context();
}

Decoder::~Decoder()
{
	ao_shutdown();
}

void Decoder::playAudioFile(File* file, int driver, int* err)
{
	*err = 0;
	
	if (file->type != "audio") {
		*err = -1;
		return;
	}
	
	if (avformat_open_input(&container, file->path.c_str(), 0, 0) < 0) {
		*err = -2;
		return;
	}
	
	int stream = -1;
	
	for (int i = 0;i < container->nb_streams; ++i) {
		if(container->streams[i]->codec->codec_type == AVMEDIA_TYPE_AUDIO) {
            stream = i;
            break;
        }
	}
	
	if (stream == -1) {
		*err = -3;
		return;
	}
	
	AVDictionary *metadata = container->metadata;
	AVCodecContext *ctx = container->streams[stream]->codec;
    AVCodec *codec = avcodec_find_decoder(ctx->codec_id);
	
	if(codec == 0) {
		*err = 4;
		return;
    }
    
    if (avcodec_open2(ctx, codec, 0) < 0) {
		*err = 5;
		return;
	}
	
    ao_sample_format format;
    AVSampleFormat sformat = ctx->sample_fmt;
	
    if(sformat == AV_SAMPLE_FMT_U8)
        format.bits = 8;
    else if(sformat == AV_SAMPLE_FMT_S16)
        format.bits = 16;
    else if(sformat == AV_SAMPLE_FMT_S32)
        format.bits = 32;

    format.channels = ctx->channels;
    format.rate = ctx->sample_rate;
    format.byte_format = AO_FMT_NATIVE;
    format.matrix = 0;

    ao_device *device = ao_open_live(driver, &format, 0);

    AVPacket packet;
    av_init_packet(&packet);
    AVFrame *frame = avcodec_alloc_frame();

    int buffer_size = ctx->frame_size;
    uint8_t buffer[buffer_size];
    packet.data = buffer;
    packet.size = buffer_size;
	
    int finished=0;
    while (av_read_frame(container, &packet) >= 0)
    {
        if (packet.stream_index != stream)
			continue;
		
		int len = avcodec_decode_audio4(ctx, frame, &finished, &packet);
		
		if (finished) {
			ao_play(device, (char*)frame->extended_data[0],frame->linesize[0] );
		}
    }
}