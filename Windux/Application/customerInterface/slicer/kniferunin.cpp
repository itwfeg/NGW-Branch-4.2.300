#include "kniferunin.h"
#include <fstream>
#include <unistd.h>
#include <iostream>

KnifeRunIn::KnifeRunIn() :
    m_simulatedState(KnifeRunIn::DeAsserted)
{
}

KnifeRunIn::state_e KnifeRunIn::State () const
{
    return m_simulatedState;
}

void KnifeRunIn::slt_SimulateAssert()
{
    m_simulatedState = Asserted;
    emit sig_Asserted();
}

void KnifeRunIn::slt_SimulateDeAssert()
{
    m_simulatedState = DeAsserted;
    emit sig_DeAsserted();
}

void KnifeRunIn::slt_fileChanged(const QString &path){
    std::cout << "FILECHANGED________________________________" << std::endl;
    std::ifstream knifeRunning;
    std::string line;
    //read in knife running file
    knifeRunning.open(path.toStdString(), std::ios::in);
    if(knifeRunning.is_open()){
        std::getline(knifeRunning, line);
        knifeRunning.close();
        std::cout <<  ":" << strcmp(line.data(), "1") << std::endl;
        //if knife is running
        if(strcmp(line.data(), "1") == 0){
            emit sig_Asserted();
        }else{
            emit sig_DeAsserted();
        }
    }
}
