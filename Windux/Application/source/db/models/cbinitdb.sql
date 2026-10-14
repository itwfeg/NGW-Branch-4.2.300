-- ============================================================
--   Database name:  Database                                  
--   DBMS name:      Codebase 6.5                              
--   Created on:     9/4/2008  10:36 AM                        
-- ============================================================

-- ============================================================
--   Table: Depts                                              
-- ============================================================
create table Depts
(
    DpNum       NUMERIC(6)            not null,
    DpName      CHAR(40)                      ,
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
    LTNum       NUMERIC(6)            not null,
    LTText      CHAR(50)                      ,
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
    ONText      VARCHAR(2048)                 ,
    ONCheck     INTEGER                       
);

-- ============================================================
--   Table: ETotals                                            
-- ============================================================
create table ETotals
(
    ETECode     NUMERIC(6)            not null,
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
    WTPckCd     NUMERIC(3)            not null,
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
    CoName      CHAR(30)              not null,
    CoValue     CHAR(30)                      ,
    CoLVal      VARCHAR(500)                  ,
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
    RDRecType   CHAR(2)               not null,
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
    AIPrID      NUMERIC(8)            not null,
    AIOCID      NUMERIC(8)            not null,
    AIYCID      NUMERIC(8)            not null,
    AINEID      NUMERIC(8)            not null,
    AIScID      NUMERIC(8)            not null,
    AILOID      NUMERIC(8)            not null,
    AILSID      NUMERIC(8)            not null,
    AILUID      NUMERIC(8)            not null,
    AILLID      NUMERIC(8)            not null,
    AIELID      NUMERIC(8)            not null,
    AITRID      NUMERIC(8)            not null,
    AICheck     INTEGER                       
);

-- ============================================================
--   Table: PrDefs                                             
-- ============================================================
create table PrDefs
(
    PDType      NUMERIC(1)            not null
        check (
            PDType in (1,2,3,4)),
    PrPrice     NUMERIC(7)                    ,
    PrPrMod     NUMERIC(3)                    ,
    PrIsFrcPr   BOOLEAN                       ,
    PrExcPr     NUMERIC(7)                    ,
    PrDscMe     SMALLINT                      
        check (
            PrDscMe is null or (PrDscMe in (1,2,3))),
    PrDscPr1    NUMERIC(7)                    ,
    PrDscPr2    NUMERIC(7)                    ,
    PrDscPr3    NUMERIC(7)                    ,
    PrTare      NUMERIC(5)                    ,
    PrIsFrcTr   BOOLEAN                       ,
    PrPrpTare   NUMERIC(3)                    ,
    PrNetWt     NUMERIC(5)                    ,
    PrByCnt     NUMERIC(3)                    ,
    PrIsFrcBC   BOOLEAN                       ,
    PrShlfLfD   INTEGER                       ,
    PrShlfLfH   SMALLINT                      ,
    PrProdLfD   INTEGER                       ,
    PrProdLfH   SMALLINT                      ,
    PrIsPrnSL   BOOLEAN                       ,
    PrIsPrnPL   BOOLEAN                       ,
    PrIsPrnPO   BOOLEAN                       ,
    PrBnsPts    INTEGER                       ,
    PrIsUsSL    BOOLEAN                       ,
    PrInMode    SMALLINT                      
        check (
            PrInMode is null or (PrInMode in (1,2,3))),
    PrLblRot1   INTEGER                       ,
    PrLblPlc1   SMALLINT                      ,
    PrLblRot2   INTEGER                       ,
    PrLblPlc2   SMALLINT                      ,
    PrLblRot3   INTEGER                       ,
    PrLblPlc3   SMALLINT                      ,
    PrName1     CHAR(50)                      ,
    PrName2     CHAR(50)                      ,
    PrVCOLTxt   BOOLEAN                       ,
    PrCOLReq    BOOLEAN                       ,
    PrPrPlT     SMALLINT                      ,
    PDClNum     NUMERIC(6)                    ,
    PDTxNum     NUMERIC(6)                    ,
    PDSMNum     NUMERIC(6)                    ,
    PDNFNum     NUMERIC(6)                    ,
    PDMaNum     NUMERIC(6)                    ,
    PDNtNum     NUMERIC(6)                    ,
    PDCaNum     NUMERIC(6)                    ,
    PDLbNum1    NUMERIC(6)                    ,
    PDLbNum2    NUMERIC(6)                    ,
    PDLbNum3    NUMERIC(6)                    ,
    PDSPNum     NUMERIC(6)                    ,
    PDLCNum     NUMERIC(6)                    ,
    PDDXNum     NUMERIC(6)                    ,
    PDLPNum     NUMERIC(6)                    ,
    PDLLNum     NUMERIC(6)                    ,
    PDUPCNS     NUMERIC(1)                    ,
    PDEANFS     CHAR(12)                      ,
    PDEANPr     NUMERIC(2)                    ,
    PDDesc      VARCHAR(2048)                 ,
    PDGrNm1     VARCHAR(254)                  ,
    PDGrNm2     VARCHAR(254)                  ,
    PDGrNm3     VARCHAR(254)                  ,
    PDGrNm4     VARCHAR(254)                  ,
    PDImage     VARCHAR(254)                  ,
    PDCOLTN     VARCHAR(1000)                 ,
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
    RLbNum      NUMERIC(6)            not null,
    RLbBin      LBINARY(4096)                 ,
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
    FFName      CHAR(240)             not null,
    FFDate      DATETIME                      ,
    FFCmplt     BOOLEAN                       ,
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
    SGNum       NUMERIC(6)            not null,
    SGr_SGNum   NUMERIC(6)                    ,
    SGTime      NUMERIC(6)                    ,
    SGActT      NUMERIC(6)                    ,
    SGActP      VARCHAR(254)                  ,
    SGImage     VARCHAR(254)                  ,
    SGCheck     INTEGER                       ,
    primary key (SGNum)
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
    EANum       NUMERIC(3)            not null,
    EAFail      BOOLEAN                       ,
    EAResol     BOOLEAN                       ,
    EADelay     INTEGER                       ,
    EAFSent     BOOLEAN                       ,
    EASubj1     CHAR(254)                     ,
    EASubj2     CHAR(254)                     ,
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
    IWLang      CHAR(2)               not null,
    IWName      CHAR(20)              not null,
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
    WENum       NUMERIC(8)            not null,
    WECode      NUMERIC(3)            not null,
    WEDtTm      DATETIME              not null,
    WEPLen      NUMERIC(3)                    ,
    WEPWdth     NUMERIC(3)                    ,
    WEPHght     NUMERIC(3)                    ,
    WELHght     NUMERIC(3)                    ,
    WETrType    CHAR(20)                      ,
    WETrPLU     NUMERIC(6)                    ,
    WEFlmSl     NUMERIC(1)                    ,
    WECheck     INTEGER                       ,
    primary key (WENum)
);

-- ============================================================
--   Index: WErrors_PK                                         
-- ============================================================
create unique index WErrors_PK on WErrors (WENum asc);

-- ============================================================
--   Table: ELabels                                            
-- ============================================================
create table ELabels
(
    ELID        NUMERIC(8)            not null,
    ELPrNum     NUMERIC(6)                    ,
    ELPrDesc    CHAR(120)                     ,
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
--   Table: Classes                                            
-- ============================================================
create table Classes
(
    ClNum       NUMERIC(6)            not null,
    Cla_ClNum   NUMERIC(6)                    ,
    ClDesc1     CHAR(80)                      ,
    ClDesc2     CHAR(80)                      ,
    ClCheck     INTEGER                       ,
    primary key (ClNum)
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
    TxNum       NUMERIC(6)            not null,
    TxDesc      VARCHAR(2048)                 ,
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
    SMNum       NUMERIC(6)            not null,
    SMDesc      VARCHAR(2048)                 ,
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
    LbNum       NUMERIC(6)            not null,
    LbBin       LBINARY(4096)                 ,
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
    OpNum       CHAR(16)              not null,
    OpRFID      CHAR(16)                      ,
    OpAccLev    SMALLINT                      ,
    OpPLang     SMALLINT                      ,
    OpSawNote   BOOLEAN                       ,
    OpName      VARCHAR(299)                  ,
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
    NFNum       NUMERIC(6)            not null,
    NFCol1      VARCHAR(2048)                 ,
    NFCol2      VARCHAR(2048)                 ,
    NFCol3      VARCHAR(2048)                 ,
    NFCol4      VARCHAR(2048)                 ,
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
    NtNum       NUMERIC(6)            not null,
    NtText      VARCHAR(2048)                 ,
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
    ScID        NUMERIC(8)            not null,
    ScHstNm     CHAR(63)                      ,
    ScIPAddr    CHAR(24)                      ,
    ScType      NUMERIC(1)            not null,
    ScCreatDT   DATETIME                      ,
    ScState     SMALLINT                      ,
    ScStateDT   DATETIME                      ,
    ScRegis     BOOLEAN                       ,
    ScDReg      BOOLEAN                       ,
    ScNBatch    BOOLEAN                       ,
    ScNBtchDT   DATETIME                      ,
    ScNOnLn     BOOLEAN                       ,
    ScNOnLnDT   DATETIME                      ,
    ScRetCt     INTEGER                       ,
    ScVer       NUMERIC(8)                    ,
    ScVerRec    NUMERIC(8)                    ,
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
    DvType      NUMERIC(3)            not null,
    DvSType     NUMERIC(3)            not null,
    DvKind      NUMERIC(3)                    ,
    DvSrlNum    CHAR(40)                      ,
    DvHWVer     CHAR(40)                      ,
    DvSWVer     CHAR(40)                      ,
    DvChkPr     BOOLEAN                       ,
    DvInstDt    DATETIME                      ,
    DvSrvHst    VARCHAR(1000)                 ,
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
    CaNum       NUMERIC(6)            not null,
    Cat_CaNum   NUMERIC(6)                    ,
    CaName1     CHAR(50)                      ,
    CaName2     CHAR(50)                      ,
    CaCdNm1     CHAR(40)                      ,
    CaCdNm2     CHAR(40)                      ,
    CaBgClr     INTEGER                       ,
    CaTxClr     INTEGER                       ,
    CaImage     VARCHAR(254)                  ,
    CaCheck     INTEGER                       ,
    primary key (CaNum)
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
--   Index: CaName2_IN                                         
-- ============================================================
create index CaName2_IN on Categrs (CaCdNm2 asc);

-- ============================================================
--   Index: CaName1_IN                                         
-- ============================================================
create index CaName1_IN on Categrs (CaCdNm1 asc);

-- ============================================================
--   Table: PMethds                                            
-- ============================================================
create table PMethds
(
    PMType      NUMERIC(3)            not null,
    PMName1     CHAR(50)                      ,
    PMName2     CHAR(50)                      ,
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
    SPNum       NUMERIC(6)            not null,
    SPName1     CHAR(50)                      ,
    SPName2     CHAR(50)                      ,
    SPDefL      INTEGER                       ,
    SPImag1     VARCHAR(254)                  ,
    SPImag2     VARCHAR(254)                  ,
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
    SSNum       NUMERIC(6)            not null,
    SSDTextL    SMALLINT                      ,
    SSDTextC    INTEGER                       ,
    SSDGrTxtC   INTEGER                       ,
    SSDFImgL    SMALLINT                      ,
    SSDBgClr    INTEGER                       ,
    SSDBgImg    VARCHAR(254)                  ,
    SSImgDir    VARCHAR(254)                  ,
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
    LXNum       NUMERIC(6)            not null,
    LXDesc      VARCHAR(2000)                 ,
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
    LCNum       NUMERIC(6)            not null,
    LCName1     CHAR(50)                      ,
    LCName2     CHAR(50)                      ,
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
    LPNum       NUMERIC(6)            not null,
    LPName1     CHAR(100)                     ,
    LPName2     CHAR(100)                     ,
    LPAllCa     BOOLEAN                       ,
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
    LUID        NUMERIC(8)            not null,
    LUName1     CHAR(100)             not null,
    LUName2     CHAR(100)                     ,
    LUAllCa     BOOLEAN                       ,
    LUKey       NUMERIC(6)                    ,
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
    LSID        NUMERIC(8)            not null,
    LSName1     CHAR(100)             not null,
    LSName2     CHAR(100)                     ,
    LSAllCa     BOOLEAN                       ,
    LSKey       NUMERIC(6)                    ,
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
    LLID        NUMERIC(8)            not null,
    LLNum       NUMERIC(6)                    ,
    LLFLink     NUMERIC(7)                    ,
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
    SGNum       NUMERIC(6)            not null,
    SGHGrid     NUMERIC(2)                    ,
    SGVGrid     NUMERIC(2)                    ,
    SGHSpc      NUMERIC(4)                    ,
    SGVSpc      NUMERIC(4)                    ,
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
    MaNum       NUMERIC(6)            not null,
    MaDesc1     CHAR(120)                     ,
    MaDesc2     CHAR(120)                     ,
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
    TTNum       NUMERIC(4)            not null,
    TTName1     CHAR(99)                      ,
    TTName2     CHAR(99)                      ,
    TTAffPr     SMALLINT                      ,
    TTBkDt      BOOLEAN                       ,
    TTAutoUPC   BOOLEAN                       ,
    TTManUPC    BOOLEAN                       ,
    TTUPCDigit  CHAR(1)                       ,
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
    BaNum       NUMERIC(8)            not null,
    ScID        NUMERIC(8)                    ,
    BaVer       NUMERIC(8)                    ,
    BaRVer      NUMERIC(8)                    ,
    BaOrigin    SMALLINT                      ,
    BaType      SMALLINT                      ,
    BaCreaDT    DATETIME                      ,
    BaComp      BOOLEAN                       ,
    BaCompDT    DATETIME                      ,
    BaSig       CHAR(8)                       ,
    BaPAppl     BOOLEAN                       ,
    BaApplDT    DATETIME                      ,
    BaPSync     BOOLEAN                       ,
    BaSyncDT    DATETIME                      ,
    BaPDelt     BOOLEAN                       ,
    BaDeltDT    DATETIME                      ,
    BaCheck     INTEGER                       ,
    primary key (BaNum)
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
    CaNum       NUMERIC(6)            not null,
    CSLang      CHAR(2)               not null,
    CSHrNum     NUMERIC(3)            not null,
    CSTotSl     INTEGER                       ,
    CSCheck     INTEGER                       ,
    primary key (CaNum, CSLang, CSHrNum)
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
    PMType      NUMERIC(3)            not null,
    MSLang      CHAR(2)               not null,
    MSHrNum     NUMERIC(3)            not null,
    MSTtMSl     INTEGER                       ,
    MSTtCSl     INTEGER                       ,
    MSTtPSl     INTEGER                       ,
    MSTtLPr     INTEGER                       ,
    MSCheck     INTEGER                       ,
    primary key (PMType, MSLang, MSHrNum)
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
    SFNum       NUMERIC(6)            not null,
    SSNum       NUMERIC(6)                    ,
    SGNum       NUMERIC(6)                    ,
    SFDHGrid    NUMERIC(2)                    ,
    SFDVGrid    NUMERIC(2)                    ,
    SFDHSpc     NUMERIC(4)                    ,
    SFDVSpc     NUMERIC(4)                    ,
    SFCheck     INTEGER                       ,
    primary key (SFNum)
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
    PrID        NUMERIC(8)            not null,
    ClNum       NUMERIC(6)                    ,
    TxNum       NUMERIC(6)                    ,
    SMNum       NUMERIC(6)                    ,
    NFNum       NUMERIC(6)                    ,
    NtNum       NUMERIC(6)                    ,
    LbNum1      NUMERIC(6)                    ,
    LbNum2      NUMERIC(6)                    ,
    LbNum3      NUMERIC(6)                    ,
    CaNum       NUMERIC(6)                    ,
    SPNum       NUMERIC(6)                    ,
    LCNum       NUMERIC(6)                    ,
    LPNum       NUMERIC(6)                    ,
    LXNum       NUMERIC(6)                    ,
    DLXNum      NUMERIC(6)                    ,
    LLID        NUMERIC(8)                    ,
    MaNum       NUMERIC(6)                    ,
    PrPrice     NUMERIC(7)                    ,
    PrPrMod     NUMERIC(3)                    ,
    PrIsFrcPr   BOOLEAN                       ,
    PrExcPr     NUMERIC(7)                    ,
    PrDscMe     SMALLINT                      
        check (
            PrDscMe is null or (PrDscMe in (1,2,3))),
    PrDscPr1    NUMERIC(7)                    ,
    PrDscPr2    NUMERIC(7)                    ,
    PrDscPr3    NUMERIC(7)                    ,
    PrTare      NUMERIC(5)                    ,
    PrIsFrcTr   BOOLEAN                       ,
    PrPrpTare   NUMERIC(3)                    ,
    PrNetWt     NUMERIC(5)                    ,
    PrByCnt     NUMERIC(3)                    ,
    PrIsFrcBC   BOOLEAN                       ,
    PrShlfLfD   INTEGER                       ,
    PrShlfLfH   SMALLINT                      ,
    PrProdLfD   INTEGER                       ,
    PrProdLfH   SMALLINT                      ,
    PrIsPrnSL   BOOLEAN                       ,
    PrIsPrnPL   BOOLEAN                       ,
    PrIsPrnPO   BOOLEAN                       ,
    PrBnsPts    INTEGER                       ,
    PrIsUsSL    BOOLEAN                       ,
    PrInMode    SMALLINT                      
        check (
            PrInMode is null or (PrInMode in (1,2,3))),
    PrLblRot1   INTEGER                       ,
    PrLblPlc1   SMALLINT                      ,
    PrLblRot2   INTEGER                       ,
    PrLblPlc2   SMALLINT                      ,
    PrLblRot3   INTEGER                       ,
    PrLblPlc3   SMALLINT                      ,
    PrName1     CHAR(50)                      ,
    PrName2     CHAR(50)                      ,
    PrVCOLTxt   BOOLEAN                       ,
    PrCOLReq    BOOLEAN                       ,
    PrPrPlT     SMALLINT                      ,
    PrNum       NUMERIC(6)            not null,
    PrType      SMALLINT                      
        check (
            PrType is null or (PrType in (1,2,3,4))),
    PrBarPref   NUMERIC(2)                    ,
    PrBarProN   NUMERIC(6)                    ,
    PrBarVenN   NUMERIC(6)                    ,
    PrEANCd     NUMERIC(9)                    ,
    PrRawBarC   CHAR(40)                      ,
    PrCdNm1     CHAR(40)                      ,
    PrCdNm2     CHAR(40)                      ,
    PrSpcSD     DATETIME                      ,
    PrDesc      VARCHAR(2048)                 ,
    PrGrNm1     VARCHAR(254)                  ,
    PrGrNm2     VARCHAR(254)                  ,
    PrGrNm3     VARCHAR(254)                  ,
    PrGrNm4     VARCHAR(254)                  ,
    PrImage     VARCHAR(254)                  ,
    PrCOLTN     VARCHAR(1000)                 ,
    PrCheck     INTEGER                       ,
    primary key (PrID)
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
--   Index: PrName1_IN                                         
-- ============================================================
create index PrName1_IN on Prods (PrCdNm1 asc);

-- ============================================================
--   Index: PrName2_IN                                         
-- ============================================================
create index PrName2_IN on Prods (PrCdNm2 asc);

-- ============================================================
--   Index: PrNum_AK                                           
-- ============================================================
create unique index PrNum_AK on Prods (PrNum asc);

-- ============================================================
--   Table: PrStats                                            
-- ============================================================
create table PrStats
(
    PrID        NUMERIC(8)            not null,
    PSLang      CHAR(2)               not null,
    PSHrNum     NUMERIC(3)            not null,
    PSTotSl     INTEGER                       ,
    PSTimSl     INTEGER                       ,
    PSTotPr     INTEGER                       ,
    PSCheck     INTEGER                       ,
    primary key (PrID, PSLang, PSHrNum)
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
--   Index: PSMPrnt_IN                                         
-- ============================================================
create index PSMPrnt_IN on PrStats (PSTotPr desc);

-- ============================================================
--   Index: PSMSlct_IN                                         
-- ============================================================
create index PSMSlct_IN on PrStats (PSTotSl desc);

-- ============================================================
--   Table: Trans                                              
-- ============================================================
create table Trans
(
    TrScID      NUMERIC(8)            not null,
    TrID        NUMERIC(8)            not null,
    TTNum       NUMERIC(4)                    ,
    TrGrp       NUMERIC(8)                    ,
    TrDtTm      DATETIME                      ,
    TrVoid      NUMERIC(1)                    ,
    TrAPrdt     NUMERIC(2)                    ,
    TrRdClr     NUMERIC(1)                    ,
    TrNmPck     NUMERIC(4)                    ,
    TrSecLb     BOOLEAN                       ,
    TrCOOLF     BOOLEAN                       ,
    TrTtVal     INTEGER                       ,
    TrOTtVl     INTEGER                       ,
    TrPrNum     NUMERIC(6)                    ,
    TrClNum     NUMERIC(6)                    ,
    TrPrTyp     SMALLINT                      ,
    TrPrPlT     SMALLINT                      ,
    TrPrDes     CHAR(100)                     ,
    TrPrMod     NUMERIC(3)                    ,
    TrBarPref   NUMERIC(2)                    ,
    TrBarNum    CHAR(40)                      ,
    TrUnPr      NUMERIC(7)                    ,
    TrExPr      NUMERIC(7)                    ,
    TrFnlPr     NUMERIC(7)                    ,
    TrTare      NUMERIC(5)                    ,
    TrPrTare    NUMERIC(3)                    ,
    TrFnlTare   NUMERIC(5)                    ,
    TrByCount   NUMERIC(3)                    ,
    TrFnlNtWt   NUMERIC(6)                    ,
    TrFxWt      NUMERIC(5)                    ,
    TrSfLfD     INTEGER                       ,
    TrSfLfH     SMALLINT                      ,
    TrSfLfM     SMALLINT                      ,
    TrPrLfD     INTEGER                       ,
    TrPrLfH     SMALLINT                      ,
    TrPrLfM     SMALLINT                      ,
    TrLab1      NUMERIC(6)                    ,
    TrLab2      NUMERIC(6)                    ,
    TrLab3      NUMERIC(6)                    ,
    TrLab4      NUMERIC(6)                    ,
    TrOpNum     CHAR(16)                      ,
    TrOpRFID    CHAR(16)                      ,
    TrOpName    CHAR(40)                      ,
    TrOpChg     INTEGER                       ,
    TrOBarDig   NUMERIC(1)                    ,
    TrOUnPr     NUMERIC(7)                    ,
    TrOFnPr     NUMERIC(7)                    ,
    TrOTare     NUMERIC(5)                    ,
    TrOByCount  NUMERIC(3)                    ,
    TrOFxWt     NUMERIC(5)                    ,
    TrOSfLfD    INTEGER                       ,
    TrOSfLfH    SMALLINT                      ,
    TrOSfLfM    SMALLINT                      ,
    TrOPrLfD    INTEGER                       ,
    TrOPrLfH    SMALLINT                      ,
    TrOPrLfM    SMALLINT                      ,
    TrOLab1     NUMERIC(6)                    ,
    TrOLab2     NUMERIC(6)                    ,
    TrOLab3     NUMERIC(6)                    ,
    TrOLab4     NUMERIC(6)                    ,
    TrGrNm1     VARCHAR(254)                  ,
    TrGrNm2     VARCHAR(254)                  ,
    TrGrNm3     VARCHAR(254)                  ,
    TrGrNm4     VARCHAR(254)                  ,
    TrCOTxt     VARCHAR(2000)                 ,
    TrCOTrTxt   VARCHAR(1000)                 ,
    TrOGrNm1    VARCHAR(254)                  ,
    TrOGrNm2    VARCHAR(254)                  ,
    TrOGrNm3    VARCHAR(254)                  ,
    TrOGrNm4    VARCHAR(254)                  ,
    TrCheck     INTEGER                       ,
    primary key (TrScID, TrID)
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
    NEID        NUMERIC(8)            not null,
    ScID        NUMERIC(8)                    ,
    NEECode     NUMERIC(6)                    ,
    NERecType   SMALLINT                      ,
    NEDateTm    DATETIME                      ,
    NECheck     INTEGER                       ,
    primary key (NEID)
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
    DvType      NUMERIC(3)            not null,
    DvSType     NUMERIC(3)            not null,
    PwMemChk1   INTEGER                       ,
    PwMemChk2   INTEGER                       ,
    PwCheck     INTEGER                       ,
    primary key (DvType, DvSType)
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
    YCID        NUMERIC(8)            not null,
    BaNum       NUMERIC(8)                    ,
    YCAppDv     INTEGER                       ,
    YCAppEv     INTEGER                       ,
    YCAppFm     INTEGER                       ,
    YCChange    VARCHAR(2000)                 ,
    YCCheck     INTEGER                       ,
    primary key (YCID)
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
    SGNum       NUMERIC(6)            not null,
    SKHPos      NUMERIC(2)            not null,
    SKVPos      NUMERIC(2)            not null,
    CaNum       NUMERIC(6)                    ,
    OpNum       CHAR(16)                      ,
    SFNum       NUMERIC(6)                    ,
    SKHSize     NUMERIC(2)                    ,
    SKVSize     NUMERIC(2)                    ,
    SKBgClr     INTEGER                       ,
    SKTxClr     INTEGER                       ,
    SKName1     CHAR(80)                      ,
    SKName2     CHAR(80)                      ,
    SKFNBCl     BOOLEAN                       ,
    SKFNBIm     BOOLEAN                       ,
    SKFNTxt     BOOLEAN                       ,
    SKLkPPl     BOOLEAN                       ,
    SKActn      SMALLINT                      ,
    SKActSb     SMALLINT                      ,
    SKPrNum     NUMERIC(6)                    ,
    SKActPI     INTEGER                       ,
    SKActPS     VARCHAR(254)                  ,
    SKBgImage   VARCHAR(254)                  ,
    SKCheck     INTEGER                       ,
    primary key (SGNum, SKHPos, SKVPos)
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
    PrID        NUMERIC(8)            not null,
    PSLang      CHAR(2)               not null,
    PSHrNum     NUMERIC(3)            not null,
    PMType      NUMERIC(3)            not null,
    DSSlctd     INTEGER                       ,
    DSPrntd     INTEGER                       ,
    DSCheck     INTEGER                       ,
    primary key (PrID, PSLang, PSHrNum, PMType)
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
    CaNum       NUMERIC(6)            not null,
    CSLang      CHAR(2)               not null,
    CSHrNum     NUMERIC(3)            not null,
    PMType      NUMERIC(3)            not null,
    CDSlctd     INTEGER                       ,
    CDCheck     INTEGER                       ,
    primary key (CaNum, CSLang, CSHrNum, PMType)
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
    PMS_PMType  NUMERIC(3)            not null,
    MSLang      CHAR(2)               not null,
    MSHrNum     NUMERIC(3)            not null,
    PMType      NUMERIC(3)            not null,
    MDSlctd     INTEGER                       ,
    MDCheck     INTEGER                       ,
    primary key (PMS_PMType, MSLang, MSHrNum, PMType)
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
    PrID        NUMERIC(8)            not null,
    PCLang      CHAR(5)               not null,
    PCSeq       BINCHAR(8)            not null,
    PCName      CHAR(40)                      ,
    PCCheck     INTEGER                       ,
    primary key (PrID, PCLang, PCSeq)
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
    LUID        NUMERIC(8)            not null,
    LCNum       NUMERIC(6)            not null,
    L2Check     INTEGER                       ,
    primary key (LUID, LCNum)
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
    LSID        NUMERIC(8)            not null,
    LCNum       NUMERIC(6)            not null,
    L1Check     INTEGER                       ,
    primary key (LSID, LCNum)
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
    LLID        NUMERIC(8)            not null,
    LXNum       NUMERIC(6)            not null,
    XLOrder     NUMERIC(3)            not null,
    XLLink      NUMERIC(7)                    ,
    XLNLink     NUMERIC(7)                    ,
    XLCheck     INTEGER                       ,
    primary key (LLID, LXNum)
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
--   Index: XLCLNum_IN                                         
-- ============================================================
create index XLCLNum_IN on COLLTxs (XLLink asc);

-- ============================================================
--   Index: COLLTxs_AK                                         
-- ============================================================
create unique index COLLTxs_AK on COLLTxs (LLID asc, XLOrder asc);

-- ============================================================
--   Table: COLCaPs                                            
-- ============================================================
create table COLCaPs
(
    LPNum       NUMERIC(6)            not null,
    LCNum       NUMERIC(6)            not null,
    L3Check     INTEGER                       ,
    primary key (LPNum, LCNum)
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
    TrScID      NUMERIC(8)            not null,
    TrID        NUMERIC(8)            not null,
    ROBarDig    NUMERIC(2)                    ,
    ROUnPr      NUMERIC(5)                    ,
    ROFnPr      NUMERIC(5)                    ,
    ROTare      NUMERIC(4)                    ,
    ROByCount   NUMERIC(2)                    ,
    ROFxWt      NUMERIC(5)                    ,
    ROSfLfD     NUMERIC(3)                    ,
    ROSfLfH     NUMERIC(2)                    ,
    ROSfLfM     NUMERIC(2)                    ,
    ROPrLfD     NUMERIC(3)                    ,
    ROPrLfH     NUMERIC(2)                    ,
    ROPrLfM     NUMERIC(2)                    ,
    ROLab1      NUMERIC(4)                    ,
    ROLab2      NUMERIC(4)                    ,
    ROLab3      NUMERIC(4)                    ,
    ROLab4      NUMERIC(4)                    ,
    ROGrNm1     VARCHAR(254)                  ,
    ROGrNm2     VARCHAR(254)                  ,
    ROGrNm3     VARCHAR(254)                  ,
    ROGrNm4     VARCHAR(254)                  ,
    ROCheck     INTEGER                       ,
    primary key (TrScID, TrID)
);

-- ============================================================
--   Index: TrnOlds_PK                                         
-- ============================================================
create unique index TrnOlds_PK on TrnOlds (TrScID asc, TrID asc);

alter table SGraphs
    add foreign key  (SGr_SGNum)
       references SGraphs (SGNum);

alter table Classes
    add foreign key  (Cla_ClNum)
       references Classes (ClNum);

alter table Categrs
    add foreign key  (Cat_CaNum)
       references Categrs (CaNum);

alter table Batches
    add foreign key  (ScID)
       references Scales (ScID);

alter table CaStats
    add foreign key  (CaNum)
       references Categrs (CaNum);

alter table PMStats
    add foreign key  (PMType)
       references PMethds (PMType);

alter table SFeatrs
    add foreign key  (SSNum)
       references SStyles (SSNum);

alter table SFeatrs
    add foreign key  (SGNum)
       references SKGrps (SGNum);

alter table Prods
    add foreign key  (ClNum)
       references Classes (ClNum);

alter table Prods
    add foreign key  (TxNum)
       references Texts (TxNum);

alter table Prods
    add foreign key  (SMNum)
       references SMsgs (SMNum);

alter table Prods
    add foreign key  (NFNum)
       references NutrFs (NFNum);

alter table Prods
    add foreign key  (NtNum)
       references Notes (NtNum);

alter table Prods
    add foreign key  (LbNum1)
       references Labels (LbNum);

alter table Prods
    add foreign key  (LbNum2)
       references Labels (LbNum);

alter table Prods
    add foreign key  (LbNum3)
       references Labels (LbNum);

alter table Prods
    add foreign key  (CaNum)
       references Categrs (CaNum);

alter table Prods
    add foreign key  (SPNum)
       references Spcials (SPNum);

alter table Prods
    add foreign key  (LCNum)
       references COLCats (LCNum);

alter table Prods
    add foreign key  (LPNum)
       references COLPTxs (LPNum);

alter table Prods
    add foreign key  (LXNum)
       references COLTxts (LXNum);

alter table Prods
    add foreign key  (DLXNum)
       references COLTxts (LXNum);

alter table Prods
    add foreign key  (LLID)
       references COLLsts (LLID);

alter table Prods
    add foreign key  (MaNum)
       references Mrquees (MaNum);

alter table PrStats
    add foreign key  (PrID)
       references Prods (PrID);

alter table Trans
    add foreign key  (TTNum)
       references TrTypes (TTNum);

alter table NErrors
    add foreign key  (ScID)
       references Scales (ScID);

alter table PwTests
    add foreign key  (DvType, DvSType)
       references Devices (DvType, DvSType);

alter table YChangs
    add foreign key  (BaNum)
       references Batches (BaNum);

alter table SftKeys
    add foreign key  (CaNum)
       references Categrs (CaNum);

alter table SftKeys
    add foreign key  (OpNum)
       references Opers (OpNum);

alter table SftKeys
    add foreign key  (SFNum)
       references SFeatrs (SFNum);

alter table SftKeys
    add foreign key  (SGNum)
       references SKGrps (SGNum);

alter table PrDStts
    add foreign key  (PMType)
       references PMethds (PMType);

alter table PrDStts
    add foreign key  (PrID, PSLang, PSHrNum)
       references PrStats (PrID, PSLang, PSHrNum);

alter table CaDStts
    add foreign key  (PMType)
       references PMethds (PMType);

alter table CaDStts
    add foreign key  (CaNum, CSLang, CSHrNum)
       references CaStats (CaNum, CSLang, CSHrNum);

alter table PMDStts
    add foreign key  (PMType)
       references PMethds (PMType);

alter table PMDStts
    add foreign key  (PMS_PMType, MSLang, MSHrNum)
       references PMStats (PMType, MSLang, MSHrNum);

alter table PCSeqs
    add foreign key  (PrID)
       references Prods (PrID);

alter table COLCaCs
    add foreign key  (LCNum)
       references COLCats (LCNum);

alter table COLCaCs
    add foreign key  (LUID)
       references COLCtrs (LUID);

alter table COLCaSs
    add foreign key  (LCNum)
       references COLCats (LCNum);

alter table COLCaSs
    add foreign key  (LSID)
       references COLStps (LSID);

alter table COLLTxs
    add foreign key  (LXNum)
       references COLTxts (LXNum);

alter table COLLTxs
    add foreign key  (LLID)
       references COLLsts (LLID);

alter table COLCaPs
    add foreign key  (LCNum)
       references COLCats (LCNum);

alter table COLCaPs
    add foreign key  (LPNum)
       references COLPTxs (LPNum);

alter table TrnOlds
    add foreign key  (TrScID, TrID)
       references Trans (TrScID, TrID);

