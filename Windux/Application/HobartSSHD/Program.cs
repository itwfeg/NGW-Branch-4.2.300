using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.ServiceProcess;
using System.Text;

namespace HobartSSHD
{
    static class Program
    {
        static FileStream log_ = new FileStream(Path.Combine(Path.GetTempPath(), "HobartSSHD.log"), FileMode.Append, FileAccess.Write, FileShare.Read);
        static TextWriterTraceListener listener_ = new TextWriterTraceListener(log_);

        /// <summary>
        /// The main entry point for the application.
        /// </summary>
        static void Main()
        {
            ServiceBase[] ServicesToRun;

            // setup debugging
            Debug.AutoFlush = true;
            Debug.Listeners.Add(listener_);

            // run the service
            try
            {
                Debug.WriteLine("================================================================================");

                Debug.WriteLine("[Main] Creating service...");
                ServicesToRun = new ServiceBase[] { new HobartSSHD.Service() };

                Debug.WriteLine("[Main] Running service...");
                ServiceBase.Run(ServicesToRun);
            }
            catch (System.Exception e)
            {
                Debug.WriteLine("[Main] Exception initializing Hobart SSHD!" + Environment.NewLine + e.ToString());
            }
        }
    }
}
