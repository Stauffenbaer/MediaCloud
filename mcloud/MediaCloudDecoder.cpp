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
	
	pipeline = gst_element_factory_make("playbin", NULL);
	GstElement *audiosink = gst_element_factory_make("audiosink", NULL);
	
	g_object_set(G_OBJECT(pipeline), "audio-sink", audiosink, NULL);
	g_object_set(G_OBJECT(pipeline), "uri", s.str().c_str(), NULL);
	
	gst_element_set_state(pipeline, GST_STATE_PLAYING);
	
	bus = gst_element_get_bus(pipeline);
	GstMessageType msgType = (GstMessageType) (GST_MESSAGE_ERROR | GST_MESSAGE_EOS);
	msg = gst_bus_timed_pop_filtered(bus, GST_CLOCK_TIME_NONE, msgType );
	
	if(msg != NULL)
		gst_message_unref(msg);
	gst_object_unref(bus);
	gst_element_set_state(pipeline, GST_STATE_NULL);
}

void Decoder::playVideoFile(std::string path)
{
	gst_init(0, 0);
	QApplication app(0, 0);
	app.setQuitOnLastWindowClosed(true);
	
	std::stringstream s;
	s << "file://" << path;
	
	pipeline = gst_element_factory_make("playbin", NULL);
	GstElement *videosink = gst_element_factory_make("xvimagesink", NULL);
	
	g_object_set(pipeline, "video-sink", videosink, NULL);
	g_object_set(pipeline, "uri", s.str().c_str(), NULL);
	
	QWidget window;
	window.setWindowTitle("MediaCloud Server");
	window.showFullScreen();
	
	WId xwinid = window.winId();
	QApplication::syncX();
	gst_video_overlay_set_window_handle(GST_VIDEO_OVERLAY(videosink), xwinid);
	
	GstStateChangeReturn sret = gst_element_set_state(pipeline, GST_STATE_PLAYING);
	if(sret == GST_STATE_CHANGE_FAILURE) {
		gst_element_set_state(pipeline, GST_STATE_NULL);
		gst_object_unref(pipeline);
		
		QTimer::singleShot(0, QApplication::activeWindow(), SLOT(quit()));
	}
	
	app.exec();
	
	window.hide();
	gst_element_set_state(pipeline, GST_STATE_NULL);
	gst_object_unref(pipeline);
}

void Decoder::setVolume(float v)
{
	g_object_set(G_OBJECT(pipeline), "volume", v, NULL);
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