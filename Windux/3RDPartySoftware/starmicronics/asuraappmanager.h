#ifndef ASURAAPPMANAGER_H
#define ASURAAPPMANAGER_H

#include <vector>
#include <string>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <QFile>
#include <QProcess>
#include <QStringList>
#include <starmicronics/libicpconfig.h>
#include "applications.h"


using namespace std;

struct acpSetting {
  string key;
  string value;
};

class AsuraAppManager
{
public:

    AsuraAppManager();

    vector<string> getAppsInstalled();

    vector<string> getAppsRunable();

    vector<string> getWebAppsRunable();

    vector<string> getCustomConfigurationApps();

    vector<string> getApplicationDirectories();

    vector<string> scanForNewApps();


    string getApplicationDescription(string appName);
    string getApplicationCategory(string appName);
    string getApplicationSupport(string appName);
    string getApplicationPrerun(string appName);
    string getApplicationPostrun(string appName);
    string getApplicationExecutable(string appName);
    string getApplicationParameters(string appName);
    string getApplicationType(string appName);
    string getApplicationIcon(string appName);
    string getApplicationBcrmode(string appName);
    string getApplicationHtdocs(string appName);
    string getApplicationHtaccess(string appName);
    string getApplicationjavapos(string appName);
    string getApplicationjasura(string appName);
    string getApplicationHome(string appName);

    QStringList getApplicationArguments(string appName);

 string getApplicationPath(string appName);

    void runApp(string appName);
    void setApplicationDirectories(string applicationPath);
    void loadApplications();
    void loadApplicationDir();
    vector<Applications*>* getApplicationsList();
    void setApplicationsList(Applications *);
    void setLaunchProcess(QString application, QString dir);
    string getApplicationValue(char const key[],char const filePath[]);

private:

    vector<Applications*> *applicationsList;

    vector<string> applicationDirectories;

    void listPid(qint64 app_pid, QString & application);
    void deListPid(qint64 app_pid);

};

#endif // ASURAAPPMANAGER_H
