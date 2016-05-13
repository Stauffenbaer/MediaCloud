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
	gst_init(0, 0);
	
 	pipeline = gst_parse_launch("playbin", NULL);
	equalizer = gst_element_factory_make("equalizer-3bands", "equalizer");
	convert = gst_element_factory_make("audioconvert", "convert");
	sink = gst_element_factory_make("autoaudiosink", "audio_sink");
}

Decoder::~Decoder()
{
	gst_object_unref(pipeline);
}

void Decoder::playAudioFile(std::string path)
{	
	gst_init(0, 0);
	
	std::stringstream s;
	s << "file://" << path;
	g_object_set(G_OBJECT(pipeline), "uri", s.str().c_str(), NULL);
	
	bin = gst_bin_new("audio_sink_bin");
	gst_bin_add_many(GST_BIN (bin), equalizer, convert, sink, NULL);
	gst_element_link_many(equalizer, convert, sink, NULL);
	pad = gst_element_get_static_pad(equalizer, "sink");
	ghost_pad = gst_ghost_pad_new("sink", pad);
	gst_pad_set_active(ghost_pad, TRUE);
	gst_element_add_pad(bin, ghost_pad);
	gst_object_unref(pad);
	
	g_object_set(GST_OBJECT (pipeline), "audio-sink", bin, NULL);
	
	gst_element_set_state(pipeline, GST_STATE_PLAYING);
	
	bus = gst_element_get_bus(pipeline);
	GstMessageType msgType = (GstMessageType) (GST_MESSAGE_ERROR | GST_MESSAGE_EOS);
	msg = gst_bus_timed_pop_filtered(bus, GST_CLOCK_TIME_NONE, msgType );
	
	if (msg != NULL)
		gst_message_unref(msg);
	gst_object_unref(bus);
	gst_element_set_state(pipeline, GST_STATE_NULL);
}

void Decoder::setVolume(float v)
{
	g_object_set(G_OBJECT(pipeline), "volume", v, NULL);
}

void Decoder::setLowGain(float g)
{
	g_object_set(G_OBJECT(equalizer), "band0", g, NULL);
}

void Decoder::setMiddleGain(float g)
{
	g_object_set(G_OBJECT(equalizer), "band1", g, NULL);
}

void Decoder::setHighGain(float g)
{
	g_object_set(G_OBJECT(equalizer), "band2", g, NULL);
}

void Decoder::pauseAudio()
{
	gst_element_set_state(pipeline, GST_STATE_PAUSED);
}

void Decoder::continueAudio()
{
	gst_element_set_state(pipeline, GST_STATE_PLAYING);
}

void Decoder::stopAudio()
{
	gst_element_set_state(pipeline, GST_STATE_NULL);
}
