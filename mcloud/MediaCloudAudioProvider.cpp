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
	current = 1;
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
			t = getTrack(atoi(trackRow.columns[0].c_str()));
			
			album.tracks.push_back(t);
		}
		
		albums.push_back(album);
		
		delete trackRes;
	}
	
	delete res;
	
	return albums;
}

std::vector<Playlist> AudioProvider::GetAllPlaylists()
{
	std::vector<Playlist> plists = std::vector<Playlist>();
	
	Result *res = db->query("SELECT * FROM tbl_playlists");
	
	for(int i = 0;i < res->rows; ++i) {
		Playlist plist = Playlist();
		plist.ID = i + 1;
		plist.name = res->data[i].columns[1];
		plist.user = getUsername(atoi(res->data[i].columns[2].c_str()));
		
		std::stringstream query = std::stringstream();
		query << "SELECT * FROM tbl_playlisttracks WHERE plistID='" << plist.ID << "'";
		Result *plistRes = db->query(query.str());
		
		plist.count = plistRes->rows;
		
		for(int j = 0;j < plistRes->rows; ++j) {
			ResultRow dat = plistRes->data[j];
			int tracknr = atoi(dat.columns[2].c_str());
			int trackid = atoi(dat.columns[3].c_str());
			
			Track t = this->getTrack(trackid);
			plist.tracks.insert(std::pair<int, Track>(tracknr, t));
		}
		
		plists.push_back(plist);
		
		delete plistRes;
	}
	
	delete res;
	
	return plists;
}

std::vector<Playlist> AudioProvider::GetUserPlaylists(std::string user)
{
	std::vector<Playlist> plists = std::vector<Playlist>();
	std::vector<Playlist> unfiltered = this->GetAllPlaylists();
	
	for(int i = 0;i < unfiltered.size(); ++i) {
		if (unfiltered[i].user == user)
			plists.push_back(unfiltered[i]);
	}
	
	return plists;
}

Track AudioProvider::getTrackByID(int id)
{
	return getTrack(id);
}

Track AudioProvider::getTrack(int ID)
{
	std::stringstream query = std::stringstream();
	query << "SELECT * FROM tbl_music WHERE ID='" << ID << "' LIMIT 1";
	Result *trackRes = db->query(query.str());
	ResultRow trackRow = trackRes->data[0];
	Track t = Track();
	t.ID = ID;
	t.file = atoi(trackRow.columns[1].c_str());
	t.title = trackRow.columns[2];
	t.artist = trackRow.columns[3];
	t.album = trackRow.columns[4];
	t.tracknr = atoi(trackRow.columns[5].c_str());
	t.duration = atoi(trackRow.columns[6].c_str());
	query = std::stringstream();
	query << "SELECT path FROM tbl_files WHERE ID='" << t.file << "'";
	t.path = db->query(query.str())->data[0].columns[0];
	
	query = std::stringstream();
	query << "SELECT cover FROM tbl_covers WHERE trackid='" << t.ID << "'";
	t.cover = db->query(query.str())->data[0].columns[0];
	
	return t;
}

std::string AudioProvider::getUsername(int ID)
{
	std::stringstream query = std::stringstream();
	query << "SELECT username FROM tbl_users WHERE ID='" << ID << "'";
	Result *res = db->query(query.str());
	std::string user = res->data[0].columns[0];
	delete res;
	return user;
}

Playlist AudioProvider::createPlaylist(std::string name, int user)
{
	std::stringstream query = std::stringstream();
	query << "SELECT name FROM tbl_playlists WHERE name='" << name << "' AND user='" << user << "'";
	if (db->query(query.str())->rows != 0)
		return Playlist();
		
	query = std::stringstream();
	query << "INSERT INTO tbl_playlists (name, user) VALUES ('" << name << "', '" << user << "')";
	db->query(query.str());
	
	Playlist plist = Playlist();
	
	query = std::stringstream();
	query << "SELECT ID FROM tbl_playlists WHERE name='" << name << "' AND user='" << user << "'";
	Result *res = db->query(query.str());
	
	plist.ID = atoi(res->data[0].columns[0].c_str());
	plist.name = name;
	plist.user = getUsername(user);
	
	return plist;
}

int AudioProvider::getCurrentPlaylistIndex(Playlist plist)
{
	std::string username = std::string(plist.user);
	std::string plistname = std::string(plist.name);
	int usr = getUserID(username);
	std::stringstream query = std::stringstream();
	query << "SELECT * FROM tbl_playlists WHERE name='" << plistname << "' AND user='" << usr << "'";
	Result *res = db->query(query.str());	
	int plistid = atoi(res->data[0].columns[0].c_str());
	
	query = std::stringstream();
	query << "SELECT plistindex FROM tbl_playlisttracks WHERE plistid='" << plistid << "' ORDER BY plistindex DESC";
	res = db->query(query.str());
	
	int max = 0;
	if (res->rows != 0)
		max = atoi(res->data[0].columns[0].c_str());
	
	delete res;
	
	return max;
}

void AudioProvider::addTrackToPlaylist(Playlist plist, Track track)
{
	int ID = plist.ID;
	int plistindex = this->getCurrentPlaylistIndex(plist) + 1;
	int trackid = track.ID;
	
	
	std::stringstream query = std::stringstream();
	query << "INSERT INTO tbl_playlisttracks (plistid, plistindex, trackid) VALUES ('";
	query << ID << "', '" << plistindex << "', '" << trackid << "')";
	
	db->query(query.str());
	plist.count++;
}

int AudioProvider::getUserID(std::string username)
{
	std::stringstream query = std::stringstream();
	query << "SELECT ID FROM tbl_users WHERE username='" << username << "'";
	Result *res = db->query(query.str());
	int ID = atoi(res->data[0].columns[0].c_str());
	delete res;
	return ID;
}

Track AudioProvider::getCurrentTrack(Playlist plist)
{
	int index = plist.current;
	for(auto it = plist.tracks.begin(); it != plist.tracks.end(); ++it) {
		if (it->first == index)
			return it->second;
	}
	
	return Track();
}

bool AudioProvider::nextTrack(Playlist plist)
{
	plist.current++;
	if (plist.current > plist.count)
		return false;
	
	return true;
}

bool AudioProvider::lastTrack(Playlist plist)
{
	plist.current--;
	if (plist.current <= 0)
		return false;
	
	return true;
}

std::string AudioProvider::getTrackCover(Track track)
{
	return track.cover;
}

std::string AudioProvider::getAlbumCover(Album album)
{
	return album.tracks[0].cover;
}

std::string AudioProvider::getPlaylistCover(Playlist plist)
{
	return plist.tracks[0].cover;
}

void AudioProvider::playTrack(Track t)
{
	this->player->setMedia(t.path);
	this->player->play();
}

bool AudioProvider::isPlaying()
{
	return !this->player->isFinished();
}

void AudioProvider::playSynchronousTrack(Track track)
{
	this->playTrack(track);
	while(this->isPlaying()) ;
}

void AudioProvider::setVolume(float v)
{
	this->player->setVolume(v);
}

void AudioProvider::setLow(float g)
{
	player->setLow(g);
}

void AudioProvider::setMiddle(float g)
{
	player->setMiddle(g);
}

void AudioProvider::setHigh(float g)
{
	player->setHigh(g);
}

void AudioProvider::pauseState()
{
	player->setPaused();
}

void AudioProvider::playState()
{
	player->setPlaying();
}