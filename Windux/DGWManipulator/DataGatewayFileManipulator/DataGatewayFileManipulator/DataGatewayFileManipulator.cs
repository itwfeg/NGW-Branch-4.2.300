using System;
using System.Collections.Generic;
using System.Text;
using System.IO;

namespace DataGatewayFileManipulator
{
    public class DataGatewayFileManipulator
    {
        public DataGatewayFileManipulator()
        {
            //Constructor

            dgwFileLines = new List<string>();
        }

        //Function to do the the work
        public void ManipulateDGWFile()
        {
            if ( (filename != null) && (filename.Length > 0) ) //do we have a filename
            {
                StreamWriter sw = new StreamWriter(filename);
                string lineToWrite = "";

                //Loop through all lines of the saved initial file, and determine whether we need 
                //to write/manipulate the lines
                for (int i=0; i < dgwFileLines.Count; i++)
                {
                    //if line starts with OpTypeWriteOneStatus, get rid of it
                    if (dgwFileLines[i].StartsWith("OpTypeWriteOneStatus"))
                        continue;

                    //if line starts with OpTypeWriteOneStatistic, get rid of it
                    if (dgwFileLines[i].StartsWith("OpTypeWriteOneStatistic"))
                        continue;
                    
                    //check if line has Date__, 
                    //get the index of where it starts (we will be removing starting here)
                    int indexOfDate = dgwFileLines[i].IndexOf("Date__");

                    if (indexOfDate > 0) //is Date__ present?
                    {
                        //now find where ScalID is (ScalID10.3.195.143)
                        int indexOfScaleID = dgwFileLines[i].IndexOf("ScalID");

                        //we will go past ScalID (will need to add 6 to counter below so we skip ScalID as
                        //well as the ip address itself
                        string temp = dgwFileLines[i].Substring(indexOfScaleID);
                        temp = temp.Replace("ScalID", "");

                        int counter = 0; //will hold size of the scaleID actual IP address

                        //find the index of the first non digit/'.', and 
                        while ((Char.IsDigit(temp[counter])) || (temp[counter] == '.'))
                            counter++;    

                        //At this point we should be at the first character after the IP address, so add one more
                        counter++;

                        //Grab the start of the line up until "Date__"
                        lineToWrite = dgwFileLines[i].Substring(0, indexOfDate);
                        
                        //Now add to the line everything past "ScalID" (which has a length of 6!)
                        lineToWrite += dgwFileLines[i].Substring(indexOfScaleID + counter + 6);
                    }

                    //Now write the line out to the manipulated file
                    DGWManipulatedFile += lineToWrite;
                    
                    if (lineToWrite.Length > 0)
                        sw.WriteLine(lineToWrite);
                }
            
                sw.Close();
            }
        }

        public void SetAndReadDGWFile(string fileName)
        {
            //save off filename so we have it for the manipulation function
            filename = fileName;
            
            if (File.Exists(fileName))
            {
                StreamReader sr = new StreamReader(fileName);

                string currentLine = "";

                while ((currentLine = sr.ReadLine()) != null)
                {
                    dgwFileLines.Add(currentLine);
                    DGWFileToManipulate += currentLine;
                }

                sr.Close();
            }
        }

#region propertiesAndAttributes

        private string dgwFileToManipulate;

        public string DGWFileToManipulate
        {
            get { return dgwFileToManipulate; }
            set { dgwFileToManipulate = value; }
        }

        private string dgwManipulatedFile;

        public string DGWManipulatedFile
        {
            get { return dgwManipulatedFile; }
            set { dgwManipulatedFile = value; }
        }

        private string filename = null;

        private List<string> dgwFileLines;

        public List<string> GetDGWFileLines
        {
            get { return dgwFileLines; }
            set { dgwFileLines = value; }
        }

#endregion 
	
    }
}
