/***************************************************************************
 *   Copyright 2009 Last.fm Ltd.                                           *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.         *
 ***************************************************************************/

#ifndef TPLAYDAR_API_HPP
#define TPLAYDAR_API_HPP
// Policy needs to provide:
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

    // qid optional
    // rql optional
    UrlT boffinTagcloud(const StringT& cometSession, const StringT& qid = StringT(), const StringT& rql = StringT())
    {
        ParamsT params;
        paramsAdd(params, "auth", m_token);
        paramsAdd(params, "comet", cometSession);
        if (qid != "") {
            paramsAdd(params, "qid", qid);
        }
        return makeUrl("/boffin/tagcloud/" + rql, params);
    }

    // qid optional
    UrlT boffinRql(const StringT& cometSession, const StringT& qid, const StringT& rql)
    {
        ParamsT params;
        paramsAdd(params, "auth", m_token);
        paramsAdd(params, "comet", cometSession);
        if (qid != "") {
            paramsAdd(params, "qid", qid);
        }
        return makeUrl("/boffin/rql/" + rql, params);
    }

    UrlT comet(const StringT& session)
    {
        ParamsT params;
        paramsAdd(params, "session", session);
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
