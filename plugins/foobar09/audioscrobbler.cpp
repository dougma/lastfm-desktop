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

    Copyright © 2007 Last.fm Ltd.
*/
#include <ScrobSubmitter.h>

#include "SDK/foobar2000.h"
#include "SDK/play_callback.h"
#include "SDK/console.h"
#include "SDK/metadb.h"
#include "SDK/initquit.h"
#include "SDK/componentversion.h"

#define coalesce(a) (a!=NULL)?a:""

ScrobSubmitter scrobbler;

class play_callback_jump : public play_callback_static
{

    virtual unsigned int get_flags()
    {
        return flag_on_playback_new_track | flag_on_playback_stop | flag_on_playback_pause;
    }

    virtual void on_playback_starting(play_control::t_track_command p_command, bool p_paused) {}
    
    virtual void on_playback_new_track(metadb_handle_ptr track)
    {
        file_info_impl info; 
        track->get_info(info);
        std::string path = coalesce(track->get_path());

        if (path.length() > 7 &&
            path[0] == 'f' &&
            path[1] == 'i' &&
            path[2] == 'l' &&
            path[3] == 'e' &&
            path[4] == ':' &&
            path[5] == '/' &&
            path[6] == '/')
        {
            path = path.substr(7, path.length() - 1);
        }
        
        scrobbler.Start(std::string(coalesce(info.meta_get("artist", 0))), 
                        std::string(coalesce(info.meta_get("title", 0))), 
                        std::string(coalesce(info.meta_get("album", 0))),
                        std::string(coalesce(info.meta_get("MUSICBRAINZ_TRACKID", 0))), 
                        (int)info.get_length(), 
                        path);

    }

    virtual void on_playback_stop(play_control::t_stop_reason reason)
    {
        if (reason != play_control::stop_reason_starting_another)
        {
            scrobbler.Stop();
        }
    }

    virtual void on_playback_pause(bool state)
    {
        if (state) { // pausing
            scrobbler.Pause();
        } else {	// unpausing
            scrobbler.Resume();
        }
    }

    virtual void on_playback_seek(double time) {}
    virtual void on_playback_edited(metadb_handle_ptr p_track) {}
    virtual void on_playback_dynamic_info(const file_info & info) {}
    virtual void on_playback_dynamic_info_track(const file_info & info) {}
    virtual void on_playback_time(double p_time) {}
    virtual void on_volume_change(float p_new_val) {};
};

void statusCallback (int reqId, bool error, std::string msg, void* userData)
{
    if (error)
    {
	    console::info(pfc::string8("Audioscrobbler: ") << msg.c_str());
    }
}

class initquit_audioscrobbler : public initquit {
public:

    void on_init()
    {
        scrobbler.Init("foo", &statusCallback, 0);
        console::info("Audioscrobbler: Loaded");
    }

    void on_quit()
    {
        scrobbler.Stop();
        scrobbler.Term();
    }
};

static play_callback_static_factory_t<play_callback_jump> foo;
static initquit_factory_t<initquit_audioscrobbler> bar;
DECLARE_COMPONENT_VERSION("Audioscrobbler", "2.3.1", "http://www.last.fm");