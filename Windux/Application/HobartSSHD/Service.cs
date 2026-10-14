using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Diagnostics;
using System.Linq;
using System.ServiceProcess;
using System.Text;

namespace HobartSSHD
{
    //
    // NOTE:
    // This service may be installed by running "installutil.exe hobartsshd.exe" from a command line. If you desire a
    // specific username and password use "installutil.exe /user=uname /password=pw hobartsshd.exe" instead.
    //
    public partial class Service : ServiceBase
    {
#region Implementation

        public Service()
        {
            Debug.WriteLine("[Service] CTOR");

            InitializeComponent();
        }

        protected override void OnStart(string[] args)
        {
            Debug.WriteLine("[OnStart] Starting Hobart SSHD service...");

            // create and start worker thread to let service run.
            System.Threading.Thread t = new System.Threading.Thread(new System.Threading.ThreadStart(StartSSHD));

            try
            {
                t.Start();
            }
            catch (System.Exception e)
            {
                Debug.WriteLine("[OnStart] Exception starting Hobart SSHD service..." + Environment.NewLine + e.ToString());
            }
        }

        protected override void OnStop()
        {
            Debug.WriteLine("[OnStop] Stopping Hobart SSHD service...");

            // not running any more
            isRunning_.Set();
        }

#endregion
#region Worker Thread

        internal System.Threading.AutoResetEvent isRunning_ = new System.Threading.AutoResetEvent( false );

        internal string rootDir_ = "c:\\";
        internal string uploadDir_ = "c:\\Program Files\\Hobart\\";
        internal string homeDir_ = "c:\\Program Files\\Hobart\\";

        internal string[] adminUsers_ = { "root" };

        internal int maxConnections_ = 0;

        internal void StartSSHD()
        {
            Debug.WriteLine("[StartSSHD] Creating SSHD...");

            wodSSHDComLIB.wodSSHDCom sshd = new wodSSHDComLIB.wodSSHDCom();

            if (null == sshd)
            {
                Debug.WriteLine("[StartSSHD] Could not create SSHD!");
                return;
            }

            Debug.WriteLine("[StartSSHD] Setting up SSHD...");

            // setup server
            sshd.LicenseKey = "MC45-T8VP-K435-323P";

            sshd.Connecting += new wodSSHDComLIB._IwodSSHDComEvents_ConnectingEventHandler(ConnectingEventHandler);
            sshd.Disconnected += new wodSSHDComLIB._IwodSSHDComEvents_DisconnectedEventHandler(DisconnectedEventHandler);
            sshd.LoginPassword += new wodSSHDComLIB._IwodSSHDComEvents_LoginPasswordEventHandler(LoginPasswordEventHandler);
            sshd.ServiceRequest += new wodSSHDComLIB._IwodSSHDComEvents_ServiceRequestEventHandler(ServiceRequestEventHandler);
            sshd.ServiceStart += new wodSSHDComLIB._IwodSSHDComEvents_ServiceStartEventHandler(ServiceStartEventHandler);

            sshd.SftpDeleteFile += new wodSSHDComLIB._IwodSSHDComEvents_SftpDeleteFileEventHandler(SftpDeleteFileEventHandler);
            sshd.SftpDownloadFile += new wodSSHDComLIB._IwodSSHDComEvents_SftpDownloadFileEventHandler(SftpDownloadFileEventHandler);
            sshd.SftpListDir += new wodSSHDComLIB._IwodSSHDComEvents_SftpListDirEventHandler(SftpListDirEventHandler);
            sshd.SftpMakeDir += new wodSSHDComLIB._IwodSSHDComEvents_SftpMakeDirEventHandler(SftpMakeDirEventHandler);
            sshd.SftpRemoveDir += new wodSSHDComLIB._IwodSSHDComEvents_SftpRemoveDirEventHandler(SftpRemoveDirEventHandler);
            sshd.SftpRename += new wodSSHDComLIB._IwodSSHDComEvents_SftpRenameEventHandler(SftpRenameEventHandler);
            sshd.SftpUploadFile += new wodSSHDComLIB._IwodSSHDComEvents_SftpUploadFileEventHandler(SftpUploadFileEventHandler);

            sshd.MaxLoginAttempts = 3;
            sshd.Protocol = wodSSHDComLIB.ProtocolsEnum.SSH2;
            sshd.Port = 22;
            sshd.Timeout = 0;

            // generate key file
            string FileName = System.IO.Path.Combine(AppDomain.CurrentDomain.BaseDirectory, "hobsshd.rsa");
            Debug.WriteLine("[StartSSHD] RSA Key located at " + FileName);

            if (System.IO.File.Exists(FileName))
                sshd.Keys.Load(FileName, null);
            else
            {
                Debug.WriteLine("[StartSSHD] Generating RSA Key...");

                sshd.Keys.Generate(wodSSHDComLIB.SSHKeyTypes.RSAkey, 1024);
                sshd.Keys.Save(wodSSHDComLIB.SSHKeyTypes.RSAkey, FileName, null);
            }

            // read admin users
            string AdminUsersFile = System.IO.Path.Combine(AppDomain.CurrentDomain.BaseDirectory, "adminusers.txt");

            if (System.IO.File.Exists(AdminUsersFile))
            {
                // read lines
                try
                {
                    adminUsers_ = System.IO.File.ReadAllLines(AdminUsersFile);
                }
                catch (System.Exception /*e*/)
                {
                }
            }

            // read configuration
            string ConfigFile = System.IO.Path.Combine(AppDomain.CurrentDomain.BaseDirectory, "config.txt");

            if (System.IO.File.Exists(ConfigFile))
            {
                const string ROOT_DIR = "rootdir=";
                const string UPLOAD_DIR = "uploaddir=";
                const string HOME_DIR = "homedir=";
                const string MAX_CONNECTIONS = "maxconnections=";

                // read lines
                try
                {
                    string[] configs = System.IO.File.ReadAllLines(ConfigFile);

                    foreach (string c in configs)
                    {
                        if (1 <= c.Length)
                            if ("#" == c.Substring(0, 1))
                                continue;

                        if (ROOT_DIR.Length <= c.Length)
                            if (ROOT_DIR == c.Substring(0, ROOT_DIR.Length))
                                rootDir_ = c.Substring(ROOT_DIR.Length);

                        if (UPLOAD_DIR.Length <= c.Length)
                            if (UPLOAD_DIR == c.Substring(0, UPLOAD_DIR.Length))
                            {
                                string dir = c.Substring(UPLOAD_DIR.Length);

                                if (rootDir_.Equals(dir.Substring(0, rootDir_.Length), StringComparison.OrdinalIgnoreCase))
                                    uploadDir_ = dir;
                            }


                        if (HOME_DIR.Length <= c.Length)
                            if (HOME_DIR == c.Substring(0, HOME_DIR.Length))
                            {
                                string dir = c.Substring(HOME_DIR.Length);

                                if (rootDir_.Equals(dir.Substring(0, rootDir_.Length), StringComparison.OrdinalIgnoreCase))
                                    homeDir_ = dir;
                            }

                        if (MAX_CONNECTIONS.Length <= c.Length)
                            if (MAX_CONNECTIONS == c.Substring(0, MAX_CONNECTIONS.Length))
                                maxConnections_ = Convert.ToInt32(c.Substring(MAX_CONNECTIONS.Length));
                    }
                }
                catch (System.Exception /*e*/)
                {
                }
            }


            // start!
            try
            {
                Debug.WriteLine("[StartSSHD] Starting SSHD on port " + sshd.Port);
                sshd.Start(sshd.Port);
            }
            catch (System.Exception e)
            {
                Debug.WriteLine("[StartSSHD] Exception starting SSHD!" + Environment.NewLine + e.ToString());
                System.IO.File.AppendAllText(sshd.DebugFile, e.Message);
            }

            Debug.WriteLine("[StartSSHD] SSHD is running");

            // This is needed in order to keep service running. If we do not perform this step,
            // the service would start and stop immediately, thus shutting down the server.
            isRunning_.WaitOne();

            Debug.WriteLine("[StartSSHD] Stopping SSHD");
            sshd.Stop();

            // finished with interface
            sshd = null;
        }

        internal bool ValidatePassword(string User, string Password)
        {
            string FileName = System.IO.Path.Combine(AppDomain.CurrentDomain.BaseDirectory, User+".pub");
            string EncodedPassword;

            try
            {
                // read encoded password from file
                System.IO.StreamReader sr = new System.IO.StreamReader(FileName);
                EncodedPassword = sr.ReadLine();
                sr.Close();
            }
            catch (System.Exception /*e*/)
            {
                EncodedPassword = "";
            }

            // encoded password does not exist or is wrong length, check against default password
            if (34 != EncodedPassword.Length)
                return false;

            Debug.WriteLine("[ValidatePassword] Encoded password is " + EncodedPassword);

            // first 2 characters are the salt, remaining 32 characters is MD5 hash
            string Salt = EncodedPassword.Substring(0, 2);
            string Hash = EncodedPassword.Substring(2);

            // encode password with salt
            System.Security.Cryptography.MD5 HashAlgorithm = System.Security.Cryptography.MD5.Create();
            byte[] data = HashAlgorithm.ComputeHash(Encoding.ASCII.GetBytes(Password + Salt));

            // convert hash to string
            StringBuilder result = new StringBuilder(data.Length * 2);

            for (int i = 0; i < data.Length; ++i)
                result.Append(data[i].ToString("x2"));

            string HashedPassword = result.ToString();

            Debug.WriteLine("[ValidatePassword] Hashed result is " + HashedPassword);

            // validate
            return (Hash.ToLower() == HashedPassword);            
        }

        internal bool IsReadOnlyControlAllowed(string User, string ResolvedPath)
        {
            if (rootDir_.Equals(ResolvedPath.Substring(0, rootDir_.Length), StringComparison.OrdinalIgnoreCase))
                return true;

            return false;
        }

        internal bool IsFullControlAllowed(string User, string ResolvedPath)
        {
            // only allow full access in hobart subfolder
            if (IsReadOnlyControlAllowed(User, ResolvedPath))
                if (uploadDir_.Equals(ResolvedPath.Substring(0, uploadDir_.Length), StringComparison.OrdinalIgnoreCase))
                    if (Array.Exists(adminUsers_, delegate(string s) { return s.Equals(User); }))
                        return true;

            return false;
        }

#endregion
#region Event Handlers

        internal int connections_ = 0;

        void ConnectingEventHandler(wodSSHDComLIB.SSHUser User, ref wodSSHDComLIB.SSHActions Action)
        {
            if ((0 < maxConnections_) && (connections_ < maxConnections_))
            {
                Action = wodSSHDComLIB.SSHActions.Allow;
                ++connections_;
            }
            else
            {
                Debug.WriteLine("[ConnectingEventHandler] Denied due to number of connections already in progress");
                Action = wodSSHDComLIB.SSHActions.Deny;
            }
        }

        void DisconnectedEventHandler(wodSSHDComLIB.SSHUser User)
        {
            Debug.WriteLine("[DisconnectedEventHandler] User " + User.Login + " disconnected at " + User.TimeConnected.ToString());
            --connections_;
        }

        void ServiceStartEventHandler(wodSSHDComLIB.SSHUser User, int ServiceIndex, wodSSHDComLIB.SSHServiceTypes ServiceType, string ServiceName)
        {
            // log session type
            Debug.WriteLine("[ServiceStartEventHandler] User " + User.Login +
                " logged in at " + User.TimeConnected.ToString() +
                " from " + User.RemoteIP +
                " with session of type " + ServiceType);

            // set home dir
            User.HomeDir = homeDir_;

            // log out banner message for interactive sessions
            if (wodSSHDComLIB.SSHServiceTypes.stShell == ServiceType)
            {
                string FileName = System.IO.Path.Combine(AppDomain.CurrentDomain.BaseDirectory, "banner.txt");
                bool UseCannedBanner = true;

                if (System.IO.File.Exists(FileName))
                {
                    string[] lines;

                    // read lines from banner
                    try
                    {
                        lines = System.IO.File.ReadAllLines(FileName);
                        UseCannedBanner = false;

                        foreach (string l in lines)
                            User.Send(l + Environment.NewLine, ServiceIndex);
                    }
                    catch (System.Exception /*e*/)
                    {
                    }
                }

                if (UseCannedBanner)
                {
                    User.Send("You have connected to the Hobart HTx SSHD server." + Environment.NewLine, ServiceIndex);
                    User.Send(Environment.NewLine, ServiceIndex);
                    User.Send("WARNING: Unauthorized access to this system is forbidden and will be" + Environment.NewLine, ServiceIndex);
                    User.Send("prosecuted by law. By accessing this system, you agree that your actions" + Environment.NewLine, ServiceIndex);
                    User.Send("will be monitored and logged." + Environment.NewLine, ServiceIndex);
                    User.Send(Environment.NewLine, ServiceIndex);
                }

                User.Send("You are connecting from " + User.RemoteIP + Environment.NewLine, ServiceIndex);
                User.Send("You connected at " + User.TimeConnected.ToString() + Environment.NewLine, ServiceIndex);
                User.Send(Environment.NewLine, ServiceIndex);
            }
        }

        void ServiceRequestEventHandler(wodSSHDComLIB.SSHUser User, int ServiceIndex, ref wodSSHDComLIB.SSHServiceTypes ServiceType, ref string ServicePath, ref wodSSHDComLIB.SSHActions Action)
        {
            if ((wodSSHDComLIB.SSHServiceTypes.stSubsystem == ServiceType) || (wodSSHDComLIB.SSHServiceTypes.stSCP == ServiceType))
                Action = wodSSHDComLIB.SSHActions.Allow;
            else
            {
                Debug.WriteLine("[ServiceRequestEventHandler] Denied " + User.Login + " service type " + ServiceType);
                Action = wodSSHDComLIB.SSHActions.Deny;
            }
        }

        void LoginPasswordEventHandler(wodSSHDComLIB.SSHUser User, string Login, string Password, ref wodSSHDComLIB.SSHActions Action)
        {
            if (ValidatePassword(Login, Password))
                Action = wodSSHDComLIB.SSHActions.Allow;
            else
            {
                Debug.WriteLine("[LoginPasswordEventHandler] Failed login for " + Login + " (" + DateTime.Now.ToShortTimeString() + ")");
                Action = wodSSHDComLIB.SSHActions.Deny;
            }
        }

        void SftpDeleteFileEventHandler(wodSSHDComLIB.SSHUser User, string RelativePath, ref string ResolvedPath, ref wodSSHDComLIB.SSHActions Action)
        {
            if (!IsFullControlAllowed(User.Login, ResolvedPath))
                Action = wodSSHDComLIB.SSHActions.Deny;
            else
            {
                Debug.WriteLine("[SftpDeleteFileEventHandler] User " + User.Login + " deleted file '" + ResolvedPath + "' (" + DateTime.Now.ToShortTimeString() + ")");
                Action = wodSSHDComLIB.SSHActions.Allow;
            }
        }

        void SftpDownloadFileEventHandler(wodSSHDComLIB.SSHUser User, string RelativePath, ref string ResolvedPath, ref wodSSHDComLIB.SSHActions Action)
        {
            if (!IsReadOnlyControlAllowed(User.Login, ResolvedPath))
                Action = wodSSHDComLIB.SSHActions.Deny;
            else
            {
                Debug.WriteLine("[SftpDownloadFileEventHandler] User " + User.Login + " downloaded file '" + ResolvedPath + "' (" + DateTime.Now.ToShortTimeString() + ")");
                Action = wodSSHDComLIB.SSHActions.Allow;
            }
        }

        void SftpListDirEventHandler(wodSSHDComLIB.SSHUser User, string RelativePath, ref string ResolvedPath, ref wodSSHDComLIB.SSHActions Action)
        {
            if (!IsReadOnlyControlAllowed(User.Login, ResolvedPath))
                Action = wodSSHDComLIB.SSHActions.Deny;
            else
            {
                Debug.WriteLine("[SftpListDirEventHandler] User " + User.Login + " listed '" + ResolvedPath + "' (" + DateTime.Now.ToShortTimeString() + ")");
                Action = wodSSHDComLIB.SSHActions.Allow;
            }
        }

        void SftpMakeDirEventHandler(wodSSHDComLIB.SSHUser User, string RelativePath, ref string ResolvedPath, ref wodSSHDComLIB.SSHActions Action)
        {
            if (!IsFullControlAllowed(User.Login, ResolvedPath))
                Action = wodSSHDComLIB.SSHActions.Deny;
            else
            {
                Debug.WriteLine("[SftpMakeDirEventHandler] User " + User.Login + " created dir '" + ResolvedPath + "' (" + DateTime.Now.ToShortTimeString() + ")");
                Action = wodSSHDComLIB.SSHActions.Allow;
            }
        }

        void SftpRemoveDirEventHandler(wodSSHDComLIB.SSHUser User, string RelativePath, ref string ResolvedPath, ref wodSSHDComLIB.SSHActions Action)
        {
            if (!IsFullControlAllowed(User.Login, ResolvedPath))
                Action = wodSSHDComLIB.SSHActions.Deny;
            else
            {
                Debug.WriteLine("[SftpRemoveDirEventHandler] User " + User.Login + " removed dir '" + ResolvedPath + "' (" + DateTime.Now.ToShortTimeString() + ")");
                Action = wodSSHDComLIB.SSHActions.Allow;
            }
        }

        void SftpRenameEventHandler(wodSSHDComLIB.SSHUser User, string RelativePath, ref string ResolvedPath, ref string NewName, ref wodSSHDComLIB.SSHActions Action)
        {
            if (!IsFullControlAllowed(User.Login, ResolvedPath))
                Action = wodSSHDComLIB.SSHActions.Deny;
            else
            {
                Debug.WriteLine("[SftpRenameEventHandler] User " + User.Login + " renamed '" + ResolvedPath + "' to '" + NewName + "' (" + DateTime.Now.ToShortTimeString() + ")");
                Action = wodSSHDComLIB.SSHActions.Allow;
            }
        }

        void SftpUploadFileEventHandler(wodSSHDComLIB.SSHUser User, string RelativePath, ref string ResolvedPath, ref wodSSHDComLIB.SSHActions Action)
        {
            if (!IsFullControlAllowed(User.Login, ResolvedPath))
                Action = wodSSHDComLIB.SSHActions.Deny;
            else
            {
                Debug.WriteLine("[SftpUploadFileEventHandler] User " + User.Login + " uploaded file '" + ResolvedPath + "' (" + DateTime.Now.ToShortTimeString() + ")");
                Action = wodSSHDComLIB.SSHActions.Allow;
            }
        }

#endregion
    }
}
