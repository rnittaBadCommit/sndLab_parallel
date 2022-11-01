#include "request.hpp"

namespace rnitta
{


Request::Request()
: status_(METHOD), body_length_(SIZE_MAX)
{

}

Request::Request(const Request& other)
{
    *this = other;
}

Request::~Request()
{

}

Request& Request::operator=(const Request& other)
{
    if (this == &other)
        return (*this);
    
    status_ = other.status_;
    save_ = other.save_;
    method_ = other.method_;
    header_length_ = other.header_length_;
    header_ = other.header_;
    body_length_ = other.body_length_;
    body_ = other.body_;
    return (*this);
}

int Request::getStatus() const
{ return (status_); }

const std::string& Request::getMethod() const
{ return (method_); }

const std::string& Request::getHeader() const
{ return (header_); }

const std::string& Request::getBody() const
{ return (body_); }

void Request::read(const std::string _content)
{
    save_ += _content;
    if (status_ == METHOD)
    {
        size_t pos = save_.find(' ');
        if (pos == std::string::npos)
            return ;
        
        method_ = save_.substr(0, pos);
        save_.erase(0, pos + 1);
        status_ = HEADER_LENGTH;
    }
    if (status_ == HEADER_LENGTH)
    {
        size_t pos = save_.find(' ');
        if (pos == std::string::npos)
            return ;
        
        header_length_ = std::stoi(save_);
        save_.erase(0, pos + 1);
        status_ = HEADER;
    }
    if (status_ == HEADER)
    {
        if (save_.size() < header_length_)
            return ;

        header_ = save_.substr(0, header_length_);
        save_.erase(0, header_length_ + 1);
        status_ = BODY_LENGTH;
    }
    if (status_ == BODY_LENGTH)
    {
        size_t pos = save_.find(' ');
        if (pos == std::string::npos)
            return ;
        
        body_length_ = std::stoi(save_);
        save_.erase(0, pos + 1);
        status_ = BODY;
    }
    if (status_ == BODY)
    {
        if (save_.size() < body_length_)
            return ;

        body_ = save_;
        status_ = FINISH;
    }
}


}
