#pragma once

extern "C" {
	#include <libavcodec/avcodec.h>
	#include <libavformat/avformat.h>
	#include <libavutil/avutil.h>
}

#include <iostream>

namespace MediaCloud {

	class Decoder
	{
	public:
		Decoder();
		~Decoder();
	};

}