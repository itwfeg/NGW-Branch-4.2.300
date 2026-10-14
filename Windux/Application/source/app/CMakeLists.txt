cmake_minimum_required(VERSION 3.13.0)

target_sources(chobart_d PRIVATE
  AbstractSession.cpp
  BuildNumber.cpp
  DBSession.cpp
  Main.cpp
  Session.cpp
  SessionManager.cpp
  SessionManagerHelper.cpp
  SessionPrinter.cpp
  SessionWrapper.cpp
  ShutdownManager.cpp
  WSSession.cpp
  GCDSession.cpp
  chobart_d.cpp
)

target_include_directories(chobart_d PRIVATE . Messages)

target_sources(chobart_d PRIVATE
  Messages/AddNewSessionMessage.cpp
  Messages/AddNewSessionMessage.h
  Messages/ChangePrimaryScaleMessage.cpp
  Messages/ChangePrimaryScaleMessage.h
  Messages/ChangeSynchronizationTypeMessage.cpp
  Messages/ChangeSynchronizationTypeMessage.h
  Messages/DisableInputProcessingMessage.cpp
  Messages/DisableInputProcessingMessage.h
  Messages/RebootMessage.cpp
  Messages/RebootMessage.h
  Messages/RemoveSessionStarterMessage.cpp
  Messages/RemoveSessionStarterMessage.h
  Messages/UIDialogMsg.cpp
  Messages/UIDialogMsg.h
)