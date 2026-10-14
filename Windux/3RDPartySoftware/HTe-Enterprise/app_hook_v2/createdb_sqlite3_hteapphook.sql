-- ============================================================
--   Database name:  HTe-Enterprise Support DB (HTeAppHook.db)                                  
--   DBMS name:      Sqlite 3                                  
--   Created on:     1/26/2022                       
-- ============================================================

-- ============================================================
--   Table: Downloads                                              
-- ============================================================
create table Downloads
(
    FileName                     TEXT            not null,
	FileId                       TEXT,
	FileSize                     INTEGER,
	FileVersion                  TEXT,
    Checksum                     TEXT,
    MostRecentDownloadDateTime   TEXTDATETIME,
	ListedInCurrentHeartbeat     INTBOOLEAN,
	Status                       INTEGER,                 -- Downloaded vs. Installed?
    primary key (FileName)
);

-- ============================================================
--   Index: Downloads_PK                                           
-- ============================================================
create unique index Downloads_PK on Downloads (FileName asc);


-- ============================================================
--   Table: Configs                                            
-- ============================================================
create table Configs
(
    CoName      TEXT                  not null,
    CoValue     TEXT                          ,
    primary key (CoName)
);

-- ============================================================
--   Index: Configs_PK                                         
-- ============================================================
create unique index Configs_PK on Configs (CoName asc);

