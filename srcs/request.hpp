#ifndef REQUEST_HPP
#define REQUEST_HPP

#include <iostream>

namespace rnitta
{



// sample
// RUN 23 user:rnitta time:123456 18 MainFunc arg1 arg2
// STOP
class Request
{
    public:
        enum e_status
        {
            METHOD,
            HEADER_LENGTH,
            HEADER,
            BODY_LENGTH,
            BODY,
            FINISH,
            ERR
        };
        Request();
        Request(const Request& other);
        ~Request();
        Request& operator=(const Request& other);

        int getStatus() const;
        const std::string& getMethod() const;
        const std::string& getHeader() const;
        const std::string& getBody() const;
        void read(const std::string _content);
        void clear();

    private:
        int status_;
        std::string save_;
        std::string method_;
        size_t header_length_;
        std::string header_;
        size_t body_length_;
        std::string body_;
};


}

#endif
