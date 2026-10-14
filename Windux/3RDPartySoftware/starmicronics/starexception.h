#ifndef STAREXCEPTION_H
#define STAREXCEPTION_H

namespace StarMicronics
{

class StarException
{
public:
    StarException(const char * message);
    StarException(const StarException& copy);
    ~StarException();
    const char * getMessage() const;

private:
    char *m_message;

};

}

#endif // STAREXCEPTION_H
