# script to upgrade an existing online ordering database to the current version

import os
import sys
import pymysql
import pymysql.cursors

LATEST_VERSION = "2.0.0"

# increase length of PluInstructions field
# change key design of orderdetails table
def migrate_1_0_0(db_conn):
    
    new_version = "1.1.0"

    try:
        with db_conn.cursor() as cursor:
            cursor.execute("alter table orderdetails modify column PluInstructions varchar(128)")
            
            cursor.execute("alter table settings add column OrdersPerSlot int(11) default 4")
            
            cursor.execute("alter table orderdetails drop primary key, add primary key(`DeptNum`,`OrderNumId`, `OrderNumber`)")
            
            cursor.execute("alter table deptssettings add column PrepOptions varchar(255) DEFAULT ''")
            
            cursor.execute("update blackboxconfig set ColValue = %s where ColName = 'databaseVersion'", (new_version,))
            
        db_conn.commit()
    except:
        raise

def get_current_db_version(db_conn):
    with db_conn.cursor() as cursor:
        cursor.execute("select * from blackboxconfig where ColName = 'databaseVersion'")
        return cursor.fetchone()["ColValue"]

# grabs functions in this file that start with "migrate_"
migrations = {k.lstrip("migrate_").replace("_","."): v for k, v in locals().items() if k.startswith("migrate_")}

def migrate():
    current_db_version = None
    db_conn = None
    
    try:
        db_conn = pymysql.connect(host='localhost',
                                 user='hobart',
                                 password='hobart',
                                 database='online_ordering',
                                 charset='utf8mb4',
                                 cursorclass=pymysql.cursors.DictCursor)
    except:
        print("failed to connect to database!")
        sys.exit(-1)
        
    with db_conn:
        try:
            current_db_version = get_current_db_version(db_conn)
            print("start\t%s" % current_db_version)
        except:
            print("failed to read version out of database!")
            sys.exit(-1)

        while current_db_version != LATEST_VERSION:
            if current_db_version not in migrations:
                print("no migration found from version %s" % current_db_version)
                sys.exit(-1)

            migrations[current_db_version](db_conn)
            current_db_version = get_current_db_version(db_conn)
            print("-->\t%s" % current_db_version)
        
        print("end\t%s" % current_db_version)

if __name__ == "__main__":
    if os.path.exists("/var/mysql/online_ordering"):
        migrate()
    else:
        print("no online ordering database exists, nothing to migrate")
