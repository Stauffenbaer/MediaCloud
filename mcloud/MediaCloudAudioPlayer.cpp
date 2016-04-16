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

#include "MediaCloudAudioPlayer.h"

using namespace MediaCloud;

AudioPlayer::AudioPlayer()
{
	decoder = new Decoder();
	
	finished = false;
	
	args = playerArgs();
	args.decoder = decoder;
	args.finished = &finished;
}

AudioPlayer::~AudioPlayer()
{
	delete decoder;
}

bool AudioPlayer::isFinished()
{
	return finished;
}

void AudioPlayer::setMedia(File *file)
{
	args.file = file;
}

void AudioPlayer::play()
{
	finished = false;
	audioThread = boost::thread(audio_func, args);
}

void AudioPlayer::setPlaying()
{
	decoder->continueAudio();
}

void AudioPlayer::setPaused()
{
	decoder->pauseAudio();
}

void AudioPlayer::setStopped()
{
	decoder->stopAudio();
	audioThread.interrupt();
	finished = true;
}

void AudioPlayer::setVolume(float v)
{
	decoder->setVolume(v);
}

void AudioPlayer::audio_func(playerArgs args)
{
	args.decoder->playAudioFile(args.file);
	*(args.finished) = true;
}