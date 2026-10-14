cmake_minimum_required(VERSION 3.13.0)

project(DiagnosticsManager LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(DiagnosticsManager STATIC)

target_compile_definitions(DiagnosticsManager PRIVATE LINUX_FIX)

target_sources(DiagnosticsManager PRIVATE
  DiagnosticsDB.cpp
  DiagnosticsDB.h
  DiagnosticsEdgeDevice.cpp
  DiagnosticsEdgeDevice.h
  DiagnosticsManager.cpp
  DiagnosticsManager.h
  DBO/Config.cpp
  DBO/Config.h
  DBO/ConfigTable.h
  DBO/Diag.cpp
  DBO/Diag.h
  DBO/DiagStep.cpp
  DBO/DiagStep.h
  DBO/DiagStepTable.h
  DBO/DiagTable.h
  DBO/EventLog.cpp
  DBO/EventLog.h
  DBO/EventLogTable.h
  DBO/PrinterDotUsage.cpp
  DBO/PrinterDotUsage.h
  DBO/PrinterDotUsageTable.h
  DBO/PrinterDotWear.cpp
  DBO/PrinterDotWear.h
  DBO/PrinterDotWearTable.h
  DBO/PrinterHeadStatus.cpp
  DBO/PrinterHeadStatus.h
  DBO/PrinterHeadStatusTable.h
  DBO/ServiceAction.cpp
  DBO/ServiceAction.h
  DBO/ServiceActionTable.h
  DBO/ServiceCat.cpp
  DBO/ServiceCat.h
  DBO/ServiceCatTable.h
  DBO/ServiceMaintLog.cpp
  DBO/ServiceMaintLog.h
  DBO/ServiceMaintLogTable.h
  DBO/ServiceMethod.cpp
  DBO/ServiceMethod.h
  DBO/ServiceMethodTable.h
  DBO/ServicePart.cpp
  DBO/ServicePart.h
  DBO/ServicePartTable.h
  DBO/TableBase.h
  DBO/TableBaseImpl.h
  DBO/ValueMax.cpp
  DBO/ValueMax.h
  DBO/ValueMaxTable.h
  DBO/Wizards.cpp
  DBO/Wizards.h
  DBO/WizardsTable.h
  DBO/WrapperAuditLog.cpp
  DBO/WrapperAuditLog.h
  DBO/WrapperAuditLogTable.h
  DBO/WrapperTimings.cpp
  DBO/WrapperTimings.h
  DBO/WrapperTimingsTable.h
  DBO/WrapperTrays.cpp
  DBO/WrapperTrays.h
  DBO/WrapperTraysTable.h
  EdgeComm/DiagnosticsMsg.h
  EdgeComm/EdgePacket.cpp
  EdgeComm/EdgePacket.h
  EdgeComm/EventMsg.h
  EdgeComm/Header.h
  EdgeComm/HealthInfoMsg.h
  EdgeComm/PowerUpMsg.h
  EdgeComm/StatusCheckMsg.h
  Messages/DiagnosticsAuditLog.h
  Messages/DiagnosticsAuditLogServiceVisit.h
  Messages/DiagnosticsCheckErrorHandler.h
  Messages/DiagnosticsClearServiceMaintRecord.h
  Messages/DiagnosticsConfiguration.h
  Messages/DiagnosticsEvent.h
  Messages/DiagnosticsExportDatabase.h
  Messages/DiagnosticsInitWizard.h
  Messages/DiagnosticsMessageBasic.h
  Messages/DiagnosticsMessage.cpp
  Messages/DiagnosticsMessage.h
  Messages/DiagnosticsPerformWizardTesting.h
  Messages/DiagnosticsPrintLabelStats.h
  Messages/DiagnosticsPrintServiceMaintLog.h
  Messages/DiagnosticsReport.h
  Messages/DiagnosticsSendDatabase.h
  Messages/DiagnosticsServiceActions.h
  Messages/DiagnosticsServiceMaintLog.h
  Messages/DiagnosticsShowWizard.h
  Messages/DiagnosticsStartErrorHandler.h
  Messages/DiagnosticsStatus.h
  Messages/DiagnosticsValueMax.h
  Util/BasicSocket.cpp
  Util/BasicSocket.h
  Util/Dict.cpp
  Util/Dict.h
  Util/NetworkVal.cpp
  Util/NetworkVal.h
  Util/Timestamp.cpp
  Util/Timestamp.h
)

target_include_directories(DiagnosticsManager PUBLIC
  .
  DBO
  EdgeComm
  Messages
  Util
)

target_include_directories(DiagnosticsManager PRIVATE
  ../../db
  ../../HFC
  ../../app
  ../../../PAL
  ../../util
  ../../util/Inc
  ../../EvolutionRTC/MessageCenter
  ../../db/util
  ../../db/util/e_string
  ../../../inc/linux
  ../../MetrologicallySignificant
  ../../EvolutionRTC/WeigherManager/Include
  ../../EvolutionRTC/RTCManager/Include
  ../../EvolutionRTC/RendererManager/Include
  ../../EvolutionRTC/WrapperManager/Include
  ../../EvolutionRTC/OperatingModes/Include
  ../../EvolutionRTC/PrinterManager/Include
  ../../core
  ../../core/VersionManager/include
  ../../comm
  ../../EvolutionRTC/TimerManager/Include
  ../../EvolutionRTC/LabelApplierManager/Include
)