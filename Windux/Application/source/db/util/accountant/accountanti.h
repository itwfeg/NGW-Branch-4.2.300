#ifndef ACCOUNTANTI_H
#define ACCOUNTANTI_H



#include <string>



#ifndef NO_NAMESPACES
namespace util_acc
{
#endif



int registerDataType(const std::string& name);
void addInstance(int dataType);
void removeInstance(int dataType);


void showInstanceCounters(bool all);
void saveInstanceCounters();
void showInstanceCounterDeltas(bool all);



#ifndef NO_NAMESPACES
} // namespace util_acc
#endif



#endif // ACCOUNTANTI_H
