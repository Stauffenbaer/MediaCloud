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

#pragma once


#include <vector>
#include <map>

#include "MediaCloudDatabase.h"
#include "MediaCloudFilesystem.h"
#include "MediaCloudAudioPlayer.h"

namespace MediaCloud {

	class Track
	{
	public:
		Track();
		~Track();
		
		int ID;
		int file;
		std::string path;
		std::string cover;
		
		std::string title;
		std::string artist;
		std::string album;
		int tracknr;
		int duration;
	};
	
	class Playlist
	{
	public:
		Playlist();
		~Playlist();
		
		int ID;
		std::string name;
		std::string user;
		
		int current;
		int count;
		
		std::map<int, Track> tracks;
	};
	
	class Album
	{
	public:
		Album();
		~Album();
		
		int ID;
		std::string name;
		
		std::vector<Track> tracks;
	};
	
	class AudioProvider
	{
	public:
		AudioProvider(Database*, Filesystem*);
		~AudioProvider();
		
		std::vector<Album> GetAlben();
		std::vector<Playlist> GetAllPlaylists();
		std::vector<Playlist> GetUserPlaylists(std::string);
		
		Playlist createPlaylist(std::string, int);
		int getCurrentPlaylistIndex(Playlist);
		void addTrackToPlaylist(Playlist, Track);
		
		Track getCurrentTrack(Playlist);
		bool nextTrack(Playlist);
		bool lastTrack(Playlist);
		
		std::string getTrackCover(Track);
		std::string getAlbumCover(Album);
		std::string getPlaylistCover(Playlist);
		
		void playTrack(Track);
		bool isPlaying();
		
	protected:
		Database *db;
		Filesystem *fs;
		
		AudioPlayer *player;
		
	private:
		Track getTrack(int);
		std::string getUsername(int);
		int getUserID(std::string);
	};

}