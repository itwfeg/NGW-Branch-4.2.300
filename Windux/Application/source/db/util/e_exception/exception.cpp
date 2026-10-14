#include "exception.h"



#ifndef NO_NAMESPACES
namespace util_exception
{
#endif


    using namespace std;


    Exception::Exception(const std::string& what, const std::exception* e)
        : runtime_error(what), ex(e)
    {
    }


    Exception::~Exception() throw()
    {
        if ( ex )
            delete ex;
    }


    const string& Exception::info() const
    {
        message.assign(what());

        const exception* nextx = ex;
        while ( nextx )
        {
            message.append("\n\n");
            message.append(nextx->what());

            const Exception* e = dynamic_cast<const Exception*>(nextx);
            if ( !e )
                break;

            nextx = e->next();
        }

        return message;
    }


#ifndef NO_NAMESPACES
} // namespace util_exception
#endif

