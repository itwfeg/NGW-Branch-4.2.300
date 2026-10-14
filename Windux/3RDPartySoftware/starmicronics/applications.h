#ifndef APPLICATIONS_H
#define APPLICATIONS_H

#include<string>

using namespace std;

class Applications
{
public:
    Applications();

    void setName(string applicationName);
    void setDescription(string description);
    void setCategory(string category);
    void setSupport(string support);
    void setPrerun(string prerun);
    void setPostrun(string postrun);
    void setExecutable(string executable);
    void setParameters(string parameters);
    void setApplication(string application);
    void setType(string type);
    void setIcon(string icon);
    void setBcrmode(string bcrmode);
    void setHtdocs(string htdocs);
    void setHtaccess(string htaccess);
    void setJavapos(string javapos);
    void setJasura(string jasura);
    void setHome(string home);
    void setFoundConfigurationSo(bool found);
    void setPath(string path);
    void setHidden(string hidden);

    bool isFoundConfigurationSo();
    string getName();
    string getDescription();
    string getCategory();
    string getSupport();
    string getPrerun();
    string getPostrun();
    string getExecutable();
    string getParameters();
    string getApplication();
    string getType();
    string getIcon();
    string getBcrmode();
    string getHtdocs();
    string getHtaccess();
    string getjavapos();
    string getjasura();
    string getHome();
    string getPath();
    string getHidden();


private:
    bool found;

    string applicationName;
    string description;
    string category;
    string support;
    string prerun;
    string postrun;
    string executable;
    string parameters;
    string application;
    string type;
    string icon;
    string bcrmode;
    string htdocs;
    string htaccess;
    string javapos;
    string jasura;
    string home;
    string path;
    string m_hidden;
};

#endif // APPLICATIONS_H
