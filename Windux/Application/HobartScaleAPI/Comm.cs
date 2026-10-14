/// <copyright file="Comm.cs" company="Hobart">
/// Copyright (c) Hobart, a division of ITW Food Equipment Group LLC 2014
/// </copyright>
/// <author>Randy Blankley</author>
/// <date>October 2014</date>
/// <summary>Comm class</summary>

using System;
using System.Collections.Generic;
using System.Linq;
using System.Net;
using System.Net.Sockets;
using System.Text;

namespace HobartRemoteAPI
{
    /// <summary>
    /// Communication class.
    /// </summary>
    public class Comm
    {
        private TimeSpan timeout_ = new TimeSpan(0, 0, 5);

        private Socket client_;
        private bool isConnected_ = false;

        private int serialNo_ = 0;
        private int SerialNo
        {
            get { return ++serialNo_; }
        }

        /// <summary>
        /// Constructor.
        /// </summary>
        /// <param name="ip">Scale IP Address.</param>
        public Comm(IPAddress ip)
        {
            try
            {
                // connect to scale management backend port 5076
                IPEndPoint endPoint = new IPEndPoint(ip, 5076);

                // create socket
                client_ = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);

                // connect to scale
                // we wait up to timeout time (5 seconds) for the connect
                var connectResult = client_.BeginConnect(endPoint, null, null);

                if (connectResult.AsyncWaitHandle.WaitOne(timeout_, true))
                {
                    client_.EndConnect(connectResult);
                    isConnected_ = true;
                }
                else
                {
                    // timeout!
                    client_.Close();
                }
            }
            catch (Exception e)
            {
            }
        }

        /// <summary>
        /// Destructor.
        /// </summary>
        ~Comm()
        {
            try
            {
                // release socket
                if (isConnected_)
                {
                    client_.Shutdown(SocketShutdown.Both);
                    client_.Close();
                }
            }
            catch (Exception e)
            {
            }
        }

        /// <summary>
        /// Set comm timeout.
        /// </summary>
        public TimeSpan Timeout
        {
            get { return timeout_; }
            set { timeout_ = value; }
        }

        /// <summary>
        /// Check for valid connection.
        /// </summary>
        public bool IsConnected
        {
            get { return isConnected_; }
        }

        /// <summary>
        /// Execute select statement on scale.
        /// </summary>
        /// <param name="table">Table name</param>
        /// <param name="whereClause">Where clause of select statement</param>
        /// <param name="records">list of records returned from select statement</param>
        /// <returns>true if successfull, false otherwise</returns>
        public bool ExecuteSelectStatement(string table, string whereClause, ref List<string> records)
        {
            // not connected
            if ( !isConnected_ )
                return false;

            int msgSerialNo = SerialNo;

            // setup a timeout for socket receive
            client_.ReceiveTimeout = timeout_.Seconds * 1000;

            // execute a select statement on table...
            string statement = "selectStatementSynchronous|{0}|{1}|{2}\n";
            byte[] msg = Encoding.ASCII.GetBytes(String.Format(statement, table, whereClause, msgSerialNo));

            // locate embedded data
            string prefix = string.Format("selectStatement|{0}|", table);
            string suffix = string.Format("|{0}|{1}|done\n", whereClause, msgSerialNo);

            string result = "";

            try
            {
                byte[] buffer = new byte[1024 * 1024];
                int bufferSize = 0;

                // write command
                client_.Send(msg);

                // get data from scale
                for (;;)
                {
                    bufferSize = client_.Receive(buffer);

                    // no data!
                    if (0 == bufferSize)
                        return false;

                    // get our result
                    result += Encoding.UTF8.GetString(buffer, 0, bufferSize);

                    // check for done
                    if (prefix.Length + suffix.Length <= result.Length)
                        if ((prefix == result.Substring(0, prefix.Length)) && (suffix == result.Substring(result.Length - suffix.Length, suffix.Length)))
                            break;
                }
            }
            catch (Exception e)
            {
                return false;
            }

            result = result.Substring(prefix.Length, result.Length - prefix.Length - suffix.Length);

            // parse out each record from result
            List<string> needToValidate = new List<string>();
            int index;

            while (-1 < (index = result.IndexOf("][")))
            {
                needToValidate.Add(result.Substring(0, index + 1));
                result = result.Substring(index + 1);
            }

            if (0 < result.Length)
                needToValidate.Add(result);

            // validate embedded data
            foreach (string v in needToValidate)
                if (('[' != v.First()) || (']' != v.Last()))
                    return false;

            // populate result set
            records = new List<string>();

            char[] charsToTrim = { '[', ']' };

            foreach (string v in needToValidate)
                records.Add(v.Trim(charsToTrim));

            return true;
        }

    }
}
