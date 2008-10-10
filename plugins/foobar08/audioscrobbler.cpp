/*
    This file is part of the Audioscrobbler component for Foobar2000.

    This component is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This component is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this source; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

	Copyright © 2006 Last.fm Ltd.
*/
#include <ScrobSubmitter.h>
#include "../SDK/foobar2000.h"
#include "../SDK/play_callback.h"
#include "../SDK/console.h"
#include "../SDK/metadb.h"
#include "../SDK/initquit.h"
#include "../SDK/componentversion.h"
#include "../SDK/file_info_helper.h"

#define coalesce(a) (a!=NULL)?a:""

CScrobSubmitter *scrobbler;

class play_callback_jump : public play_callback {
	virtual void on_playback_starting() {}
	virtual void on_playback_new_track(metadb_handle * track) {
		file_info_i_full info; 
		track->handle_query(&info);	
		try {
			scrobbler->Start(std::string(coalesce(info.meta_get("artist"))), 
						std::string(coalesce(info.meta_get("title"))), 
						std::string(coalesce(info.meta_get("album"))),
						std::string(coalesce(info.meta_get("MUSICBRAINZ_TRACKID"))), 
						info.get_length(), 
						std::string(coalesce(info.get_file_path()))
						);
		} catch (CScrobSubmitter::CAudioScrobblerException& e) {
			console::warning(e.what());
		}
	}
	virtual void on_playback_stop(play_control::stop_reason reason) {
		try {
			scrobbler->Stop();
		} catch (CScrobSubmitter::CAudioScrobblerException& e) {
			console::warning(e.what());
		}
	}
	virtual void on_playback_pause(int state) {
		try {
			if (state) { // pausing
				scrobbler->Pause();
			} else {	// unpausing
				scrobbler->Resume();
			}
		} catch (CScrobSubmitter::CAudioScrobblerException& e) {
			console::warning(e.what());
		}
	}
	virtual void on_playback_seek(double time) {}
	virtual void on_playback_edited(metadb_handle * track) {}
	virtual void on_playback_dynamic_info(const file_info * info,bool b_track_change) {}
};

class initquit_audioscrobbler : public initquit {
public:
	void on_init() 	{
		scrobbler = new CScrobSubmitter();
		try {
			scrobbler->Init("foo");
		} catch (CScrobSubmitter::CAudioScrobblerException& e) {
			console::warning(e.what());
		}
		console::info("Audioscrobbler loaded");
	}
	void on_quit() 	{
		delete scrobbler;
	}
};

static service_factory_single_t<play_callback, play_callback_jump> foo;
static service_factory_single_t<initquit, initquit_audioscrobbler> bar;
DECLARE_COMPONENT_VERSION("Audioscrobbler", "2.0", "http://www.last.fm");