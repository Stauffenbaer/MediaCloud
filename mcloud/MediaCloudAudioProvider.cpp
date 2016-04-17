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

#include "MediaCloudAudioProvider.h"

using namespace MediaCloud;

Track::Track()
{
	ID = 0;
	file = 0;
	title = "";
	artist = "";
	album = "";
	tracknr = 0;
	duration = 0;
}

Track::~Track()
{
	
}


Playlist::Playlist()
{
	
}

Playlist::~Playlist()
{
	
}


Album::Album()
{
	ID = 0;
	name = "";
	tracks = std::vector<Track>();
}

Album::~Album()
{
	tracks.clear();
}


AudioProvider::AudioProvider(Database *database, Filesystem *filesystem)
{
	db = database;
	fs = filesystem;
	player = new AudioPlayer(database);
}

AudioProvider::~AudioProvider()
{
	delete player;
}

std::vector<Album> AudioProvider::GetAlben()
{
	std::vector<Album> albums = std::vector<Album>();
	
	Result *res = db->query("SELECT DISTINCT album FROM tbl_music ORDER BY album");
	
	std::stringstream query;
	for(int i = 0;i < res->rows; ++i) {
		query = std::stringstream();
		ResultRow r = res->data[i];
		std::string albumName = r.columns[0];
		
		query << "SELECT * FROM tbl_music WHERE album='" << albumName << "' ORDER BY tracknr";
		Result *trackRes = db->query(query.str());
		
		Album album = Album();
		album.ID = i + 1;
		album.name = albumName;
		
		for(int i = 0;i < trackRes->rows; ++i) {
			ResultRow trackRow = trackRes->data[i];
			Track t = Track();
			t.ID = atoi(trackRow.columns[0].c_str());
			t.file = atoi(trackRow.columns[1].c_str());
			t.title = trackRow.columns[2];
			t.artist = trackRow.columns[3];
			t.album = trackRow.columns[4];
			t.tracknr = atoi(trackRow.columns[5].c_str());
			t.duration = atoi(trackRow.columns[6].c_str());
			
			query = std::stringstream();
			query << "SELECT path FROM tbl_files WHERE ID='" << t.file << "'";
			Result *pathRes = db->query(query.str());
			t.path = pathRes->data[0].columns[0];
			
			delete pathRes;
			album.tracks.push_back(t);
		}
		
		albums.push_back(album);
		
		delete trackRes;
	}
	
	delete res;
	
	return albums;
}