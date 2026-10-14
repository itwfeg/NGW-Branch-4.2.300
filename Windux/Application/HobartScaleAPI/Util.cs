/// <copyright file="Util.cs" company="Hobart">
/// Copyright (c) Hobart, a division of ITW Food Equipment Group LLC 2014
/// </copyright>
/// <author>Randy Blankley</author>
/// <date>October 2014</date>
/// <summary>Util class</summary>

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace HobartRemoteAPI
{
    /// <summary>
    /// Utility methods.
    /// </summary>
    public class Util
    {
        /// <summary>
        /// Convert DB record value to boolean.
        /// </summary>
        /// <param name="value">value to convert</param>
        /// <returns>boolean value</returns>
        public static bool ConvertToBool(string value)
        {
            if ((null == value) || ("None" == value) || (0 == value.Length))
                return false;

            return ("1" == value);
        }

        /// <summary>
        /// Convert DB record value to integer.
        /// </summary>
        /// <param name="value">value to convert</param>
        /// <returns>integer value</returns>
        public static int ConvertToInt(string value)
        {
            if ((null == value) || ("None" == value) || (0 == value.Length))
                return 0;

            return Convert.ToInt32(value);
        }

        /// <summary>
        /// Convert DB record value to string.
        /// </summary>
        /// <param name="value">value to convert</param>
        /// <returns>string value</returns>
        public static string ConvertToString(string value)
        {
            if ((null == value) || ("None" == value))
                return "";

            return value;
        }

        /// <summary>
        /// Convert DB value to date/time.
        /// </summary>
        /// <param name="value">value to convert</param>
        /// <returns>date/time value</returns>
        public static DateTime ConvertToDateTime(string value)
        {
            DateTime result = new DateTime();

            if ((null == value) || ("None" == value))
                return result;

            try
            {
                result = Convert.ToDateTime(value);
            }
            catch (Exception e)
            {
                // nop
            }

            return result;
        }
    }
}
