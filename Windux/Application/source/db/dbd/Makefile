##############################################################
#
# File : makefile
#
# Make file used for building db/dbd/
#
##############################################################

###################################
#defines
###################################
OS = linux

HOBART_APP ?= $(HOME)/projects/Hobart/shared_code/Application
export HOBART_APP
export HOBART_OUT_DIR=$(HOBART_APP)/output


##################################
#make vars
##################################

#
#Vars for Source and object files
#
SUB_DIRS :=
C_SRCS :=
CPP_SRCS := \
    dbcategory.cpp \
    dbcategoriesrm.cpp \
    dbbatch.cpp \
    dbbarcodescanner.cpp \
    dbbarcodescannersrm.cpp \
    dbbatchesrm.cpp \
    dbcategorydetailedstatistic.cpp \
    dbcategorydetailedstatisticsrm.cpp \
    dbcategorystatistic.cpp \
    dbcategorystatisticsrm.cpp \
    dbclass.cpp \
    dbclassesrm.cpp \
    dbconfiguration.cpp \
    dbconfigurationsrm.cpp \
    dbcoolcategoriesrm.cpp \
    dbcoolcategory.cpp \
    dbcoolcategorycountriesrm.cpp \
    dbcoolcategorycountry.cpp \
    dbcoolcategorypretext.cpp \
    dbcoolcategorypretextsrm.cpp \
    dbcoolcategoryproductionstep.cpp \
    dbcoolcategoryproductionstepsrm.cpp \
    dbcoolcountriesrm.cpp \
    dbcoolcountry.cpp \
    dbcoollist.cpp \
    dbcoollistsrm.cpp \
    dbcoollisttext.cpp \
    dbcoollisttextsrm.cpp \
    dbcoolpretext.cpp \
    dbcoolpretextsrm.cpp \
    dbcoolproductionstep.cpp \
    dbcoolproductionstepsrm.cpp \
    dbcooltext.cpp \
    dbcooltextsrm.cpp \
    dbcuttingtest.cpp \
    dbcuttingtestsrm.cpp \
    dbdept.cpp \
    dbdeptsrm.cpp \
    dbdevice.cpp \
    dbdevicesrm.cpp \
    dbemailalert.cpp \
    dbemailalertsrm.cpp \
    dbexpirationlabel.cpp \
    dbexpirationlabelsrm.cpp \
    dbflashfile.cpp \
    dbflashfilesrm.cpp \
    dbknifetimehistory.cpp \
    dbknifetimehistoryrm.cpp \
    dblabel.cpp \
    dblabelsrm.cpp \
    dblabeltext.cpp \
    dblabeltextsrm.cpp \
    dbnetworkerror.cpp \
    dbnetworkerrorsrm.cpp \
    dbnutrition.cpp \
    dbnutritionsrm.cpp \
    dboperator.cpp \
    dboperatornote.cpp \
    dboperatornotesrm.cpp \
    dboperatorsrm.cpp \
    dbpresentationmethod.cpp \
    dbpresentationmethoddetailedstatistic.cpp \
    dbpresentationmethoddetailedstatisticsrm.cpp \
    dbpresentationmethodsrm.cpp \
    dbpresentationmethodstatistic.cpp \
    dbpresentationmethodstatisticsrm.cpp \
    dbproduct.cpp \
    dbproductdefault.cpp \
    dbproductdefaultsrm.cpp \
    dbproductdetailedstatistic.cpp \
    dbproductdetailedstatisticsrm.cpp \
    dbproductsrm.cpp \
    dbproductstatistic.cpp \
    dbproductstatisticsrm.cpp \
    dbproducttext.cpp \
    dbproducttextcfg.cpp \
    dbproducttextcfgsrm.cpp \
    dbproducttextsrm.cpp \
    dbrecall.cpp \
    dbrecallsrm.cpp \
    dbromlabel.cpp \
    dbromlabelsrm.cpp \
    dbscale.cpp \
    dbscalesrm.cpp \
    dbsoftkey.cpp \
    dbsoftkeyfeature.cpp \
    dbsoftkeyfeaturesrm.cpp \
    dbsoftkeygroup.cpp \
    dbsoftkeygroupsrm.cpp \
    dbsoftkeysrm.cpp \
    dbsoftkeystyle.cpp \
    dbsoftkeystylesrm.cpp \
    dbspecialprogram.cpp \
    dbspecialprogramsrm.cpp \
    dbstatus.cpp \
    dbstatusrm.cpp \
    dbstatusstorage.cpp \
    dbstatusstoragerm.cpp \
    dbstreaminggraphic.cpp \
    dbstreaminggraphicsrm.cpp \
    dbsynchronizationchange.cpp \
    dbsynchronizationchangesrm.cpp \
    dbtransaction.cpp \
    dbtransactionsrm.cpp \
    dbtransactiontype.cpp \
    dbtransactiontypesrm.cpp \
    dbwrappererrorhistory.cpp \
    dbwrappererrorhistoryrm.cpp

INC_DIRS := \
    -I$(HOBART_APP)/PAL \
    -I$(HOBART_APP)/PAL/PAL_linux \
    -I$(HOBART_APP)/inc/linux \
    -I$(HOBART_APP)/source/app \
    -I$(HOBART_APP)/source/comm \
    -I$(HOBART_APP)/source/core \
    -I$(HOBART_APP)/source/core/Synchronization \
    -I$(HOBART_APP)/source/core/VersionManager/include \
    -I$(HOBART_APP)/source/db \
    -I$(HOBART_APP)/source/db/util \
    -I$(HOBART_APP)/source/db/util/e_string \
    -I$(HOBART_APP)/source/db/util/thread \
    -I$(HOBART_APP)/source/EvolutionRTC/MessageCenter \
    -I$(HOBART_APP)/source/EvolutionRTC/LabelApplierManager/Include \
    -I$(HOBART_APP)/source/EvolutionRTC/MarqueeManager/Include \
    -I$(HOBART_APP)/source/EvolutionRTC/NumericManager/Include \
    -I$(HOBART_APP)/source/EvolutionRTC/OperatingModes/Include \
    -I$(HOBART_APP)/source/EvolutionRTC/PngGraphics/Include \
    -I$(HOBART_APP)/source/EvolutionRTC/PrinterManager/Include \
    -I$(HOBART_APP)/source/EvolutionRTC/ProductDev/Include \
    -I$(HOBART_APP)/source/EvolutionRTC/RendererManager/Include \
    -I$(HOBART_APP)/source/EvolutionRTC/RFIDManager/Include \
    -I$(HOBART_APP)/source/EvolutionRTC/RTCManager/Include \
    -I$(HOBART_APP)/source/EvolutionRTC/ScannerManager/Include \
    -I$(HOBART_APP)/source/EvolutionRTC/TimerManager/Include \
    -I$(HOBART_APP)/source/EvolutionRTC/USBCommManagers/Include \
    -I$(HOBART_APP)/source/EvolutionRTC/WeigherManager/Include \
    -I$(HOBART_APP)/source/EvolutionRTC/WrapperManager/Include \
    -I$(HOBART_APP)/source/HFC \
    -I$(HOBART_APP)/source/HFC/devices \
    -I$(HOBART_APP)/source/HFC/messages \
    -I$(HOBART_APP)/source/HFC/reports \
    -I$(HOBART_APP)/source/HFC/tags \
    -I$(HOBART_APP)/source/util \
    -I$(HOBART_APP)/source/util/Inc \
    -I$(HOBART_APP)/HobartCustomerHook \
    -I$(HOBART_APP)/source/MetrologicallySignificant \

ARCHIVES := \
    $(HOBART_OUT_DIR)/source_db_dbm.a \
    $(HOBART_OUT_DIR)/source_db_util_e_exception.a \
    $(HOBART_OUT_DIR)/source_db_util_accountant.a \
    $(HOBART_OUT_DIR)/source_db_util_e_string.a \
    $(HOBART_OUT_DIR)/source_db_util_platform.a \
    $(HOBART_OUT_DIR)/source_db_util_filesys.a \
    $(HOBART_OUT_DIR)/source_db_util_checksums.a \
    $(HOBART_OUT_DIR)/source_db_util_time.a \
    $(HOBART_OUT_DIR)/source_util.a \
    $(HOBART_OUT_DIR)/source_db_util_properties.a \
    $(HOBART_OUT_DIR)/source_EvolutionRTC_MessageCenter.a \
    $(HOBART_OUT_DIR)/source_util_Src.a \
    $(HOBART_OUT_DIR)/source_app.a \
    $(HOBART_OUT_DIR)/source_HFC.a \
    $(HOBART_OUT_DIR)/PAL_PAL_linux.a \


##################################
# makefile template
##################################

include $(HOBART_APP)/hobart.mk
