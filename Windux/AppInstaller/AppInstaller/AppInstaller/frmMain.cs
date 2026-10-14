using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Diagnostics;
using System.Threading;
using System.IO;

using System.Security;
using System.Security.AccessControl;
using System.Security.Principal;	


namespace AppInstaller
{
    public partial class frmMain : Form
    {
        bool showResults = false;

        public frmMain()
        {
            InitializeComponent();
        }

        //This function will take a progress bar text update, 
        //will take a value to use in setting the progress bar to a certain value (0-100)
        //Also will take a sleepTime (in ms) to sleep
        private void UpdateProgressBarAndSleep(string progressBarText, int newProgressBarVal, int sleepTime)
        {
            if (showResults)
                MessageBox.Show("AppInstaller " + progressBarText + "val=" + newProgressBarVal);
            tsStatusLabel.Text = progressBarText;
            tsProgressBar.Value = newProgressBarVal;
            Application.DoEvents();
            Thread.Sleep(sleepTime);
        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="filename"></param>
        /// <param name="arguments"></param>
        /// <param name="wait"></param>
        /// <param name="standardInput"></param>
        public static string RunCommandLineProgram(string filename, string arguments, bool wait, string standardInput)
        {
            Process p = new Process();
            string output;
            p.StartInfo = new ProcessStartInfo();
            p.StartInfo.FileName = filename;
            p.StartInfo.Arguments = arguments;
            p.StartInfo.WindowStyle = ProcessWindowStyle.Hidden;
            p.StartInfo.RedirectStandardOutput = true;
            p.StartInfo.RedirectStandardError = true;
            if (!string.IsNullOrEmpty(standardInput))
                p.StartInfo.RedirectStandardInput = true;
            p.StartInfo.UseShellExecute = false;
            p.Start();
            if (!wait)
                return string.Empty;
            using (StreamReader srStandardOutput = p.StandardOutput)
            {
                if (p.StartInfo.RedirectStandardInput)
                    using (StreamWriter sw = p.StandardInput)
                        sw.WriteLine(standardInput);
                p.WaitForExit();
                output = p.StandardError.ReadToEnd() + "\n\n" + srStandardOutput.ReadToEnd();
            }
            return output;
        }


        //load our form
        private void frmMain_Load(object sender, EventArgs e)
        {
            this.Text = "Hobart Application Upgrade";
            pbHobart.Visible = true;

            // Prevent form from being resized. 
            this.FormBorderStyle = FormBorderStyle.FixedSingle;

            // Get the screen bounds 
            Rectangle formrect = Screen.GetBounds(this);

            // Set the form's location and size -> will fill entire screen, cover taskbar
            this.Location = formrect.Location;
            this.Size = formrect.Size;

            timer1.Enabled = true;
        }


        //This function will run once, launching the various HLX needed applications
        private void timer1_Tick(object sender, EventArgs e)
        {
            timer1.Enabled = false;

            string statusMsg = "Upgrading, please wait ...";
            int upgradeSteps = 10;
            int upgradeStep = 0;
            UpdateProgressBarAndSleep(statusMsg, (100 / upgradeSteps) * upgradeStep, 1000);


            string versionManagerTempDir = "";   // "/etc/hobart/vmTmp";
            string appUpgradeIndicatorFile = "";   // "/etc/hobart/vmTmp/appInstaller.log";
            string appUpgradeDirectory = "";
            string dbBackupFile = "";
            string dbLocation = "";
            string emptyDbLocation = "";
            string systemDir = "";
            string webPagesDir = "";

            string result = "";
            string sourceDirectory = "";
            string targetDirectory = "";

            try
            {
                StreamReader myReader = new StreamReader("/etc/hobart/config_linux.dat");

                string currentConfigLine = "";
                int configCount = 0;
                while ((currentConfigLine = myReader.ReadLine()) != null)
                {
                    //look until we hit scaleModel line
                    if (currentConfigLine.StartsWith("VersionManagerTempDir"))
                    {
                        versionManagerTempDir = currentConfigLine.Substring(currentConfigLine.IndexOf('=') + 1);
                        appUpgradeDirectory = versionManagerTempDir + "SC";
                        configCount++;
                    }
                    else if (currentConfigLine.StartsWith("appUpgradeIndicatorFile"))
                    {
                        appUpgradeIndicatorFile = currentConfigLine.Substring(currentConfigLine.IndexOf('=') + 1);
                        configCount++;
                    }
                   else if (currentConfigLine.StartsWith("versionIndependentBackupFile"))
                    {
                        dbBackupFile = currentConfigLine.Substring(currentConfigLine.IndexOf('=') + 1);
                        configCount++;
                    }
                    else if (currentConfigLine.StartsWith("dbLocation"))
                    {
                        dbLocation = currentConfigLine.Substring(currentConfigLine.IndexOf('=') + 1);
                        configCount++;
                    }
                    else if (currentConfigLine.StartsWith("emptyDbLocation"))
                    {
                        emptyDbLocation = currentConfigLine.Substring(currentConfigLine.IndexOf('=') + 1);
                        configCount++;
                    }
                    else if (currentConfigLine.StartsWith("systemDir"))
                    {
                        systemDir = currentConfigLine.Substring(currentConfigLine.IndexOf('=') + 1);
                        configCount++;
                    }
                    else if (currentConfigLine.StartsWith("webPagesDir"))
                    {
                        webPagesDir = currentConfigLine.Substring(currentConfigLine.IndexOf('=') + 1);
                        configCount++;
                    }

                    if (configCount >= 7)
                        break;
                }

                myReader.Close();
                myReader.Dispose();
            }
            catch (Exception)
            {
                //We don't want to throw up any message here, just use the default hobart image
            }

            if (showResults)
            {
                MessageBox.Show("AppInstaller versionManagerTempDir is " + versionManagerTempDir);
                MessageBox.Show("AppInstaller appUpgradeDirectory is " + appUpgradeDirectory);
                MessageBox.Show("AppInstaller appUpgradeIndicatorFile is " + appUpgradeIndicatorFile);
                MessageBox.Show("AppInstaller dbBackupFile is " + dbBackupFile);
                MessageBox.Show("AppInstaller dbLocation is " + dbLocation);
                MessageBox.Show("AppInstaller emptyDbLocation is " + emptyDbLocation);
                MessageBox.Show("AppInstaller systemDir is " + systemDir);
                MessageBox.Show("AppInstaller webPagesDir is " + webPagesDir);
            }

            try
            {
                // save current version.ini from normal location (/etc/hobart) to upgrade source location (/etc/hobart/vmTmp/SC/)
                if (File.Exists("/etc/hobart/version.ini"))
                    File.Copy("/etc/hobart/version.ini", appUpgradeDirectory + "/etc/hobart/version.ini", true);

                UpdateProgressBarAndSleep(statusMsg, (100 / upgradeSteps) * (++upgradeStep), 1000);


                // if db version-independent file exists, install the new db files
                if (File.Exists(dbBackupFile))
                {
                    result = RunCommandLineProgram("chmod", "-R 777 " + dbLocation, true, "");
                    if (showResults)
                        MessageBox.Show("result of chmod " + dbLocation + " is " + result);

                    if (Directory.Exists(dbLocation))
                        Directory.Delete(dbLocation, true);

                    // Shouldn't we be deleting any copy of the database (ie. dbmirror)?
                    // They can't be used anymore in case the main db directory becomes corrupt, because they have a different schema now.

                    Directory.CreateDirectory(dbLocation);

                    // Copy all the files
                    foreach (string existingFile in Directory.GetFiles(appUpgradeDirectory + "/etc/hobart/dbempty", "*", SearchOption.TopDirectoryOnly))
                        File.Copy(existingFile, existingFile.Replace(appUpgradeDirectory + "/etc/hobart/dbempty", dbLocation));

                    File.Copy(dbBackupFile, appUpgradeDirectory + "/etc/hobart", true);
                }

                UpdateProgressBarAndSleep(statusMsg, (100 / upgradeSteps) * (++upgradeStep), 1000);


                // Upgrade /usr/local/hobart/system ...

                sourceDirectory = appUpgradeDirectory + "/system";
                targetDirectory = "/usr/local/hobart";

                result = RunCommandLineProgram("chmod", "-R 777 " + targetDirectory, true, "");
                if (showResults)
                    MessageBox.Show("result of chmod " + targetDirectory + " is " + result);

                // Remove the System directory and all subdirectories/files (/usr/local/hobart/system/)
                if (Directory.Exists(systemDir))
                    Directory.Delete(systemDir, true);

                // Create all of the 'system' directories
                foreach (string dirPath in Directory.GetDirectories(sourceDirectory, "*", SearchOption.AllDirectories))
                    Directory.CreateDirectory(dirPath.Replace(appUpgradeDirectory, targetDirectory));

                // Copy all the files
                foreach (string existingFile in Directory.GetFiles(sourceDirectory, "*", SearchOption.AllDirectories))
                    File.Copy(existingFile, existingFile.Replace(appUpgradeDirectory, targetDirectory));

                result = RunCommandLineProgram("chown", "-R oper " + targetDirectory, true, "");
                if (showResults)
                    MessageBox.Show("result of chown " + targetDirectory + " is " + result);
                result = RunCommandLineProgram("chgrp", "-R hob " + targetDirectory, true, "");
                if (showResults)
                    MessageBox.Show("result of chgrp " + targetDirectory + " is " + result);
                result = RunCommandLineProgram("chmod", "-R 774 " + targetDirectory, true, "");
                if (showResults)
                    MessageBox.Show("result of chmod " + targetDirectory + " is " + result);

                UpdateProgressBarAndSleep(statusMsg, (100 / upgradeSteps) * (++upgradeStep), 1000);


                // remove the emptyDb directory and all subdirectories/files
                if (Directory.Exists(emptyDbLocation))
                {
                    result = RunCommandLineProgram("chmod", "-R 777 " + emptyDbLocation, true, "");
                    if (showResults)
                        MessageBox.Show("result of chmod " + emptyDbLocation + " is " + result);

                    Directory.Delete(emptyDbLocation, true);
                }

                UpdateProgressBarAndSleep(statusMsg, (100 / upgradeSteps) * (++upgradeStep), 1000);


                // Upgrade etc/hobart ...

                sourceDirectory = appUpgradeDirectory + "/etc/hobart";
                targetDirectory = "/etc/hobart";

                result = RunCommandLineProgram("chmod", "-R 777 " + targetDirectory, true, "");
                if (showResults)
                    MessageBox.Show("result of chmod " + targetDirectory + " is " + result);

                // Delete all files in /etc/hobart (not including any remaining subdirectories)
                foreach (string newPath in Directory.GetFiles(targetDirectory, "*", SearchOption.TopDirectoryOnly))
                    File.Delete(newPath);

                UpdateProgressBarAndSleep(statusMsg, (100 / upgradeSteps) * (++upgradeStep), 1000);


                // Create all of the directories in etc/hobart
                foreach (string dirPath in Directory.GetDirectories(sourceDirectory, "*", SearchOption.AllDirectories))
                    Directory.CreateDirectory(dirPath.Replace(appUpgradeDirectory, ""));

                // Copy all the files
                foreach (string existingFile in Directory.GetFiles(sourceDirectory, "*", SearchOption.AllDirectories))
                {
                    string targetFile = existingFile.Replace(appUpgradeDirectory, "");
                    File.Copy(existingFile, targetFile, true);
                }

                result = RunCommandLineProgram("chown", "-R oper " + targetDirectory, true, "");
                if (showResults)
                    MessageBox.Show("result of chown " + targetDirectory + " is " + result);
                result = RunCommandLineProgram("chgrp", "-R hob " + targetDirectory, true, "");
                if (showResults)
                    MessageBox.Show("result of chgrp " + targetDirectory + " is " + result);
                result = RunCommandLineProgram("chmod", "-R 774 " + targetDirectory, true, "");
                if (showResults)
                    MessageBox.Show("result of chmod " + targetDirectory + " is " + result);

                UpdateProgressBarAndSleep(statusMsg, (100 / upgradeSteps) * (++upgradeStep), 1000);



                // Upgrade webpages ...

                sourceDirectory = appUpgradeDirectory + "/webpages";

                result = RunCommandLineProgram("chmod", "-R 777 " + webPagesDir, true, "");
                if (showResults)
                    MessageBox.Show("result of chmod " + webPagesDir + " is " + result);

                // Remove the following directories within webPages/images/ that we control
                if (Directory.Exists(webPagesDir + "/images/blanks"))
                    Directory.Delete(webPagesDir + "/images/blanks", true);

                if (Directory.Exists(webPagesDir + "/images/epp"))
                    Directory.Delete(webPagesDir + "/images/epp", true);

                if (Directory.Exists(webPagesDir + "/images/esc"))
                    Directory.Delete(webPagesDir + "/images/esc", true);

                if (Directory.Exists(webPagesDir + "/images/gradients"))
                    Directory.Delete(webPagesDir + "/images/gradients", true);

                if (Directory.Exists(webPagesDir + "/images/keyboards"))
                    Directory.Delete(webPagesDir + "/images/keyboards", true);

                if (Directory.Exists(webPagesDir + "/images/selfserv"))
                    Directory.Delete(webPagesDir + "/images/selfserv", true);

                if (Directory.Exists(webPagesDir + "/images/specials"))
                    Directory.Delete(webPagesDir + "/images/specials", true);

                if (Directory.Exists(webPagesDir + "/images/system"))
                    Directory.Delete(webPagesDir + "/images/system", true);

                if (Directory.Exists(webPagesDir + "/images/unit"))
                    Directory.Delete(webPagesDir + "/images/unit", true);

                if (Directory.Exists(webPagesDir + "/images/universal"))
                    Directory.Delete(webPagesDir + "/images/universal", true);

                // Delete all files in webpages/images (not including any remaining subdirectories)
                foreach (string existingFile in Directory.GetFiles(webPagesDir + "/images", "*", SearchOption.TopDirectoryOnly))
                    File.Delete(existingFile);

                // Delete all files and subdirectories in webpages/language
                if (Directory.Exists(webPagesDir + "/language"))
                    Directory.Delete(webPagesDir + "/language", true);

                // Delete all files in webpages (not including any remaining subdirectories)
                foreach (string existingFile in Directory.GetFiles(webPagesDir, "*", SearchOption.TopDirectoryOnly))
                    File.Delete(existingFile);

                // what about ...
                // rm -rf /var/www/images/softkeys

                UpdateProgressBarAndSleep(statusMsg, (100 / upgradeSteps) * (++upgradeStep), 1000);


                // Create all of the webpages directories
                foreach (string dirPath in Directory.GetDirectories(sourceDirectory, "*", SearchOption.AllDirectories))
                    Directory.CreateDirectory(dirPath.Replace(sourceDirectory, webPagesDir));

                // Copy all the webpages files
                foreach (string existingFile in Directory.GetFiles(sourceDirectory, "*", SearchOption.AllDirectories))
                    File.Copy(existingFile, existingFile.Replace(sourceDirectory, webPagesDir), true);

                result = RunCommandLineProgram("chown", "-R oper " + webPagesDir, true, "");
                if (showResults)
                    MessageBox.Show("result of chown " + webPagesDir + " is " + result);
                result = RunCommandLineProgram("chgrp", "-R hob " + webPagesDir, true, "");
                if (showResults)
                    MessageBox.Show("result of chgrp " + webPagesDir + " is " + result);
                result = RunCommandLineProgram("chmod", "-R 555 " + webPagesDir, true, "");
                if (showResults)
                    MessageBox.Show("result of chmod " + webPagesDir + " is " + result);
                result = RunCommandLineProgram("chmod", "-R 775 " + webPagesDir + "/images/categories", true, "");
                if (showResults)
                    MessageBox.Show("result of chmod " + webPagesDir + "/images/categories" + " is " + result);
                result = RunCommandLineProgram("chmod", "-R 775 " + webPagesDir + "/images/products", true, "");
                if (showResults)
                    MessageBox.Show("result of chmod " + webPagesDir + "/images/products" + " is " + result);
                result = RunCommandLineProgram("chmod", "-R 775 " + webPagesDir + "/images/softkeys", true, "");
                if (showResults)
                    MessageBox.Show("result of chmod " + webPagesDir + "/images/softkeys" + " is " + result);

                UpdateProgressBarAndSleep(statusMsg, (100 / upgradeSteps) * (++upgradeStep), 1000);


                // Upgrade /usr/bin ...

                // remove the win directory (handled when I remove the webs.exe below)
                // remove files from Access Scale directory
                // Windows: appInstaller.exe, appLauncher.exe, AT91Boot_DLL.dll, bootParams.cfg, config.dat, DST.TXT, ScaleService.exe, srvany.exe,
                //         Text.en_CA.txt (.en_US, .es_MX, .fr_CA), version.ini (alread saved), WifiProfiles.exe
                // Can't just delete all in /usr/bin, and can't be sure of the names of all hobart files in this directory (could vary over time)
                // so don't delete anything here before copying over the upgrade files in /usr/bin

                // Create all of the executable directories
                sourceDirectory = appUpgradeDirectory + "/usr/bin";
                foreach (string dirPath in Directory.GetDirectories(sourceDirectory, "*", SearchOption.AllDirectories))
                    Directory.CreateDirectory(dirPath.Replace(appUpgradeDirectory, ""));

                // Copy all the executable files
                foreach (string existingFile in Directory.GetFiles(sourceDirectory, "*", SearchOption.AllDirectories))
                {
                    string targetFile = existingFile.Replace(appUpgradeDirectory, "");
                    File.Copy(existingFile, targetFile, true);

                    result = RunCommandLineProgram("chown", "root " + targetFile, true, "");
                    if (showResults)
                        MessageBox.Show("result of chown " + targetFile + " is " + result);
                    result = RunCommandLineProgram("chgrp", "hob " + targetFile, true, "");
                    if (showResults)
                        MessageBox.Show("result of chgrp " + targetFile + " is " + result);
                    result = RunCommandLineProgram("chmod", "555 " + targetFile, true, "");
                    if (showResults)
                        MessageBox.Show("result of chmod " + targetFile + " is " + result);

                    // System.Security.AccessControl.FileSecurity fSecurity = File.GetAccessControl(newPath);
                    // Remove the FileSystemAccessRule from the security settings.
                    // fSecurity.RemoveAccessRule(new FileSystemAccessRule(account, rights, controlType));
                    // Set the new access settings.
                    // File.SetAccessControl(scalePath, fSecurity);
                }

                UpdateProgressBarAndSleep(statusMsg, (100 / upgradeSteps) * (++upgradeStep), 1000);


                if (File.Exists(appUpgradeIndicatorFile))
                    File.Delete(appUpgradeIndicatorFile);

                UpdateProgressBarAndSleep(statusMsg, (100 / upgradeSteps) * (++upgradeStep), 1000);


                //result = RunCommandLineProgram("chmod", "-R 777 " + versionManagerTempDir, true, "");
                //if (showResults)
                //    MessageBox.Show("result of chmod " + versionManagerTempDir + " is " + result);

                RunCommandLineProgram("/usr/bin/HLXLinuxLauncher.exe", "", false, "");
            
            }
            catch (System.AccessViolationException eee)
            {
                MessageBox.Show("Upgrade Problem! AccessViolationException  " + eee.ToString());
            }
            catch (System.IO.IOException eee2)
            {
                MessageBox.Show("Upgrade Problem! IO.IOException  " + eee2.ToString());
            }
            catch (System.UnauthorizedAccessException eee3)
            {
                MessageBox.Show("Upgrade Problem! UnauthorizedAccessException  " + eee3.ToString());
            }
            catch (Exception)
            {
                MessageBox.Show("Application Upgrade Problem!");
            }
            
           
            //Exit the application
            Application.Exit();
        }


        // Adds an ACL entry on the specified file for the specified account.
        public void AddFileSecurity(string fileName, string account,
            FileSystemRights rights, AccessControlType controlType)
        {


            // Get a FileSecurity object that represents the
            // current security settings.
            System.Security.AccessControl.FileSecurity fSecurity = File.GetAccessControl(fileName);
    //System.Security.F
            // Add the FileSystemAccessRule to the security settings.
            fSecurity.AddAccessRule(new FileSystemAccessRule(account,
                rights, controlType));

            // Set the new access settings.
            File.SetAccessControl(fileName, fSecurity);

        }

    
    
    
    }



}
