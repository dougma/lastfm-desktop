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

    UrlT auth1(const StringT& productName)
    {
        ParamsT params;
        paramsAdd( params, "website", productName);
        paramsAdd( params, "name", productName);
        return makeUrl("/auth/", params);
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

    UrlT getResults(const StringT& qid)
    {
        ParamsT params;
        paramsAdd(params, "method", "get_results");
        paramsAdd(params, "qid", qid);
        paramsAdd(params, "auth", m_token);
        return apiCall(params);
    }

    // boffin does its own thing:
    UrlT boffinTagcloud()
    {
        ParamsT params;
        paramsAdd(params, "auth", m_token);
        return makeUrl("/boffin/tagcloud", params);
    }

    UrlT boffinRql(const StringT& rql)
    {
        ParamsT params;
        paramsAdd(params, "auth", m_token);
        return makeUrl("/boffin/rql/" + rql, params);
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