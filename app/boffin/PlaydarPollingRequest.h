#ifndef PLAYDAR_POLLING_REQUEST_H
#define PLAYDAR_POLLING_REQUEST_H

#include <string>
#include "json_spirit/json_spirit.h"


// abstract base class for polling kind of requests
//
class PlaydarPollingRequest
{
public:
    PlaydarPollingRequest();
    ~PlaydarPollingRequest();

    void start();

    const std::string& qid();

private:
    virtual void issueRequest() = 0;
    virtual void issuePoll(unsigned msDelay) = 0;

    // return true if another poll should be made
    virtual bool handleJsonPollResponse(
        int poll, 
        const json_spirit::Object& query, 
        const json_spirit::Array& results) = 0;

    virtual void fail(const char* message) = 0;

protected:
    // derived class calls this with the response of the initial request
    void handleResponse(const char *data, unsigned size);

    // derived class calls this with the response from a poll
    void handlePollResponse(const char *data, unsigned size);

private:
    std::string m_qid;
    int m_pollCount;
};

#endif
