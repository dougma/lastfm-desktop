/*
   Copyright 2009 Last.fm Ltd. 
      - Primarily authored by Max Howell, Jono Cole and Doug Mansell

   This file is part of the Last.fm Desktop Application Suite.

   lastfm-desktop is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   lastfm-desktop is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with lastfm-desktop.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef TPLAYDAR_API_HPP
#define TPLAYDAR_API_HPP

// Policy provides:
//
// void paramsAdd(ParamsT& p, StringT name, StringT value)
// UrlT createUrl(StringT base, StringT path, ParamsT p)

template <typename StringT, typename UrlT, typename ParamsT, typename Policy>
class TPlaydarApi : public Policy
{
public:
    TPlaydarApi(const StringT& baseUrl, const StringT& token)
        : m_baseUrl(baseUrl)
        , m_token(token)
    {
    }

    void setAuthToken(const StringT& token)
    {
        m_token = token;
    }

    UrlT apiCall(const ParamsT& params)
    {
        return makeUrl("/api/", params);
    }

    UrlT stat()
    {
        ParamsT params;
        paramsAdd(params, "method", "stat");
        paramsAdd(params, "auth", m_token);
        return apiCall(params);
    }

    UrlT auth1(const StringT& applicationName)
    {
        ParamsT params;
        paramsAdd(params, "name", applicationName);
        paramsAdd(params, "website", "");
        paramsAdd(params, "json", "");
        return makeUrl("/auth_1/", params);
    }

    UrlT auth2(const StringT& applicationName, const StringT& formtoken, ParamsT& outPostParams)
    {
        paramsAdd(outPostParams, "name", applicationName);
        paramsAdd(outPostParams, "website", "");
        paramsAdd(outPostParams, "formtoken", formtoken);
        paramsAdd(outPostParams, "json", "");
        return makeUrl("/auth_2/");
    }

    UrlT getResults(const StringT& qid)
    {
        ParamsT params;
        paramsAdd(params, "method", "get_results");
        paramsAdd(params, "qid", qid);
        paramsAdd(params, "auth", m_token);
        return apiCall(params);
    }

    UrlT lanRoster()
    {
        return makeUrl("/lan/roster");
    }

    // cometsession optional
    // qid optional
    UrlT trackResolve(const StringT& artist, const StringT& album, const StringT& track, 
        const StringT& cometSession = StringT(), const StringT& qid = StringT())
    {
        ParamsT params;
        paramsAdd(params, "method", "resolve");
        paramsAdd(params, "artist", artist);
        paramsAdd(params, "album", album);
        paramsAdd(params, "track", track);
        paramsAdd(params, "auth", m_token);
        if (cometSession != "") {
            paramsAdd(params, "comet", cometSession);
        }
        if (qid != "") {
            paramsAdd(params, "qid", qid);
        }
        return apiCall(params);
    }

    // boffinTags to obtain tags and weights for a query
    // qid optional
    // rql optional
    UrlT boffinTags(const StringT& cometSession, const StringT& qid = StringT(), const StringT& rql = StringT())
    {
        ParamsT params;
        paramsAdd(params, "auth", m_token);
        paramsAdd(params, "comet", cometSession);
        if (qid != "") {
            paramsAdd(params, "qid", qid);
        }
        return makeUrl("/boffin/tags/" + rql, params);
    }

    // boffinTracks to obtain playable items matching a query
    // qid optional
    UrlT boffinTracks(const StringT& cometSession, const StringT& qid, const StringT& rql)
    {
        ParamsT params;
        paramsAdd(params, "auth", m_token);
        paramsAdd(params, "comet", cometSession);
        if (qid != "") {
            paramsAdd(params, "qid", qid);
        }
        return makeUrl("/boffin/tracks/" + rql, params);
    }

    // boffinSummary provides file count and total play time for a query
    // qid optional
    UrlT boffinSummary(const StringT& cometSession, const StringT& qid, const StringT& rql)
    {
        ParamsT params;
        paramsAdd(params, "auth", m_token);
        paramsAdd(params, "comet", cometSession);
        if (qid != "") {
            paramsAdd(params, "qid", qid);
        }
        return makeUrl("/boffin/summary/" + rql, params);
    }

    UrlT comet(const StringT& session)
    {
        ParamsT params;
        paramsAdd(params, "session", session);
        paramsAdd(params, "auth", m_token);
        return makeUrl("/comet/", params);
    }

private:
    UrlT makeUrl(const StringT& path, const ParamsT& params = ParamsT())
    {
        return createUrl(m_baseUrl, path, params);
    }

    StringT m_baseUrl;
    StringT m_token;
};

#endif
