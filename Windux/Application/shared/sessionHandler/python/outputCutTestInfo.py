#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import sys
import os
#import shellCommand
#import file_io
import languageSettings

def updateCutTestInfo(cutTestFileOutput, cutTestDataOutput, cutTestSubsOutput):

    language = languageSettings.currentLanguage()

    #create headers
    if language == 'spanish':
        headers = "Núm de prueba corte,Número de producto,Número de proveedor,Costo del Producto,Peso del Producto,Número de lote,Estatus,Nombre del operador,Fecha de empaque,Fecha/Hora de inicio,Fecha/Hora de final,Desperdicio,Peso Total,Monto de venta,Desperdicio %,Participación Total"
        subheaders = "PLU,Descripción,Paquetes,Peso,Precio Unitario,Monto de venta,Participación"
    else:
        headers = 'Test Number,Product Number,Vendor Number,Product Cost,Product Weight,Lot Number,Status,Operator Name,Packing Date,Start Date,End Date,Total Wasted,Total Weight,Total Sales Price,Total Wasted Percentage,Total Participation'
        subheaders = 'Subprimal Product Number,Subprimal Description,Subprimal Packages,Subprimal Weights,Subprimal Unit Price,Subprimal Total Price,Subprimal Participation'
        
    with open(cutTestFileOutput, 'w+') as log_file:
        log_file.write(headers)
        log_file.write('\n')
        log_file.write(cutTestDataOutput)
        log_file.write('\n')
        log_file.write(subheaders)
        log_file.write('\n')
        log_file.write( str(cutTestSubsOutput) )
        log_file.close()
    
    os.system("/bin/sync")

    status = 0
    return status

if __name__ == "__main__":

    status = 0

    cutTestFile = csvfile=sys.argv[1]
    cutTestData = csvfile=sys.argv[2]
    cutTestSubs = csvfile=sys.argv[3]

    status = updateCutTestInfo( cutTestFile, cutTestData,  cutTestSubs )

    if status == 'error':
        sys.exit(1)
    else:
        sys.exit(0)
