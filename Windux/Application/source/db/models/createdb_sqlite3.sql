-- ============================================================
--   Database name:  Database                                  
--   DBMS name:      Sqlite 3                                  
--   Created on:     1/16/2013  11:58 AM                       
-- ============================================================

-- ============================================================
--   Table: Depts                                              
-- ============================================================
create table Depts
(
    DpNum       INTEGER               not null,
    DpName      TEXT                          ,
    DpCheck     INTEGER                       ,
    primary key (DpNum)
);

-- ============================================================
--   Index: Depts_PK                                           
-- ============================================================
create unique index Depts_PK on Depts (DpNum asc);

-- ============================================================
--   Table: LTexts                                             
-- ============================================================
create table LTexts
(
    LTNum       INTEGER               not null,
    LTText      TEXT                          ,
    LTCheck     INTEGER                       ,
    primary key (LTNum)
);

-- ============================================================
--   Index: LTexts_PK                                          
-- ============================================================
create unique index LTexts_PK on LTexts (LTNum asc);

-- ============================================================
--   Table: ONotes                                             
-- ============================================================
create table ONotes
(
    ONText      TEXT                          ,
    ONCheck     INTEGER                       
);

-- ============================================================
--   Table: ETotals                                            
-- ============================================================
create table ETotals
(
    ETECode     INTEGER               not null,
    ETTotErr    INTEGER                       ,
    ETCheck     INTEGER                       ,
    primary key (ETECode)
);

-- ============================================================
--   Index: ETotals_PK                                         
-- ============================================================
create unique index ETotals_PK on ETotals (ETECode asc);

-- ============================================================
--   Table: WTotals                                            
-- ============================================================
create table WTotals
(
    WTPckCd     INTEGER               not null,
    WTPcks      INTEGER                       ,
    WTJams      INTEGER                       ,
    WTCheck     INTEGER                       ,
    primary key (WTPckCd)
);

-- ============================================================
--   Index: WTotals_PK                                         
-- ============================================================
create unique index WTotals_PK on WTotals (WTPckCd asc);

-- ============================================================
--   Table: Configs                                            
-- ============================================================
create table Configs
(
    CoName      TEXT                  not null,
    CoValue     TEXT                          ,
    CoCheck     INTEGER                       ,
    primary key (CoName)
);

-- ============================================================
--   Index: Configs_PK                                         
-- ============================================================
create unique index Configs_PK on Configs (CoName asc);

-- ============================================================
--   Table: RDates                                             
-- ============================================================
create table RDates
(
    RDRecType   TEXT                  not null,
    RDDate      DATETIME                      ,
    RDCheck     INTEGER                       ,
    primary key (RDRecType)
);

-- ============================================================
--   Index: RDates_PK                                          
-- ============================================================
create unique index RDates_PK on RDates (RDRecType asc);

-- ============================================================
--   Table: AutoInc                                            
-- ============================================================
create table AutoInc
(
    AIPrID      INTEGER               not null,
    AIOCID      INTEGER               not null,
    AIYCID      INTEGER               not null,
    AINEID      INTEGER               not null,
    AIScID      INTEGER               not null,
    AILOID      INTEGER               not null,
    AILSID      INTEGER               not null,
    AILUID      INTEGER               not null,
    AILLID      INTEGER               not null,
    AIELID      INTEGER               not null,
    AITRID      INTEGER               not null,
    AIWEID      INTEGER               not null,
    AICheck     INTEGER                       
);

-- ============================================================
--   Table: PrDefs                                             
-- ============================================================
create table PrDefs
(
    PDType      INTEGER               not null
        check (
            PDType in (1,2,3,4)),
    PrPrice     INTEGER                       ,
    PrPrMod     INTEGER                       ,
    PrIsFrcPr   INTBOOLEAN                    ,
    PrExcPr     INTEGER                       ,
    PrDscMe     INTEGER                       
        check (
            PrDscMe is null or (PrDscMe in (0,1,2,3))),
    PrDscPr1    INTEGER                       ,
    PrDscPr2    INTEGER                       ,
    PrDscPr3    INTEGER                       ,
    PrTare      INTEGER                       ,
    PrIsFrcTr   INTBOOLEAN                    ,
    PrPrpTare   INTEGER                       ,
    PrNetWt     INTEGER                       ,
    PrByCnt     INTEGER                       ,
    PrIsFrcBC   INTBOOLEAN                    ,
    PrShlfLfD   INTEGER                       ,
    PrShlfLfH   INTEGER                       ,
    PrProdLfD   INTEGER                       ,
    PrProdLfH   INTEGER                       ,
    PrIsPrnSL   INTBOOLEAN                    ,
    PrIsPrnPL   INTBOOLEAN                    ,
    PrIsPrnPO   INTBOOLEAN                    ,
    PrBnsPts    INTEGER                       ,
    PrPrtns     INTEGER                       ,
    PrIsUsSL    INTBOOLEAN                    ,
    PrInMode    INTEGER                       
        check (
            PrInMode is null or (PrInMode in (0,1,2,3))),
    PrLblRot1   INTEGER                       ,
    PrLblPlc1   INTEGER                       ,
    PrLblRot2   INTEGER                       ,
    PrLblPlc2   INTEGER                       ,
    PrLblRot3   INTEGER                       ,
    PrLblPlc3   INTEGER                       ,
    PrName1     TEXT                          ,
    PrName2     TEXT                          ,
    PrVCOLTxt   INTBOOLEAN                    ,
    PrCOLReq    INTBOOLEAN                    ,
    PrPrPlT     INTEGER                       ,
    PDClNum     INTEGER                       ,
    PDTxNum     INTEGER                       ,
    PDSMNum     INTEGER                       ,
    PDNFNum     INTEGER                       ,
    PDMaNum     INTEGER                       ,
    PDNtNum     INTEGER                       ,
    PDCaNum     INTEGER                       ,
    PDLbNum1    INTEGER                       ,
    PDLbNum2    INTEGER                       ,
    PDLbNum3    INTEGER                       ,
    PDSPNum     INTEGER                       ,
    PDLCNum     INTEGER                       ,
    PDDXNum     INTEGER                       ,
    PDLPNum     INTEGER                       ,
    PDLLNum     INTEGER                       ,
    PDUPCNS     INTEGER                       ,
    PDEANFS     TEXT                          ,
    PDEANPr     INTEGER                       ,
    PDDesc      TEXT                          ,
    PDGrNm1     TEXT                          ,
    PDGrNm2     TEXT                          ,
    PDGrNm3     TEXT                          ,
    PDGrNm4     TEXT                          ,
    PDImage     TEXT                          ,
    PDCOLTN     TEXT                          ,
    PDCheck     INTEGER                       ,
    primary key (PDType)
);

-- ============================================================
--   Index: PrDefs_PK                                          
-- ============================================================
create unique index PrDefs_PK on PrDefs (PDType asc);

-- ============================================================
--   Table: RLabels                                            
-- ============================================================
create table RLabels
(
    RLbNum      INTEGER               not null,
    RLbBin      TEXT                          ,
    RLbCheck    INTEGER                       ,
    primary key (RLbNum)
);

-- ============================================================
--   Index: RLabels_PK                                         
-- ============================================================
create unique index RLabels_PK on RLabels (RLbNum asc);

-- ============================================================
--   Table: FlshFls                                            
-- ============================================================
create table FlshFls
(
    FFName      TEXT                  not null,
    FFDate      DATETIME                      ,
    FFCmplt     INTBOOLEAN                    ,
    FFCheck     INTEGER                       ,
    primary key (FFName)
);

-- ============================================================
--   Index: FlshFls_PK                                         
-- ============================================================
create unique index FlshFls_PK on FlshFls (FFName asc);

-- ============================================================
--   Table: SGraphs                                            
-- ============================================================
create table SGraphs
(
    SGNum       INTEGER               not null,
    SGr_SGNum   INTEGER                       ,
    SGTime      INTEGER                       ,
    SGActT      INTEGER                       ,
    SGActP      TEXT                          ,
    SGImage     TEXT                          ,
    SGCheck     INTEGER                       ,
    primary key (SGNum),
    foreign key  (SGr_SGNum)
       references SGraphs (SGNum)
);

-- ============================================================
--   Index: SGraphs_PK                                         
-- ============================================================
create unique index SGraphs_PK on SGraphs (SGNum asc);

-- ============================================================
--   Index: SGPrevs_FK                                         
-- ============================================================
create index SGPrevs_FK on SGraphs (SGr_SGNum asc);

-- ============================================================
--   Table: EAlerts                                            
-- ============================================================
create table EAlerts
(
    EANum       INTEGER               not null,
    EAFail      INTBOOLEAN                    ,
    EAResol     INTBOOLEAN                    ,
    EADelay     INTEGER                       ,
    EAFSent     INTBOOLEAN                    ,
    EASubj1     TEXT                          ,
    EASubj2     TEXT                          ,
    EACheck     INTEGER                       ,
    primary key (EANum)
);

-- ============================================================
--   Index: EAlerts_PK                                         
-- ============================================================
create unique index EAlerts_PK on EAlerts (EANum asc);

-- ============================================================
--   Table: IgWords                                            
-- ============================================================
create table IgWords
(
    IWLang      TEXT                  not null,
    IWName      TEXT                  not null,
    IWCheck     INTEGER                       ,
    primary key (IWLang, IWName)
);

-- ============================================================
--   Index: IgWords_PK                                         
-- ============================================================
create unique index IgWords_PK on IgWords (IWLang asc, IWName asc);

-- ============================================================
--   Table: WErrors                                            
-- ============================================================
create table WErrors
(
    WEID        INTEGER               not null,
    WECode      INTEGER               not null,
    WEDtTm      DATETIME              not null,
    WEPLen      INTEGER                       ,
    WEPWdth     INTEGER                       ,
    WEPHght     INTEGER                       ,
    WEFlmSl     INTEGER                       ,
    WETrType    TEXT                          ,
    WEBeFit     INTBOOLEAN                    ,
    WEPerNet    INTBOOLEAN                    ,
    WECheck     INTEGER                       ,
    primary key (WEID)
);

-- ============================================================
--   Index: WErrors_PK                                         
-- ============================================================
create unique index WErrors_PK on WErrors (WEID asc);

-- ============================================================
--   Table: ELabels                                            
-- ============================================================
create table ELabels
(
    ELID        INTEGER               not null,
    ELPrNum     INTEGER                       ,
    ELPrDesc    TEXT                          ,
    ELExpDT     DATETIME                      ,
    ELCheck     INTEGER                       ,
    primary key (ELID)
);

-- ============================================================
--   Index: ELabels_PK                                         
-- ============================================================
create unique index ELabels_PK on ELabels (ELID asc);

-- ============================================================
--   Index: ELExpDT_IN                                         
-- ============================================================
create index ELExpDT_IN on ELabels (ELExpDT asc);

-- ============================================================
--   Index: ELPrNum_IN                                         
-- ============================================================
create index ELPrNum_IN on ELabels (ELPrNum asc);

-- ============================================================
--   Table: CutTsts                                            
-- ============================================================
create table CutTsts
(
    UTNum       INTEGER               not null,
    OpNum       INTEGER                       ,
    UTStrDt     DATETIME                      ,
    UTEndDt     DATETIME                      ,
    UTPmPrN     INTEGER                       ,
    UTPmVdN     INTEGER                       ,
    UTPmWgt     INTEGER                       ,
    UTPmCst     INTEGER                       ,
    UTCheck     INTEGER                       ,
    primary key (UTNum)
);

-- ============================================================
--   Index: CutTsts_PK                                         
-- ============================================================
create unique index CutTsts_PK on CutTsts (UTNum asc);

-- ============================================================
--   Index: OpCTsts_FK                                         
-- ============================================================
create index OpCTsts_FK on CutTsts (OpNum asc);

-- ============================================================
--   Table: WKnfTms                                            
-- ============================================================
create table WKnfTms
(
    WKType      INTEGER               not null,
    WKTmng      INTEGER                       ,
    WKDtTm      DATETIME                      ,
    WKCheck     INTEGER                       ,
    primary key (WKType)
);

-- ============================================================
--   Index: WKnfTms_PK                                         
-- ============================================================
create unique index WKnfTms_PK on WKnfTms (WKType asc);

-- ============================================================
--   Table: Classes                                            
-- ============================================================
create table Classes
(
    ClNum       INTEGER               not null,
    Cla_ClNum   INTEGER                       ,
    ClDesc1     TEXT                          ,
    ClDesc2     TEXT                          ,
    ClCheck     INTEGER                       ,
    primary key (ClNum),
    foreign key  (Cla_ClNum)
       references Classes (ClNum)
);

-- ============================================================
--   Index: Classes_PK                                         
-- ============================================================
create unique index Classes_PK on Classes (ClNum asc);

-- ============================================================
--   Index: ClRprt_FK                                          
-- ============================================================
create index ClRprt_FK on Classes (Cla_ClNum asc);

-- ============================================================
--   Table: Texts                                              
-- ============================================================
create table Texts
(
    TxNum       INTEGER               not null,
    TxDesc      TEXT                          ,
    TxCheck     INTEGER                       ,
    primary key (TxNum)
);

-- ============================================================
--   Index: Texts_PK                                           
-- ============================================================
create unique index Texts_PK on Texts (TxNum asc);

-- ============================================================
--   Table: SMsgs                                              
-- ============================================================
create table SMsgs
(
    SMNum       INTEGER               not null,
    SMDesc      TEXT                          ,
    SMCheck     INTEGER                       ,
    primary key (SMNum)
);

-- ============================================================
--   Index: SMsgs_PK                                           
-- ============================================================
create unique index SMsgs_PK on SMsgs (SMNum asc);

-- ============================================================
--   Table: Labels                                             
-- ============================================================
create table Labels
(
    LbNum       INTEGER               not null,
    LbBin       TEXT                          ,
    LbCheck     INTEGER                       ,
    primary key (LbNum)
);

-- ============================================================
--   Index: Labels_PK                                          
-- ============================================================
create unique index Labels_PK on Labels (LbNum asc);

-- ============================================================
--   Table: Opers                                              
-- ============================================================
create table Opers
(
    OpNum       TEXT                  not null,
    OpRFID      TEXT                          ,
    OpAccLev    INTEGER                       ,
    OpPLang     INTEGER                       ,
    OpSawNote   INTBOOLEAN                    ,
    OpName      TEXT                          ,
    OpCheck     INTEGER                       ,
    primary key (OpNum)
);

-- ============================================================
--   Index: Opers_PK                                           
-- ============================================================
create unique index Opers_PK on Opers (OpNum asc);

-- ============================================================
--   Index: OpRFID_IN                                          
-- ============================================================
create index OpRFID_IN on Opers (OpRFID asc);

-- ============================================================
--   Table: NutrFs                                             
-- ============================================================
create table NutrFs
(
    NFNum       INTEGER               not null,
    NFCol1      TEXT                          ,
    NFCol2      TEXT                          ,
    NFCol3      TEXT                          ,
    NFCol4      TEXT                          ,
    NFCheck     INTEGER                       ,
    primary key (NFNum)
);

-- ============================================================
--   Index: NutrFs_PK                                          
-- ============================================================
create unique index NutrFs_PK on NutrFs (NFNum asc);

-- ============================================================
--   Table: Notes                                              
-- ============================================================
create table Notes
(
    NtNum       INTEGER               not null,
    NtText      TEXT                          ,
    NtCheck     INTEGER                       ,
    primary key (NtNum)
);

-- ============================================================
--   Index: Notes_PK                                           
-- ============================================================
create unique index Notes_PK on Notes (NtNum asc);

-- ============================================================
--   Table: Scales                                             
-- ============================================================
create table Scales
(
    ScID        INTEGER               not null,
    ScHstNm     TEXT                          ,
    ScIPAddr    TEXT                          ,
    ScType      INTEGER               not null,
    ScCreatDT   DATETIME                      ,
    ScState     INTEGER                       ,
    ScStateDT   DATETIME                      ,
    ScRegis     INTBOOLEAN                    ,
    ScDReg      INTBOOLEAN                    ,
    ScNBatch    INTBOOLEAN                    ,
    ScNBtchDT   DATETIME                      ,
    ScNOnLn     INTBOOLEAN                    ,
    ScNOnLnDT   DATETIME                      ,
    ScRetCt     INTEGER                       ,
    ScVer       INTEGER                       ,
    ScVerRec    INTEGER                       ,
    ScVerRecDT  DATETIME                      ,
    ScLSuccCm   DATETIME                      ,
    ScFFailCm   DATETIME                      ,
    ScTSuccCm   INTEGER                       ,
    ScTFailCm   INTEGER                       ,
    ScTBusyCm   INTEGER                       ,
    ScCheck     INTEGER                       ,
    primary key (ScID)
);

-- ============================================================
--   Index: Scales_PK                                          
-- ============================================================
create unique index Scales_PK on Scales (ScID asc);

-- ============================================================
--   Index: ScType_IN                                          
-- ============================================================
create index ScType_IN on Scales (ScType asc);

-- ============================================================
--   Index: ScHstNm_IN                                         
-- ============================================================
create index ScHstNm_IN on Scales (ScHstNm asc);

-- ============================================================
--   Index: ScIPAdd_IN                                         
-- ============================================================
create index ScIPAdd_IN on Scales (ScIPAddr asc);

-- ============================================================
--   Table: Devices                                            
-- ============================================================
create table Devices
(
    DvType      INTEGER               not null,
    DvSType     INTEGER               not null,
    DvKind      INTEGER                       ,
    DvSrlNum    TEXT                          ,
    DvHWVer     TEXT                          ,
    DvSWVer     TEXT                          ,
    DvChkPr     INTBOOLEAN                    ,
    DvInstDt    DATETIME                      ,
    DvSrvHst    TEXT                          ,
    DvCheck     INTEGER                       ,
    primary key (DvType, DvSType)
);

-- ============================================================
--   Index: Devices_PK                                         
-- ============================================================
create unique index Devices_PK on Devices (DvType asc, DvSType asc);

-- ============================================================
--   Table: Categrs                                            
-- ============================================================
create table Categrs
(
    CaNum       INTEGER               not null,
    Cat_CaNum   INTEGER                       ,
    CaName1     TEXT                          ,
    CaName2     TEXT                          ,
    CaCdNm1     TEXT                          ,
    CaCdNm2     TEXT                          ,
    CaBgClr     INTEGER                       ,
    CaTxClr     INTEGER                       ,
    CaImage     TEXT                          ,
    CaCheck     INTEGER                       ,
    primary key (CaNum),
    foreign key  (Cat_CaNum)
       references Categrs (CaNum)
);

-- ============================================================
--   Index: Categrs_PK                                         
-- ============================================================
create unique index Categrs_PK on Categrs (CaNum asc);

-- ============================================================
--   Index: SubCats_FK                                         
-- ============================================================
create index SubCats_FK on Categrs (Cat_CaNum asc);

-- ============================================================
--   Index: CaName1_IN                                         
-- ============================================================
create index CaName1_IN on Categrs (CaCdNm1 asc);

-- ============================================================
--   Index: CaName2_IN                                         
-- ============================================================
create index CaName2_IN on Categrs (CaCdNm2 asc);

-- ============================================================
--   Table: PMethds                                            
-- ============================================================
create table PMethds
(
    PMType      INTEGER               not null,
    PMName1     TEXT                          ,
    PMName2     TEXT                          ,
    PMCheck     INTEGER                       ,
    primary key (PMType)
);

-- ============================================================
--   Index: PMethds_PK                                         
-- ============================================================
create unique index PMethds_PK on PMethds (PMType asc);

-- ============================================================
--   Table: Spcials                                            
-- ============================================================
create table Spcials
(
    SPNum       INTEGER               not null,
    SPName1     TEXT                          ,
    SPName2     TEXT                          ,
    SPDefL      INTEGER                       ,
    SPImag1     TEXT                          ,
    SPImag2     TEXT                          ,
    SPCheck     INTEGER                       ,
    primary key (SPNum)
);

-- ============================================================
--   Index: Spcials_PK                                         
-- ============================================================
create unique index Spcials_PK on Spcials (SPNum asc);

-- ============================================================
--   Table: SStyles                                            
-- ============================================================
create table SStyles
(
    SSNum       INTEGER               not null,
    SSDTextL    INTEGER                       ,
    SSDTextC    INTEGER                       ,
    SSDGrTxtC   INTEGER                       ,
    SSDFImgL    INTEGER                       ,
    SSDBgClr    INTEGER                       ,
    SSDBgImg    TEXT                          ,
    SSImgDir    TEXT                          ,
    SSCheck     INTEGER                       ,
    primary key (SSNum)
);

-- ============================================================
--   Index: SStyles_PK                                         
-- ============================================================
create unique index SStyles_PK on SStyles (SSNum asc);

-- ============================================================
--   Table: COLTxts                                            
-- ============================================================
create table COLTxts
(
    LXNum       INTEGER               not null,
    LXDesc      TEXT                          ,
    LXCheck     INTEGER                       ,
    primary key (LXNum)
);

-- ============================================================
--   Index: COLTxts_PK                                         
-- ============================================================
create unique index COLTxts_PK on COLTxts (LXNum asc);

-- ============================================================
--   Table: COLCats                                            
-- ============================================================
create table COLCats
(
    LCNum       INTEGER               not null,
    LCName1     TEXT                          ,
    LCName2     TEXT                          ,
    LCCheck     INTEGER                       ,
    primary key (LCNum)
);

-- ============================================================
--   Index: COLCats_PK                                         
-- ============================================================
create unique index COLCats_PK on COLCats (LCNum asc);

-- ============================================================
--   Table: COLPTxs                                            
-- ============================================================
create table COLPTxs
(
    LPNum       INTEGER               not null,
    LPName1     TEXT                          ,
    LPName2     TEXT                          ,
    LPAllCa     INTBOOLEAN                    ,
    LPCheck     INTEGER                       ,
    primary key (LPNum)
);

-- ============================================================
--   Index: COLPTxs_PK                                         
-- ============================================================
create unique index COLPTxs_PK on COLPTxs (LPNum asc);

-- ============================================================
--   Index: LPName1_IN                                         
-- ============================================================
create index LPName1_IN on COLPTxs (LPName1 asc);

-- ============================================================
--   Index: LPAllCa_IN                                         
-- ============================================================
create index LPAllCa_IN on COLPTxs (LPAllCa asc);

-- ============================================================
--   Table: COLCtrs                                            
-- ============================================================
create table COLCtrs
(
    LUID        INTEGER               not null,
    LUName1     TEXT                  not null,
    LUName2     TEXT                          ,
    LUAllCa     INTBOOLEAN                    ,
    LUKey       INTEGER                       ,
    LUCheck     INTEGER                       ,
    primary key (LUID)
);

-- ============================================================
--   Index: COLCtrs_PK                                         
-- ============================================================
create unique index COLCtrs_PK on COLCtrs (LUID asc);

-- ============================================================
--   Index: LUName1_AK                                         
-- ============================================================
create unique index LUName1_AK on COLCtrs (LUName1 asc);

-- ============================================================
--   Index: LUAllCa_IN                                         
-- ============================================================
create index LUAllCa_IN on COLCtrs (LUAllCa asc);

-- ============================================================
--   Table: COLStps                                            
-- ============================================================
create table COLStps
(
    LSID        INTEGER               not null,
    LSName1     TEXT                  not null,
    LSName2     TEXT                          ,
    LSAllCa     INTBOOLEAN                    ,
    LSKey       INTEGER                       ,
    LSCheck     INTEGER                       ,
    primary key (LSID)
);

-- ============================================================
--   Index: COLStps_PK                                         
-- ============================================================
create unique index COLStps_PK on COLStps (LSID asc);

-- ============================================================
--   Index: LSName1_AK                                         
-- ============================================================
create unique index LSName1_AK on COLStps (LSName1 asc);

-- ============================================================
--   Index: LSAllCa_IN                                         
-- ============================================================
create index LSAllCa_IN on COLStps (LSAllCa asc);

-- ============================================================
--   Table: COLLsts                                            
-- ============================================================
create table COLLsts
(
    LLID        INTEGER               not null,
    LLNum       INTEGER                       ,
    LLFLink     INTEGER                       ,
    LLCheck     INTEGER                       ,
    primary key (LLID)
);

-- ============================================================
--   Index: COLLsts_PK                                         
-- ============================================================
create unique index COLLsts_PK on COLLsts (LLID asc);

-- ============================================================
--   Index: LLNum_IN                                           
-- ============================================================
create index LLNum_IN on COLLsts (LLNum asc);

-- ============================================================
--   Index: LLFLink_IN                                         
-- ============================================================
create index LLFLink_IN on COLLsts (LLFLink asc);

-- ============================================================
--   Table: SKGrps                                             
-- ============================================================
create table SKGrps
(
    SGNum       INTEGER               not null,
    SGHGrid     INTEGER                       ,
    SGVGrid     INTEGER                       ,
    SGHSpc      INTEGER                       ,
    SGVSpc      INTEGER                       ,
    SGCheck     INTEGER                       ,
    primary key (SGNum)
);

-- ============================================================
--   Index: SKGrps_PK                                          
-- ============================================================
create unique index SKGrps_PK on SKGrps (SGNum asc);

-- ============================================================
--   Table: Mrquees                                            
-- ============================================================
create table Mrquees
(
    MaNum       INTEGER               not null,
    MaDesc1     TEXT                          ,
    MaDesc2     TEXT                          ,
    MaCheck     INTEGER                       ,
    primary key (MaNum)
);

-- ============================================================
--   Index: Mrquees_PK                                         
-- ============================================================
create unique index Mrquees_PK on Mrquees (MaNum asc);

-- ============================================================
--   Table: TrTypes                                            
-- ============================================================
create table TrTypes
(
    TTNum       INTEGER               not null,
    TTName1     TEXT                          ,
    TTName2     TEXT                          ,
    TTAffPr     INTEGER                       ,
    TTBkDt      INTBOOLEAN                    ,
    TTAutoUPC   INTBOOLEAN                    ,
    TTManUPC    INTBOOLEAN                    ,
    TTUPCDigit  TEXT                          ,
    TTCheck     INTEGER                       ,
    primary key (TTNum)
);

-- ============================================================
--   Index: TrTypes_PK                                         
-- ============================================================
create unique index TrTypes_PK on TrTypes (TTNum asc);

-- ============================================================
--   Table: Batches                                            
-- ============================================================
create table Batches
(
    BaNum       INTEGER               not null,
    ScID        INTEGER                       ,
    BaVer       INTEGER                       ,
    BaRVer      INTEGER                       ,
    BaOrigin    INTEGER                       ,
    BaType      INTEGER                       ,
    BaCreaDT    DATETIME                      ,
    BaComp      INTBOOLEAN                    ,
    BaCompDT    DATETIME                      ,
    BaSig       TEXT                          ,
    BaPAppl     INTBOOLEAN                    ,
    BaApplDT    DATETIME                      ,
    BaPSync     INTBOOLEAN                    ,
    BaSyncDT    DATETIME                      ,
    BaPDelt     INTBOOLEAN                    ,
    BaDeltDT    DATETIME                      ,
    BaCheck     INTEGER                       ,
    primary key (BaNum),
    foreign key  (ScID)
       references Scales (ScID)
);

-- ============================================================
--   Index: Batches_PK                                         
-- ============================================================
create unique index Batches_PK on Batches (BaNum asc);

-- ============================================================
--   Index: ScBtchs_FK                                         
-- ============================================================
create index ScBtchs_FK on Batches (ScID asc);

-- ============================================================
--   Index: BaVer_IN                                           
-- ============================================================
create index BaVer_IN on Batches (BaVer asc);

-- ============================================================
--   Index: BaSig_IN                                           
-- ============================================================
create index BaSig_IN on Batches (BaSig asc);

-- ============================================================
--   Table: CaStats                                            
-- ============================================================
create table CaStats
(
    CaNum       INTEGER               not null,
    CSLang      TEXT                  not null,
    CSHrNum     INTEGER               not null,
    CSTotSl     INTEGER                       ,
    CSCheck     INTEGER                       ,
    primary key (CaNum, CSLang, CSHrNum),
    foreign key  (CaNum)
       references Categrs (CaNum)
);

-- ============================================================
--   Index: CaStats_PK                                         
-- ============================================================
create unique index CaStats_PK on CaStats (CaNum asc, CSLang asc, CSHrNum asc);

-- ============================================================
--   Index: CaStats_FK                                         
-- ============================================================
create index CaStats_FK on CaStats (CaNum asc);

-- ============================================================
--   Index: CSMSlct_IN                                         
-- ============================================================
create index CSMSlct_IN on CaStats (CSTotSl desc);

-- ============================================================
--   Table: PMStats                                            
-- ============================================================
create table PMStats
(
    PMType      INTEGER               not null,
    MSLang      TEXT                  not null,
    MSHrNum     INTEGER               not null,
    MSTtMSl     INTEGER                       ,
    MSTtCSl     INTEGER                       ,
    MSTtPSl     INTEGER                       ,
    MSTtLPr     INTEGER                       ,
    MSCheck     INTEGER                       ,
    primary key (PMType, MSLang, MSHrNum),
    foreign key  (PMType)
       references PMethds (PMType)
);

-- ============================================================
--   Index: PMStats_PK                                         
-- ============================================================
create unique index PMStats_PK on PMStats (PMType asc, MSLang asc, MSHrNum asc);

-- ============================================================
--   Index: PMStats_FK                                         
-- ============================================================
create index PMStats_FK on PMStats (PMType asc);

-- ============================================================
--   Table: SFeatrs                                            
-- ============================================================
create table SFeatrs
(
    SFNum       INTEGER               not null,
    SSNum       INTEGER                       ,
    SGNum       INTEGER                       ,
    SFDHGrid    INTEGER                       ,
    SFDVGrid    INTEGER                       ,
    SFDHSpc     INTEGER                       ,
    SFDVSpc     INTEGER                       ,
    SFCheck     INTEGER                       ,
    primary key (SFNum),
    foreign key  (SSNum)
       references SStyles (SSNum),
    foreign key  (SGNum)
       references SKGrps (SGNum)
);

-- ============================================================
--   Index: SFeatrs_PK                                         
-- ============================================================
create unique index SFeatrs_PK on SFeatrs (SFNum asc);

-- ============================================================
--   Table: Prods                                              
-- ============================================================
create table Prods
(
    PrID        INTEGER               not null,
    ClNum       INTEGER                       ,
    TxNum       INTEGER                       ,
    SMNum       INTEGER                       ,
    NFNum       INTEGER                       ,
    NtNum       INTEGER                       ,
    LbNum1      INTEGER                       ,
    LbNum2      INTEGER                       ,
    LbNum3      INTEGER                       ,
    CaNum       INTEGER                       ,
    SPNum       INTEGER                       ,
    LCNum       INTEGER                       ,
    LPNum       INTEGER                       ,
    LXNum       INTEGER                       ,
    DLXNum      INTEGER                       ,
    LLID        INTEGER                       ,
    MaNum       INTEGER                       ,
    PrPrice     INTEGER                       ,
    PrPrMod     INTEGER                       ,
    PrIsFrcPr   INTBOOLEAN                    ,
    PrExcPr     INTEGER                       ,
    PrDscMe     INTEGER                       
        check (
            PrDscMe is null or (PrDscMe in (0,1,2,3))),
    PrDscPr1    INTEGER                       ,
    PrDscPr2    INTEGER                       ,
    PrDscPr3    INTEGER                       ,
    PrTare      INTEGER                       ,
    PrIsFrcTr   INTBOOLEAN                    ,
    PrPrpTare   INTEGER                       ,
    PrNetWt     INTEGER                       ,
    PrByCnt     INTEGER                       ,
    PrIsFrcBC   INTBOOLEAN                    ,
    PrShlfLfD   INTEGER                       ,
    PrShlfLfH   INTEGER                       ,
    PrProdLfD   INTEGER                       ,
    PrProdLfH   INTEGER                       ,
    PrIsPrnSL   INTBOOLEAN                    ,
    PrIsPrnPL   INTBOOLEAN                    ,
    PrIsPrnPO   INTBOOLEAN                    ,
    PrBnsPts    INTEGER                       ,
    PrPrtns     INTEGER                       ,
    PrIsUsSL    INTBOOLEAN                    ,
    PrInMode    INTEGER                       
        check (
            PrInMode is null or (PrInMode in (0,1,2,3))),
    PrLblRot1   INTEGER                       ,
    PrLblPlc1   INTEGER                       ,
    PrLblRot2   INTEGER                       ,
    PrLblPlc2   INTEGER                       ,
    PrLblRot3   INTEGER                       ,
    PrLblPlc3   INTEGER                       ,
    PrName1     TEXT                          ,
    PrName2     TEXT                          ,
    PrVCOLTxt   INTBOOLEAN                    ,
    PrCOLReq    INTBOOLEAN                    ,
    PrPrPlT     INTEGER                       ,
    PrNum       INTEGER               not null,
    PrType      INTEGER                       
        check (
            PrType is null or (PrType in (1,2,3,4))),
    PrBarPref   INTEGER                       ,
    PrBarProN   INTEGER                       ,
    PrBarVenN   INTEGER                       ,
    PrEANCd     INTEGER                       ,
    PrRawBarC   TEXT                          ,
    PrCdNm1     TEXT                          ,
    PrCdNm2     TEXT                          ,
    PrSpcSD     DATETIME                      ,
    PrDesc      TEXT                          ,
    PrGrNm1     TEXT                          ,
    PrGrNm2     TEXT                          ,
    PrGrNm3     TEXT                          ,
    PrGrNm4     TEXT                          ,
    PrImage     TEXT                          ,
    PrCOLTN     TEXT                          ,
    PrCheck     INTEGER                       ,
    primary key (PrID),
    foreign key  (ClNum)
       references Classes (ClNum),
    foreign key  (TxNum)
       references Texts (TxNum),
    foreign key  (SMNum)
       references SMsgs (SMNum),
    foreign key  (NFNum)
       references NutrFs (NFNum),
    foreign key  (NtNum)
       references Notes (NtNum),
    foreign key  (LbNum1)
       references Labels (LbNum),
    foreign key  (LbNum2)
       references Labels (LbNum),
    foreign key  (LbNum3)
       references Labels (LbNum),
    foreign key  (CaNum)
       references Categrs (CaNum),
    foreign key  (SPNum)
       references Spcials (SPNum),
    foreign key  (LCNum)
       references COLCats (LCNum),
    foreign key  (LPNum)
       references COLPTxs (LPNum),
    foreign key  (LXNum)
       references COLTxts (LXNum),
    foreign key  (DLXNum)
       references COLTxts (LXNum),
    foreign key  (LLID)
       references COLLsts (LLID),
    foreign key  (MaNum)
       references Mrquees (MaNum)
);

-- ============================================================
--   Index: Prods_PK                                           
-- ============================================================
create unique index Prods_PK on Prods (PrID asc);

-- ============================================================
--   Index: PrRepCl_FK                                         
-- ============================================================
create index PrRepCl_FK on Prods (ClNum asc);

-- ============================================================
--   Index: PrLnkTx_FK                                         
-- ============================================================
create index PrLnkTx_FK on Prods (TxNum asc);

-- ============================================================
--   Index: PrLnkSM_FK                                         
-- ============================================================
create index PrLnkSM_FK on Prods (SMNum asc);

-- ============================================================
--   Index: PrLnkNF_FK                                         
-- ============================================================
create index PrLnkNF_FK on Prods (NFNum asc);

-- ============================================================
--   Index: PrLnkNt_FK                                         
-- ============================================================
create index PrLnkNt_FK on Prods (NtNum asc);

-- ============================================================
--   Index: CaProds_FK                                         
-- ============================================================
create index CaProds_FK on Prods (CaNum asc);

-- ============================================================
--   Index: SPProds_FK                                         
-- ============================================================
create index SPProds_FK on Prods (SPNum asc);

-- ============================================================
--   Index: PrCOLCa_FK                                         
-- ============================================================
create index PrCOLCa_FK on Prods (LCNum asc);

-- ============================================================
--   Index: PrCOLPT_FK                                         
-- ============================================================
create index PrCOLPT_FK on Prods (LPNum asc);

-- ============================================================
--   Index: PrDCOTx_FK                                         
-- ============================================================
create index PrDCOTx_FK on Prods (DLXNum asc);

-- ============================================================
--   Index: PrCOLLs_FK                                         
-- ============================================================
create index PrCOLLs_FK on Prods (LLID asc);

-- ============================================================
--   Index: PrLnkMa_FK                                         
-- ============================================================
create index PrLnkMa_FK on Prods (MaNum asc);

-- ============================================================
--   Index: PrNum_AK                                           
-- ============================================================
create unique index PrNum_AK on Prods (PrNum asc);

-- ============================================================
--   Index: PrName1_IN                                         
-- ============================================================
create index PrName1_IN on Prods (PrCdNm1 asc);

-- ============================================================
--   Index: PrName2_IN                                         
-- ============================================================
create index PrName2_IN on Prods (PrCdNm2 asc);

-- ============================================================
--   Table: PrStats                                            
-- ============================================================
create table PrStats
(
    PrID        INTEGER               not null,
    PSLang      TEXT                  not null,
    PSHrNum     INTEGER               not null,
    PSTotSl     INTEGER                       ,
    PSTimSl     INTEGER                       ,
    PSTotPr     INTEGER                       ,
    PSCheck     INTEGER                       ,
    primary key (PrID, PSLang, PSHrNum),
    foreign key  (PrID)
       references Prods (PrID)
);

-- ============================================================
--   Index: PrStats_PK                                         
-- ============================================================
create unique index PrStats_PK on PrStats (PrID asc, PSLang asc, PSHrNum asc);

-- ============================================================
--   Index: PrStats_FK                                         
-- ============================================================
create index PrStats_FK on PrStats (PrID asc);

-- ============================================================
--   Index: PSMSlct_IN                                         
-- ============================================================
create index PSMSlct_IN on PrStats (PSTotSl desc);

-- ============================================================
--   Index: PSMPrnt_IN                                         
-- ============================================================
create index PSMPrnt_IN on PrStats (PSTotPr desc);

-- ============================================================
--   Table: Trans                                              
-- ============================================================
create table Trans
(
    TrScID      INTEGER               not null,
    TrID        INTEGER               not null,
    TTNum       INTEGER                       ,
    TrGrp       INTEGER                       ,
    TrDtTm      DATETIME                      ,
    TrVoid      INTEGER                       ,
    TrAPrdt     INTEGER                       ,
    TrRdClr     INTEGER                       ,
    TrNmPck     INTEGER                       ,
    TrSecLb     INTBOOLEAN                    ,
    TrCOOLF     INTBOOLEAN                    ,
    TrTtVal     INTEGER                       ,
    TrOTtVl     INTEGER                       ,
    TrPrNum     INTEGER                       ,
    TrClNum     INTEGER                       ,
    TrPrTyp     INTEGER                       ,
    TrPrPlT     INTEGER                       ,
    TrPrDes     TEXT                          ,
    TrPrMod     INTEGER                       ,
    TrBarPref   INTEGER                       ,
    TrBarNum    TEXT                          ,
    TrUnPr      INTEGER                       ,
    TrExPr      INTEGER                       ,
    TrFnlPr     INTEGER                       ,
    TrTare      INTEGER                       ,
    TrPrTare    INTEGER                       ,
    TrFnlTare   INTEGER                       ,
    TrByCount   INTEGER                       ,
    TrFnlNtWt   INTEGER                       ,
    TrFxWt      INTEGER                       ,
    TrSfLfD     INTEGER                       ,
    TrSfLfH     INTEGER                       ,
    TrSfLfM     INTEGER                       ,
    TrPrLfD     INTEGER                       ,
    TrPrLfH     INTEGER                       ,
    TrPrLfM     INTEGER                       ,
    TrLab1      INTEGER                       ,
    TrLab2      INTEGER                       ,
    TrLab3      INTEGER                       ,
    TrLab4      INTEGER                       ,
    TrOpNum     TEXT                          ,
    TrOpRFID    TEXT                          ,
    TrOpName    TEXT                          ,
    TrOpChg     INTEGER                       ,
    TrOBarDig   INTEGER                       ,
    TrOUnPr     INTEGER                       ,
    TrOFnPr     INTEGER                       ,
    TrOTare     INTEGER                       ,
    TrOByCount  INTEGER                       ,
    TrOFxWt     INTEGER                       ,
    TrOSfLfD    INTEGER                       ,
    TrOSfLfH    INTEGER                       ,
    TrOSfLfM    INTEGER                       ,
    TrOPrLfD    INTEGER                       ,
    TrOPrLfH    INTEGER                       ,
    TrOPrLfM    INTEGER                       ,
    TrOLab1     INTEGER                       ,
    TrOLab2     INTEGER                       ,
    TrOLab3     INTEGER                       ,
    TrOLab4     INTEGER                       ,
    TrGrNm1     TEXT                          ,
    TrGrNm2     TEXT                          ,
    TrGrNm3     TEXT                          ,
    TrGrNm4     TEXT                          ,
    TrCOTxt     TEXT                          ,
    TrCOTrTxt   TEXT                          ,
    TrOGrNm1    TEXT                          ,
    TrOGrNm2    TEXT                          ,
    TrOGrNm3    TEXT                          ,
    TrOGrNm4    TEXT                          ,
    TrUTNum     INTEGER                       ,
    TrCheck     INTEGER                       ,
    primary key (TrScID, TrID),
    foreign key  (TTNum)
       references TrTypes (TTNum)
);

-- ============================================================
--   Index: Trans_PK                                           
-- ============================================================
create unique index Trans_PK on Trans (TrScID asc, TrID asc);

-- ============================================================
--   Index: TrType_FK                                          
-- ============================================================
create index TrType_FK on Trans (TTNum asc);

-- ============================================================
--   Index: TrGrp_IN                                           
-- ============================================================
create index TrGrp_IN on Trans (TrGrp asc);

-- ============================================================
--   Index: TrID_IN                                            
-- ============================================================
create index TrID_IN on Trans (TrID asc);

-- ============================================================
--   Index: TrOpNum_IN                                         
-- ============================================================
create index TrOpNum_IN on Trans (TrOpNum asc);

-- ============================================================
--   Index: TrPrNum_IN                                         
-- ============================================================
create index TrPrNum_IN on Trans (TrPrNum asc);

-- ============================================================
--   Index: TrRdCl_IN                                          
-- ============================================================
create index TrRdCl_IN on Trans (TrRdClr asc);

-- ============================================================
--   Index: TrVoid_IN                                          
-- ============================================================
create index TrVoid_IN on Trans (TrVoid asc);

-- ============================================================
--   Index: TrDtTm_IN                                          
-- ============================================================
create index TrDtTm_IN on Trans (TrDtTm asc);

-- ============================================================
--   Table: NErrors                                            
-- ============================================================
create table NErrors
(
    NEID        INTEGER               not null,
    ScID        INTEGER                       ,
    NEECode     INTEGER                       ,
    NERecType   INTEGER                       ,
    NEDateTm    DATETIME                      ,
    NECheck     INTEGER                       ,
    primary key (NEID),
    foreign key  (ScID)
       references Scales (ScID)
);

-- ============================================================
--   Index: NErrors_PK                                         
-- ============================================================
create unique index NErrors_PK on NErrors (NEID asc);

-- ============================================================
--   Index: ScErrs_FK                                          
-- ============================================================
create index ScErrs_FK on NErrors (ScID asc);

-- ============================================================
--   Table: PwTests                                            
-- ============================================================
create table PwTests
(
    DvType      INTEGER               not null,
    DvSType     INTEGER               not null,
    PwMemChk1   INTEGER                       ,
    PwMemChk2   INTEGER                       ,
    PwCheck     INTEGER                       ,
    primary key (DvType, DvSType),
    foreign key  (DvType, DvSType)
       references Devices (DvType, DvSType)
);

-- ============================================================
--   Index: PwTests_PK                                         
-- ============================================================
create unique index PwTests_PK on PwTests (DvType asc, DvSType asc);

-- ============================================================
--   Table: YChangs                                            
-- ============================================================
create table YChangs
(
    YCID        INTEGER               not null,
    BaNum       INTEGER                       ,
    YCAppDv     INTEGER                       ,
    YCAppEv     INTEGER                       ,
    YCAppFm     INTEGER                       ,
    YCChange    TEXT                          ,
    YCCheck     INTEGER                       ,
    primary key (YCID),
    foreign key  (BaNum)
       references Batches (BaNum)
);

-- ============================================================
--   Index: YChangs_PK                                         
-- ============================================================
create unique index YChangs_PK on YChangs (YCID asc);

-- ============================================================
--   Index: BchChgs_FK                                         
-- ============================================================
create index BchChgs_FK on YChangs (BaNum asc);

-- ============================================================
--   Table: SftKeys                                            
-- ============================================================
create table SftKeys
(
    SGNum       INTEGER               not null,
    SKHPos      INTEGER               not null,
    SKVPos      INTEGER               not null,
    CaNum       INTEGER                       ,
    OpNum       TEXT                          ,
    SFNum       INTEGER                       ,
    SKHSize     INTEGER                       ,
    SKVSize     INTEGER                       ,
    SKBgClr     INTEGER                       ,
    SKTxClr     INTEGER                       ,
    SKName1     TEXT                          ,
    SKName2     TEXT                          ,
    SKFNBCl     INTBOOLEAN                    ,
    SKFNBIm     INTBOOLEAN                    ,
    SKFNTxt     INTBOOLEAN                    ,
    SKLkPPl     INTBOOLEAN                    ,
    SKActn      INTEGER                       ,
    SKActSb     INTEGER                       ,
    SKPrNum     INTEGER                       ,
    SKActPI     INTEGER                       ,
    SKActPS     TEXT                          ,
    SKBgImage   TEXT                          ,
    SKCheck     INTEGER                       ,
    primary key (SGNum, SKHPos, SKVPos),
    foreign key  (CaNum)
       references Categrs (CaNum),
    foreign key  (OpNum)
       references Opers (OpNum),
    foreign key  (SFNum)
       references SFeatrs (SFNum),
    foreign key  (SGNum)
       references SKGrps (SGNum)
);

-- ============================================================
--   Index: SftKeys_PK                                         
-- ============================================================
create unique index SftKeys_PK on SftKeys (SGNum asc, SKHPos asc, SKVPos asc);

-- ============================================================
--   Index: SGKeys_FK                                          
-- ============================================================
create index SGKeys_FK on SftKeys (SGNum asc);

-- ============================================================
--   Table: PrDStts                                            
-- ============================================================
create table PrDStts
(
    PrID        INTEGER               not null,
    PSLang      TEXT                  not null,
    PSHrNum     INTEGER               not null,
    PMType      INTEGER               not null,
    DSSlctd     INTEGER                       ,
    DSPrntd     INTEGER                       ,
    DSCheck     INTEGER                       ,
    primary key (PrID, PSLang, PSHrNum, PMType),
    foreign key  (PMType)
       references PMethds (PMType),
    foreign key  (PrID, PSLang, PSHrNum)
       references PrStats (PrID, PSLang, PSHrNum)
);

-- ============================================================
--   Index: PrDStts_PK                                         
-- ============================================================
create unique index PrDStts_PK on PrDStts (PrID asc, PSLang asc, PSHrNum asc, PMType asc);

-- ============================================================
--   Index: PrAccd_FK                                          
-- ============================================================
create index PrAccd_FK on PrDStts (PMType asc);

-- ============================================================
--   Index: PrSDets_FK                                         
-- ============================================================
create index PrSDets_FK on PrDStts (PrID asc, PSLang asc, PSHrNum asc);

-- ============================================================
--   Table: CaDStts                                            
-- ============================================================
create table CaDStts
(
    CaNum       INTEGER               not null,
    CSLang      TEXT                  not null,
    CSHrNum     INTEGER               not null,
    PMType      INTEGER               not null,
    CDSlctd     INTEGER                       ,
    CDCheck     INTEGER                       ,
    primary key (CaNum, CSLang, CSHrNum, PMType),
    foreign key  (PMType)
       references PMethds (PMType),
    foreign key  (CaNum, CSLang, CSHrNum)
       references CaStats (CaNum, CSLang, CSHrNum)
);

-- ============================================================
--   Index: CaDStts_PK                                         
-- ============================================================
create unique index CaDStts_PK on CaDStts (CaNum asc, CSLang asc, CSHrNum asc, PMType asc);

-- ============================================================
--   Index: CaAccd_FK                                          
-- ============================================================
create index CaAccd_FK on CaDStts (PMType asc);

-- ============================================================
--   Index: CaSDets_FK                                         
-- ============================================================
create index CaSDets_FK on CaDStts (CaNum asc, CSLang asc, CSHrNum asc);

-- ============================================================
--   Table: PMDStts                                            
-- ============================================================
create table PMDStts
(
    PMS_PMType  INTEGER               not null,
    MSLang      TEXT                  not null,
    MSHrNum     INTEGER               not null,
    PMType      INTEGER               not null,
    MDSlctd     INTEGER                       ,
    MDCheck     INTEGER                       ,
    primary key (PMS_PMType, MSLang, MSHrNum, PMType),
    foreign key  (PMType)
       references PMethds (PMType),
    foreign key  (PMS_PMType, MSLang, MSHrNum)
       references PMStats (PMType, MSLang, MSHrNum)
);

-- ============================================================
--   Index: PMDStts_PK                                         
-- ============================================================
create unique index PMDStts_PK on PMDStts (PMS_PMType asc, MSLang asc, MSHrNum asc, PMType asc);

-- ============================================================
--   Index: PMAccd_FK                                          
-- ============================================================
create index PMAccd_FK on PMDStts (PMType asc);

-- ============================================================
--   Index: PMSDets_FK                                         
-- ============================================================
create index PMSDets_FK on PMDStts (PMS_PMType asc, MSLang asc, MSHrNum asc);

-- ============================================================
--   Table: PCSeqs                                             
-- ============================================================
create table PCSeqs
(
    PrID        INTEGER               not null,
    PCLang      TEXT                  not null,
    PCSeq       TEXT                  not null,
    PCName      TEXT                          ,
    PCCheck     INTEGER                       ,
    primary key (PrID, PCLang, PCSeq),
    foreign key  (PrID)
       references Prods (PrID)
);

-- ============================================================
--   Index: PCSeqs_PK                                          
-- ============================================================
create unique index PCSeqs_PK on PCSeqs (PrID asc, PCLang asc, PCSeq asc);

-- ============================================================
--   Index: PrSeqs_FK                                          
-- ============================================================
create index PrSeqs_FK on PCSeqs (PrID asc);

-- ============================================================
--   Index: PCSqLan_IN                                         
-- ============================================================
create index PCSqLan_IN on PCSeqs (PCLang asc, PCSeq asc);

-- ============================================================
--   Table: COLCaCs                                            
-- ============================================================
create table COLCaCs
(
    LUID        INTEGER               not null,
    LCNum       INTEGER               not null,
    L2Check     INTEGER                       ,
    primary key (LUID, LCNum),
    foreign key  (LCNum)
       references COLCats (LCNum),
    foreign key  (LUID)
       references COLCtrs (LUID)
);

-- ============================================================
--   Index: COLCaCs_PK                                         
-- ============================================================
create unique index COLCaCs_PK on COLCaCs (LUID asc, LCNum asc);

-- ============================================================
--   Index: COLCaCs_FK                                         
-- ============================================================
create index COLCaCs_FK on COLCaCs (LCNum asc);

-- ============================================================
--   Index: COLCoCs_FK                                         
-- ============================================================
create index COLCoCs_FK on COLCaCs (LUID asc);

-- ============================================================
--   Table: COLCaSs                                            
-- ============================================================
create table COLCaSs
(
    LSID        INTEGER               not null,
    LCNum       INTEGER               not null,
    L1Check     INTEGER                       ,
    primary key (LSID, LCNum),
    foreign key  (LCNum)
       references COLCats (LCNum),
    foreign key  (LSID)
       references COLStps (LSID)
);

-- ============================================================
--   Index: COLCaSs_PK                                         
-- ============================================================
create unique index COLCaSs_PK on COLCaSs (LSID asc, LCNum asc);

-- ============================================================
--   Index: COLCaSs_FK                                         
-- ============================================================
create index COLCaSs_FK on COLCaSs (LCNum asc);

-- ============================================================
--   Index: COLStCs_FK                                         
-- ============================================================
create index COLStCs_FK on COLCaSs (LSID asc);

-- ============================================================
--   Table: COLLTxs                                            
-- ============================================================
create table COLLTxs
(
    LLID        INTEGER               not null,
    LXNum       INTEGER               not null,
    XLOrder     INTEGER               not null,
    XLLink      INTEGER                       ,
    XLNLink     INTEGER                       ,
    XLCheck     INTEGER                       ,
    primary key (LLID, LXNum),
    foreign key  (LXNum)
       references COLTxts (LXNum),
    foreign key  (LLID)
       references COLLsts (LLID)
);

-- ============================================================
--   Index: COLLTxs_PK                                         
-- ============================================================
create unique index COLLTxs_PK on COLLTxs (LLID asc, LXNum asc);

-- ============================================================
--   Index: COLLsTxMs_FK                                       
-- ============================================================
create index COLLsTxMs_FK on COLLTxs (LLID asc);

-- ============================================================
--   Index: COLLTxs_AK                                         
-- ============================================================
create unique index COLLTxs_AK on COLLTxs (LLID asc, XLOrder asc);

-- ============================================================
--   Index: XLCLNum_IN                                         
-- ============================================================
create index XLCLNum_IN on COLLTxs (XLLink asc);

-- ============================================================
--   Table: COLCaPs                                            
-- ============================================================
create table COLCaPs
(
    LPNum       INTEGER               not null,
    LCNum       INTEGER               not null,
    L3Check     INTEGER                       ,
    primary key (LPNum, LCNum),
    foreign key  (LCNum)
       references COLCats (LCNum),
    foreign key  (LPNum)
       references COLPTxs (LPNum)
);

-- ============================================================
--   Index: COLCaPs_PK                                         
-- ============================================================
create unique index COLCaPs_PK on COLCaPs (LPNum asc, LCNum asc);

-- ============================================================
--   Index: COLCaPs_FK                                         
-- ============================================================
create index COLCaPs_FK on COLCaPs (LCNum asc);

-- ============================================================
--   Index: COLPTCs_FK                                         
-- ============================================================
create index COLPTCs_FK on COLCaPs (LPNum asc);

-- ============================================================
--   Table: TrnOlds                                            
-- ============================================================
create table TrnOlds
(
    TrScID      INTEGER               not null,
    TrID        INTEGER               not null,
    ROBarDig    INTEGER                       ,
    ROUnPr      INTEGER                       ,
    ROFnPr      INTEGER                       ,
    ROTare      INTEGER                       ,
    ROByCount   INTEGER                       ,
    ROFxWt      INTEGER                       ,
    ROSfLfD     INTEGER                       ,
    ROSfLfH     INTEGER                       ,
    ROSfLfM     INTEGER                       ,
    ROPrLfD     INTEGER                       ,
    ROPrLfH     INTEGER                       ,
    ROPrLfM     INTEGER                       ,
    ROLab1      INTEGER                       ,
    ROLab2      INTEGER                       ,
    ROLab3      INTEGER                       ,
    ROLab4      INTEGER                       ,
    ROGrNm1     TEXT                          ,
    ROGrNm2     TEXT                          ,
    ROGrNm3     TEXT                          ,
    ROGrNm4     TEXT                          ,
    ROCheck     INTEGER                       ,
    primary key (TrScID, TrID),
    foreign key  (TrScID, TrID)
       references Trans (TrScID, TrID)
);

-- ============================================================
--   Index: TrnOlds_PK                                         
-- ============================================================
create unique index TrnOlds_PK on TrnOlds (TrScID asc, TrID asc);

