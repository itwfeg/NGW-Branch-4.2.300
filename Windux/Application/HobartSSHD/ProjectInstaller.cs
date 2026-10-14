using System;
using System.Collections;
using System.Collections.Generic;
using System.ComponentModel;
using System.Configuration.Install;
using System.Linq;


namespace HobartSSHD
{
    [RunInstaller(true)]
    public partial class ProjectInstaller : System.Configuration.Install.Installer
    {
        public ProjectInstaller()
        {
            InitializeComponent();
        }

        public string GetContextParameter(string key)
        {
            string sValue = "";
            try
            {
                sValue = this.Context.Parameters[key].ToString();
            }
            catch
            {
                sValue = "";
            }
            return sValue;
        }

        // Override the 'OnBeforeInstall' method.
        protected override void OnBeforeInstall(IDictionary savedState)
        {
            base.OnBeforeInstall(savedState);

            string username = GetContextParameter("user").Trim();
            string password = GetContextParameter("password").Trim();

            if (username != "")
                serviceProcessInstaller.Username = username;
            if (password != "")
                serviceProcessInstaller.Password = password;
        }
    }
}
