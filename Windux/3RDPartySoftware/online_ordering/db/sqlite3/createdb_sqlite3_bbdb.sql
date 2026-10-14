-- ============================================================
--   Database name:  Database                                  
--   DBMS name:      Sqlite 3                                  
--   Created on:     03/06/2021  12:52 PM                       
-- ============================================================

-- ============================================================
--   Table: Depts                                              
-- ============================================================
create table Depts
(
    DeptNum       INTEGER               not null,
    DeptName1     TEXT                          ,
    DeptName2     TEXT                          ,
    DeptCheck     INTEGER                       ,
    primary key (DeptNum)
);

-- ============================================================
--   Index: Depts_PK                                           
-- ============================================================
create unique index Depts_PK on Depts (DeptNum asc);


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
--   Table: PrDefs                                             
-- ============================================================
create table PrDefs
(
    DeptNum     INTEGER               not null,
    PDType      INTEGER               not null
        check (
            PDType in (1,2,3,4)),
    PrPrice     INTEGER                       ,
    PrPrMod     INTEGER                       ,
    PrIsFrcPr   INTBOOLEAN                    ,
    PrExcPr     INTEGER                       ,
    PrDscMe     INTEGER                       ,
    PrDscPr1    INTEGER                       ,
    PrDscPr2    INTEGER                       ,
    PrDscPr3    INTEGER                       ,
    PrTare      INTEGER                       ,
    PrTareB     INTEGER                       ,
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
    PrInMode    INTEGER                       ,
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
    PDEAN128FS  TEXT                          ,
    PDGS1GTINFS TEXT                          ,
    PDGS1AIFS   TEXT                          ,
    PDQRCODEFS  TEXT                          ,
    PDDMATRXFS  TEXT                          ,
    PDEANPr     INTEGER                       ,
    PDDesc      TEXT                          ,
    PDGrNm1     TEXT                          ,
    PDGrNm2     TEXT                          ,
    PDGrNm3     TEXT                          ,
    PDGrNm4     TEXT                          ,
    PDImage     TEXT                          ,
    PDCOLTN     TEXT                          ,
    PrMealSideItemsGroupNum   INTEGER         ,
    PrMealSideItemsAreFree    INTBOOLEAN      ,
    PrMealNumSidesIncluded    INTEGER         ,
    PrCutTestMeatSpecSheetGroupNum  INTEGER   ,
    PrWeightMin               INTEGER         ,
    PrWeightMax               INTEGER         ,
    PDTotalPriceMin           INTEGER         ,
    PDTotalPriceMax           INTEGER         ,
    PDManualLabelTypeNum1     INTEGER         ,
    PDManualLabelTypeNum2     INTEGER         ,
    PDManualLabelTypeNum3     INTEGER         ,
    PDPrepackLabelTypeNum1    INTEGER         ,
    PDPrepackLabelTypeNum2    INTEGER         ,
    PDPrepackLabelTypeNum3    INTEGER         ,
    PDProdEntryLabelTypeNum1  INTEGER         ,
    PDProdEntryLabelTypeNum2  INTEGER         ,
    PDProdEntryLabelTypeNum3  INTEGER         ,
    PDSelfServLabelTypeNum1   INTEGER         ,
    PDSelfServLabelTypeNum2   INTEGER         ,
    PDSelfServLabelTypeNum3   INTEGER         ,
    PDPick5LabelTypeNum1      INTEGER         ,
    PDPick5LabelTypeNum2      INTEGER         ,
    PDPick5LabelTypeNum3      INTEGER         ,
    PDCheck     INTEGER                       ,
    primary key (DeptNum, PDType),
    foreign key (DeptNum)
       references Depts (DeptNum)
);

-- ============================================================
--   Index: PrDefs_PK                                          
-- ============================================================
create unique index PrDefs_PK on PrDefs (DeptNum asc, PDType asc);


-- ============================================================
--   Table: Categrs                                            
-- ============================================================
create table Categrs
(
    DeptNum     INTEGER               not null,
    CaNum       INTEGER               not null,
    Cat_CaNum   INTEGER                       ,
    CaName1     TEXT                          ,
    CaName2     TEXT                          ,
    CaCdNm1     TEXT                          ,
    CaCdNm2     TEXT                          ,
    CaBgClr     INTEGER                       ,
    CaTxClr     INTEGER                       ,
    CaImage     TEXT                          ,
    CaPList     TEXT                          ,
    CaCheck     INTEGER                       ,
    primary key (DeptNum, CaNum),
    foreign key  (Cat_CaNum)
       references Categrs (CaNum),
    foreign key  (DeptNum)
       references Depts (DeptNum)
);

-- ============================================================
--   Index: Categrs_PK                                         
-- ============================================================
create unique index Categrs_PK on Categrs (DeptNum asc, CaNum asc);

-- ============================================================
--   Index: SubCats_FK                                         
-- ============================================================
create index SubCats_FK on Categrs (DeptNum asc, Cat_CaNum asc);

-- ============================================================
--   Index: CaName1_IN                                         
-- ============================================================
create index CaName1_IN on Categrs (DeptNum asc, CaCdNm1 asc);

-- ============================================================
--   Index: CaName2_IN                                         
-- ============================================================
create index CaName2_IN on Categrs (DeptNum asc, CaCdNm2 asc);


-- ============================================================
--   Table: Spcials                                            
-- ============================================================
create table Spcials
(
    DeptNum     INTEGER               not null,
    SPNum       INTEGER               not null,
    SPName1     TEXT                          ,
    SPName2     TEXT                          ,
    SPDefL      INTEGER                       ,
    SPImag1     TEXT                          ,
    SPImag2     TEXT                          ,
    SPCheck     INTEGER                       ,
    primary key (DeptNum, SPNum),
    foreign key  (DeptNum)
       references Depts (DeptNum)
);

-- ============================================================
--   Index: Spcials_PK                                         
-- ============================================================
create unique index Spcials_PK on Spcials (DeptNum asc, SPNum asc);


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
--   Table: SKGrps                                             
-- ============================================================
create table SKGrps
(
    DeptNum     INTEGER               not null,
    SGNum       INTEGER               not null,
    SGHGrid     INTEGER                       ,
    SGVGrid     INTEGER                       ,
    SGHSpc      INTEGER                       ,
    SGVSpc      INTEGER                       ,
    SGCheck     INTEGER                       ,
    primary key (DeptNum, SGNum),
    foreign key  (DeptNum)
       references Depts (DeptNum)
);

-- ============================================================
--   Index: SKGrps_PK                                          
-- ============================================================
create unique index SKGrps_PK on SKGrps (DeptNum asc, SGNum asc);


-- ============================================================
--   Table: SFeatrs                                            
-- ============================================================
create table SFeatrs
(
    DeptNum     INTEGER               not null,
    SFNum       INTEGER               not null,
    SSNum       INTEGER                       ,
    SGNum       INTEGER                       ,
    SFDHGrid    INTEGER                       ,
    SFDVGrid    INTEGER                       ,
    SFDHSpc     INTEGER                       ,
    SFDVSpc     INTEGER                       ,
    SFCheck     INTEGER                       ,
    primary key (DeptNum, SFNum),
    foreign key  (SSNum)
       references SStyles (SSNum),
    foreign key  (DeptNum, SGNum)
       references SKGrps (DeptNum, SGNum),
    foreign key  (DeptNum)
       references Depts (DeptNum)
);

-- ============================================================
--   Index: SFeatrs_PK                                         
-- ============================================================
create unique index SFeatrs_PK on SFeatrs (DeptNum asc, SFNum asc);


-- ============================================================
--   Table: Prods                                              
-- ============================================================
create table Prods
(
    PrID        INTEGER               not null,
    DeptNum     INTEGER               not null,
    ClNum       INTEGER                       ,
    TxNum       INTEGER                       ,
    ProdTxt5Num INTEGER                       ,
    ProdTxt6Num INTEGER                       ,
    ProdTxt7Num INTEGER                       ,
    ProdTxt8Num INTEGER                       ,
    ProdTxt9Num INTEGER                       ,
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
    PrDscMe     INTEGER                       ,
    PrDscPr1    INTEGER                       ,
    PrDscPr2    INTEGER                       ,
    PrDscPr3    INTEGER                       ,
    PrTare      INTEGER                       ,
    PrTareB     INTEGER                       ,
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
    PrInMode    INTEGER                       ,
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
    PrEANCd     TEXT                          ,
    PrRawBarC   TEXT                          ,
    PrCdNm1     TEXT                          ,
    PrCdNm2     TEXT                          ,
    PrSpcSD     TEXTDATETIME                  ,
    PrDesc      TEXT                          ,
    PrGrNm1     TEXT                          ,
    PrGrNm2     TEXT                          ,
    PrGrNm3     TEXT                          ,
    PrGrNm4     TEXT                          ,
    PrImage     TEXT                          ,
    PrCOLTN     TEXT                          ,
    PrMealSideItemsGroupNum   INTEGER         ,
    PrMealSideItemsAreFree    INTBOOLEAN      ,
    PrMealNumSidesIncluded    INTEGER         ,
    PrCutTestMeatSpecSheetGroupNum  INTEGER   ,
    PrWeightMin               INTEGER         ,
    PrWeightMax               INTEGER         ,
    PrTotalPriceMin           INTEGER         ,
    PrTotalPriceMax           INTEGER         ,
    PrManualLabelTypeNum1     INTEGER         ,
    PrManualLabelTypeNum2     INTEGER         ,
    PrManualLabelTypeNum3     INTEGER         ,
    PrPrepackLabelTypeNum1    INTEGER         ,
    PrPrepackLabelTypeNum2    INTEGER         ,
    PrPrepackLabelTypeNum3    INTEGER         ,
    PrProdEntryLabelTypeNum1  INTEGER         ,
    PrProdEntryLabelTypeNum2  INTEGER         ,
    PrProdEntryLabelTypeNum3  INTEGER         ,
    PrSelfServLabelTypeNum1   INTEGER         ,
    PrSelfServLabelTypeNum2   INTEGER         ,
    PrSelfServLabelTypeNum3   INTEGER         ,
    PrPick5LabelTypeNum1      INTEGER         ,
    PrPick5LabelTypeNum2      INTEGER         ,
    PrPick5LabelTypeNum3      INTEGER         ,
    PrCheck                   INTEGER         ,
    primary key (PrID),
    foreign key  (DeptNum)
       references Depts (DeptNum),
    foreign key  (CaNum)
       references Categrs (CaNum),
    foreign key  (SPNum)
       references Spcials (SPNum)
);

-- ============================================================
--   Index: Prods_PK                                           
-- ============================================================
create unique index Prods_PK on Prods (PrID asc);


-- ============================================================
--   Index: CaProds_FK                                         
-- ============================================================
create index CaProds_FK on Prods (DeptNum asc, CaNum asc);

-- ============================================================
--   Index: SPProds_FK                                         
-- ============================================================
create index SPProds_FK on Prods (SPNum asc);

-- ============================================================
--   Index: PrNum_AK                                           
-- ============================================================
create unique index PrNum_AK on Prods (DeptNum asc, PrNum asc);

-- ============================================================
--   Index: PrName1_IN                                         
-- ============================================================
create index PrName1_IN on Prods (PrCdNm1 asc);

-- ============================================================
--   Index: PrName2_IN                                         
-- ============================================================
create index PrName2_IN on Prods (PrCdNm2 asc);


-- ============================================================
--   Table: SftKeys                                            
-- ============================================================
create table SftKeys
(
    DeptNum     INTEGER               not null,
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
    primary key (DeptNum, SGNum, SKHPos, SKVPos),
    foreign key  (CaNum)
       references Categrs (CaNum),
    foreign key  (SFNum)
       references SFeatrs (SFNum),
    foreign key  (SGNum)
       references SKGrps (SGNum),
    foreign key  (DeptNum)
       references Depts (DeptNum)
);

-- ============================================================
--   Index: SftKeys_PK                                         
-- ============================================================
create unique index SftKeys_PK on SftKeys (DeptNum asc, SGNum asc, SKHPos asc, SKVPos asc);

-- ============================================================
--   Index: SGKeys_FK                                          
-- ============================================================
create index SGKeys_FK on SftKeys (DeptNum asc, SGNum asc);

-- ============================================================
-- Table structure for table Orders
-- ============================================================
CREATE TABLE Orders (
  DeptNum             INTEGER             not null,
  OrderNumber         INTEGER             not null,
  OrderDetails        TEXT                        ,
  OrderStatus         INTEGER                     ,
  CustomerName        TEXT                        ,
  OrderSMS            TEXT                        ,
  OrderEmail          TEXT                        ,
  OrderOrigin         INTEGER                     ,
  StoreNumber         INTEGER                     ,
  OrderCreation_dt    TEXTDATETIME                ,
  OrderPickup_dt      TEXTDATETIME                ,
  OrderStart_dt       TEXTDATETIME                ,
  OrderFinish_dt      TEXTDATETIME                ,
  OrderTotal_dt       TEXTDATETIME                ,
  BlockOrder          INTEGER                     ,
  OperatorNumber      INTEGER                     ,
  ReasonCanceled      TEXT                        ,
  DeniedCancelReq     INTEGER                     ,
  NumOfTimesPrinted   INTEGER                     ,
  primary key (DeptNum, OrderNumber),
  foreign key  (DeptNum)
       references Depts (DeptNum)
);

-- ============================================================
--   Index: Orders_PK                                           
-- ============================================================
create unique index Orders_PK on Orders (DeptNum asc, OrderNumber asc);


-- ============================================================
--   Table: OrderDetails                                            
-- ============================================================
CREATE TABLE OrderDetails
(
    DeptNum             INTEGER               not null,
	OrderNumId          INTEGER               not null,
    OrderNumber         INTEGER               not null,
    PluNumber           INTEGER                       ,
    ProductType         INTEGER                       ,
    PluPrice            INTEGER                       ,
	PluActualPrice      INTEGER                       ,
    PluByCount          INTEGER                       ,
    PluWeight           INTEGER                       ,
	PluWeightActual     INTETER                       ,
    PluThickness        INTEGER                       ,
    PluDescription  TEXT                              ,
    primary key   (DeptNum, OrderNumId),
    foreign key   (OrderNumber)
       references Orders (OrderNumber),
	foreign key  (DeptNum)
       references Depts (DeptNum)
);

-- ============================================================
--   Index: OrderDetails_PK                                           
-- ============================================================
create unique index OrderDetails_PK on OrderDetails (DeptNum asc, OrderNumId asc);

-- ============================================================
--   Index: ODIDs_FK                                         
-- ============================================================
create index OrderNum_FK on OrderDetails (OrderNumber asc);


-- ============================================================
--   Table: BlackBoxConfig                                            
-- ============================================================
create table BlackBoxConfig
(
    ColName      TEXT                  not null,
    ColValue     TEXT                          ,
    ColCheck     INTEGER                       ,
    primary key (ColName)
);

-- ============================================================
--   Index: BlackBoxConfig_PK
-- ============================================================
create unique index BlackBoxConfig_PK on BlackBoxConfig(ColName asc);
