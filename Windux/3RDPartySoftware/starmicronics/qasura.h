#ifndef QASURA_H
#define QASURA_H

#include "printerstatus.h"
#include "qabarcodegenerator.h"
#include "qabarcodereader.h"
#include "qaexception.h"
#include "qaprinter.h"
#include "QAsura_global.h"


/**
    @defgroup QAsura QAsura
        This namespace is used to control the QAPrinter, QABarcodeReader, and QABarcodeGenerator classes, which provide
        a Qt4 specific API for controling the unique features of the AsuraCPRNT.
*/


namespace StarMicronics_QAsura
{


class QASURASHARED_EXPORT QAsura {
public:

    QAsura();
    ~QAsura();
};


} // namespace StarMicronics_QAsura


#endif // QASURA_H
