#ifndef QASURA_GLOBAL_H
#define QASURA_GLOBAL_H

#include <QtCore/qglobal.h>


#if defined(QASURA_LIBRARY)
  #define QASURASHARED_EXPORT Q_DECL_EXPORT
#else
  #define QASURASHARED_EXPORT Q_DECL_IMPORT
#endif


#endif // QASURA_GLOBAL_H
