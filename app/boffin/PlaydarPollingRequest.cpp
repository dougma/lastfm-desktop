#include "PlaydarPollingRequest.h"
#include "jsonGetMember.hpp"

using namespace std;
using namespace json_spirit;


PlaydarPollingRequest::PlaydarPollingRequest()
    : m_pollCount(0)
{
}

PlaydarPollingRequest::~PlaydarPollingRequest()
{
}

void
PlaydarPollingRequest::start()
{
    issueRequest();
}

const string& 
PlaydarPollingRequest::qid()
{
    return m_qid;
}

void 
PlaydarPollingRequest::handleResponse(const char *data, unsigned size)
{
    Value v;
    if (read(string(data, size), v) && 
        jsonGetMember(v, "qid", m_qid)) 
    {
        issuePoll(0);
        return;
    }
    fail("bad json in poll response");
}

void 
PlaydarPollingRequest::handlePollResponse(const char *data, unsigned size)
{
    m_pollCount++;
    
    Value v;
    string qid;
    int refresh_interval;
    Object query;
    Array results;

    if (read(string(data, size), v) &&
        jsonGetMember(v, "qid", qid) &&
        jsonGetMember(v, "refresh_interval", refresh_interval) &&
        jsonGetMember(v, "query", query) &&
        jsonGetMember(v, "results", results) )
    {
        if (handleJsonPollResponse(m_pollCount, query, results)) {
            issuePoll(refresh_interval);
            return;
        }
    }
    fail("bad json in poll response");
}

