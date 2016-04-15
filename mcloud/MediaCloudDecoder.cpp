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
}

Decoder::~Decoder()
{
	
}

void Decoder::playAudioFile(File* file)
{
	std::stringstream str = std::stringstream();
	str << "file://" << file->path;
	GMainLoop *loop;
	GstElement *play;
	GstBus *bus;
	
	/* init GStreamer */
	gst_init (0, 0);
	loop = g_main_loop_new (NULL, FALSE);

	/* set up */
	play = gst_element_factory_make ("playbin", "play");
	g_object_set (G_OBJECT (play), "uri", str.str().c_str(), NULL);

	bus = gst_pipeline_get_bus (GST_PIPELINE (play));
	gst_bus_add_watch (bus, bus_call, loop);
	gst_object_unref (bus);

	gst_element_set_state (play, GST_STATE_PLAYING);

	/* now run */
	g_main_loop_run (loop);

	/* also clean up */
	gst_element_set_state (play, GST_STATE_NULL);
	gst_object_unref (GST_OBJECT (play));
}

gboolean Decoder::bus_call(GstBus *bus, GstMessage *msg, gpointer data)
{
	GMainLoop *loop = (GMainLoop *) data;
	
	switch (GST_MESSAGE_TYPE (msg)) {
		
		case GST_MESSAGE_EOS:
		g_print ("End of stream\n");
		g_main_loop_quit (loop);
		break;
		
		case GST_MESSAGE_ERROR: {
		gchar  *debug;
		GError *error;
		
		gst_message_parse_error (msg, &error, &debug);
		g_free (debug);
	
		g_printerr ("Error: %s\n", error->message);
		g_error_free (error);

		g_main_loop_quit (loop);
		break;
		}
		default:
		break;
	}
	
	return TRUE;
}