using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.IO;

namespace SetupUtility
{
    public partial class Form1 : Form
    {
        public string[] RemovableDriveList = new string[20];      
        public Form1()
        {
  
            RemovableDriveList = new string[20];
            InitializeComponent();
            this.wiredDNSManualRadio.Checked = true;
            this.wiredIPStaticRadio.Checked = true;
            this.createSettingsFileDriveChoicePanel.Hide();
            DataGridViewColumn column0 = this.driveGridView.Columns[0];
            column0.Width = 260;
            DataGridViewColumn column1 = this.driveGridView.Columns[1];
            column0.Width = 100;
            DataGridViewColumn column2 = this.driveGridView.Columns[2];
            column0.Width = 100;
            this.openPanel.Hide();
            this.linkRateAutoCheck.Checked = true;
            this.linkrateCombo.SelectedIndex = 0;
            this.channelCombo.SelectedIndex = 0;
            this.securityMethodCombo.SelectedIndex = 0;
            this.encryptionCombo.SelectedIndex = 0;
            //this.driveGridView.HeaderRow.Cells[0].Attributes["Width"] = "150px";
            //this.driveGridView.HeaderRow.Cells[0].Attributes["Width"] = "150px";
        }

        private void tabPage5_Click(object sender, EventArgs e)
        {

        }

        private void label2_Click(object sender, EventArgs e)
        {

        }

        private void groupBox2_Enter(object sender, EventArgs e)
        {

        }

        private void groupBox1_Enter(object sender, EventArgs e)
        {

        }

        private void radioButton1_CheckedChanged(object sender, EventArgs e)
        {
            if (this.wiredIPDHCPRadio.Checked == true) this.wiredDNSAutomaticRadio.Enabled = true;
            else
            {
                this.wiredDNSAutomaticRadio.Enabled = false;
                this.wiredDNSManualRadio.Checked = true;
            }
        }

        private void radioButton2_CheckedChanged(object sender, EventArgs e)
        {

        }

        private void label1_Click(object sender, EventArgs e)
        {

        }

        private string validateOctet(string inOctet)
        {
            //removes alpha chars and numbers > 255
            //round robin rollin from the right
            string newVal = "";
            bool stripped = false;
            /*if (inOctet.Length == 1)
            {
                foreach (var inputChar in inOctet)
                {
                    if (Char.IsLetter(inputChar))
                    {
                        newVal = "";
                        stripped = true;
                    }
                }
            }
           */
            if (inOctet.Length > 3)
            {
               foreach (var inputChar in inOctet)
	            {
	                if (Char.IsLetter(inputChar))
	                {
	                    newVal = inOctet.Substring(0, inOctet.Length - 1);
                        stripped = true;
                    }
	            }
                if(!stripped) newVal = inOctet.Substring(1, inOctet.Length-1);
            }
            else newVal = inOctet;
            int parsedInt = 0;
            if (int.TryParse(newVal, out parsedInt))
            {
                if (parsedInt > 255)
                {
                    newVal = newVal.Substring(1, newVal.Length - 1);
                }
            }
            else
            {
                newVal = newVal.Substring(0, newVal.Length - 1);
            }

            if (newVal.Length > 1 && newVal.Substring(0, 1) == "0") newVal = newVal.Substring(1, newVal.Length - 1);
        
            return newVal;
        }

        private void wiredGateway1_TextChanged(object sender, EventArgs e)
        {
            this.wiredGateway1.Text = validateOctet(this.wiredGateway1.Text);
            this.wiredGateway1.Select(this.wiredGateway1.Text.Length, 0);
        }

        private void wiredGateway1_CursorChanged(object sender, EventArgs e)
        {
            this.wiredGateway1.Select(this.wiredGateway1.Text.Length, 0);
        }

        private void radioButton4_CheckedChanged(object sender, EventArgs e)
        {

        }

        private void tabPage1_Click(object sender, EventArgs e)
        {

        }

        private void textBox2_TextChanged(object sender, EventArgs e)
        {
            this.wiredGateway2.Select(this.wiredGateway2.Text.Length, 0);
            this.wiredGateway2.Text = validateOctet(this.wiredGateway2.Text);
      
        }

        private void wiredGateway3_TextChanged(object sender, EventArgs e)
        {
            this.wiredGateway3.Select(this.wiredGateway3.Text.Length, 0);
            this.wiredGateway3.Text = validateOctet(this.wiredGateway3.Text);

        }

        private void wiredGateway4_TextChanged(object sender, EventArgs e)
        {
            this.wiredGateway4.Select(this.wiredGateway4.Text.Length, 0);
            this.wiredGateway4.Text = validateOctet(this.wiredGateway4.Text);
        }

        private void wiredSubnet1_TextChanged(object sender, EventArgs e)
        {
            this.wiredSubnet1.Select(this.wiredSubnet1.Text.Length, 0);
            this.wiredSubnet1.Text = validateOctet(this.wiredSubnet1.Text);

        }

        private void wiredSubnet2_TextChanged(object sender, EventArgs e)
        {
            this.wiredSubnet2.Select(this.wiredSubnet2.Text.Length, 0);
            this.wiredSubnet2.Text = validateOctet(this.wiredSubnet2.Text);

        }

        private void wiredSubnet3_TextChanged(object sender, EventArgs e)
        {
            this.wiredSubnet3.Select(this.wiredSubnet3.Text.Length, 0);
            this.wiredSubnet3.Text = validateOctet(this.wiredSubnet3.Text);

        }

        private void wiredSubnet4_TextChanged(object sender, EventArgs e)
        {
            this.wiredSubnet4.Select(this.wiredSubnet4.Text.Length, 0);
            this.wiredSubnet4.Text = validateOctet(this.wiredSubnet4.Text);

        }

        private void wiredIPAddress1_TextChanged(object sender, EventArgs e)
        {
            this.wiredIPAddress1.Select(this.wiredIPAddress1.Text.Length, 0);
            this.wiredIPAddress1.Text = validateOctet(this.wiredIPAddress1.Text);

        }

        private void wiredIPAddress2_TextChanged(object sender, EventArgs e)
        {
            this.wiredIPAddress2.Select(this.wiredIPAddress2.Text.Length, 0);
            this.wiredIPAddress2.Text = validateOctet(this.wiredIPAddress2.Text);

        }

        private void wiredIPAddress3_TextChanged(object sender, EventArgs e)
        {
            this.wiredIPAddress3.Select(this.wiredIPAddress3.Text.Length, 0);
            this.wiredIPAddress3.Text = validateOctet(this.wiredIPAddress3.Text);

        }

        private void wiredIPAddress4_TextChanged(object sender, EventArgs e)
        {
            this.wiredIPAddress4.Select(this.wiredIPAddress4.Text.Length, 0);
            this.wiredIPAddress4.Text = validateOctet(this.wiredIPAddress4.Text);

        }

        private void wiredDNS1st1_TextChanged(object sender, EventArgs e)
        {
            this.wiredDNS1st1.Select(this.wiredDNS1st1.Text.Length, 0);
            this.wiredDNS1st1.Text = validateOctet(this.wiredDNS1st1.Text);

        }

        private void wiredDNS1st2_TextChanged(object sender, EventArgs e)
        {
            this.wiredDNS1st2.Select(this.wiredDNS1st2.Text.Length, 0);
            this.wiredDNS1st2.Text = validateOctet(this.wiredDNS1st2.Text);

        }

        private void wiredDNS1st3_TextChanged(object sender, EventArgs e)
        {
            this.wiredDNS1st3.Select(this.wiredDNS1st3.Text.Length, 0);
            this.wiredDNS1st3.Text = validateOctet(this.wiredDNS1st3.Text);

        }

        private void wiredDNS1st4_TextChanged(object sender, EventArgs e)
        {
            this.wiredDNS1st4.Select(this.wiredDNS1st4.Text.Length, 0);
            this.wiredDNS1st4.Text = validateOctet(this.wiredDNS1st4.Text);

        }

        private void wiredDNS2nd1_TextChanged(object sender, EventArgs e)
        {
            this.wiredDNS2nd1.Select(this.wiredDNS2nd1.Text.Length, 0);
            this.wiredDNS2nd1.Text = validateOctet(this.wiredDNS2nd1.Text);

        }

        private void wiredDNS2nd2_TextChanged(object sender, EventArgs e)
        {
            this.wiredDNS2nd2.Select(this.wiredDNS2nd2.Text.Length, 0);
            this.wiredDNS2nd2.Text = validateOctet(this.wiredDNS2nd2.Text);

        }

        private void wiredDNS2nd3_TextChanged(object sender, EventArgs e)
        {
            this.wiredDNS2nd3.Select(this.wiredDNS2nd3.Text.Length, 0);
            this.wiredDNS2nd3.Text = validateOctet(this.wiredDNS2nd3.Text);

        }

        private void wiredDNS2nd4_TextChanged(object sender, EventArgs e)
        {
            this.wiredDNS2nd4.Select(this.wiredDNS2nd4.Text.Length, 0);
            this.wiredDNS2nd4.Text = validateOctet(this.wiredDNS2nd4.Text);

        }

        private void wiredDNS3rd1_TextChanged(object sender, EventArgs e)
        {
            this.wiredDNS3rd1.Select(this.wiredDNS3rd1.Text.Length, 0);
            this.wiredDNS3rd1.Text = validateOctet(this.wiredDNS3rd1.Text);

        }

        private void wiredDNS3rd2_TextChanged(object sender, EventArgs e)
        {
            this.wiredDNS3rd2.Select(this.wiredDNS3rd2.Text.Length, 0);
            this.wiredDNS3rd2.Text = validateOctet(this.wiredDNS3rd2.Text);

        }

        private void wiredDNS3rd3_TextChanged(object sender, EventArgs e)
        {
            this.wiredDNS3rd3.Select(this.wiredDNS3rd3.Text.Length, 0);
            this.wiredDNS3rd3.Text = validateOctet(this.wiredDNS3rd3.Text);

        }

        private void wiredDNS3rd4_TextChanged(object sender, EventArgs e)
        {
            this.wiredDNS3rd4.Select(this.wiredDNS3rd4.Text.Length, 0);
            this.wiredDNS3rd4.Text = validateOctet(this.wiredDNS3rd4.Text);

        }

        private void wiredGateway2_Click(object sender, EventArgs e)
        {
            this.wiredGateway2.Select(this.wiredGateway2.Text.Length, 0);
  
        }

        private void wiredGateway3_Click(object sender, EventArgs e)
        {
            this.wiredGateway3.Select(this.wiredGateway3.Text.Length, 0);
  
        }

        private void wiredGateway4_Click(object sender, EventArgs e)
        {
            this.wiredGateway4.Select(this.wiredGateway4.Text.Length, 0);
  
        }

        private void wiredSubnet1_Click(object sender, EventArgs e)
        {
            this.wiredSubnet1.Select(this.wiredSubnet1.Text.Length, 0);
  
        }

        private void wiredSubnet2_Click(object sender, EventArgs e)
        {
            this.wiredSubnet2.Select(this.wiredSubnet2.Text.Length, 0);
  
        }

        private void wiredSubnet3_Click(object sender, EventArgs e)
        {
            this.wiredSubnet3.Select(this.wiredSubnet3.Text.Length, 0);
  
        }

        private void wiredSubnet4_Click(object sender, EventArgs e)
        {
            this.wiredSubnet4.Select(this.wiredSubnet4.Text.Length, 0);
  
        }

        private void wiredIPAddress1_Click(object sender, EventArgs e)
        {
            this.wiredIPAddress1.Select(this.wiredIPAddress1.Text.Length, 0);
  
        }

        private void wiredIPAddress2_Click(object sender, EventArgs e)
        {
            this.wiredIPAddress2.Select(this.wiredIPAddress2.Text.Length, 0);
 
        }

        private void wiredIPAddress3_Click(object sender, EventArgs e)
        {
            this.wiredIPAddress3.Select(this.wiredIPAddress3.Text.Length, 0);
 
        }

        private void wiredIPAddress4_Click(object sender, EventArgs e)
        {
            this.wiredIPAddress4.Select(this.wiredIPAddress4.Text.Length, 0);
 
        }

        private void wiredDNS1st1_Click(object sender, EventArgs e)
        {
            this.wiredDNS1st1.Select(this.wiredDNS1st1.Text.Length, 0);
 
        }

        private void wiredDNS1st2_Click(object sender, EventArgs e)
        {
            this.wiredDNS1st2.Select(this.wiredDNS1st2.Text.Length, 0);

        }

        private void wiredDNS1st3_Click(object sender, EventArgs e)
        {
            this.wiredDNS1st3.Select(this.wiredDNS1st3.Text.Length, 0);

        }

        private void wiredDNS1st4_Click(object sender, EventArgs e)
        {
            this.wiredDNS1st4.Select(this.wiredDNS1st4.Text.Length, 0);

        }

        private void wiredDNS2nd1_Click(object sender, EventArgs e)
        {
            this.wiredDNS2nd1.Select(this.wiredDNS2nd1.Text.Length, 0);

        }

        private void wiredDNS2nd2_Click(object sender, EventArgs e)
        {
            this.wiredDNS2nd2.Select(this.wiredDNS2nd2.Text.Length, 0);

        }

        private void wiredDNS2nd3_Click(object sender, EventArgs e)
        {
            this.wiredDNS2nd3.Select(this.wiredDNS2nd3.Text.Length, 0);

        }

        private void wiredDNS2nd4_Click(object sender, EventArgs e)
        {
            this.wiredDNS2nd4.Select(this.wiredDNS2nd4.Text.Length, 0);

        }

        private void wiredDNS3rd1_Click(object sender, EventArgs e)
        {
            this.wiredDNS3rd1.Select(this.wiredDNS3rd1.Text.Length, 0);

        }

        private void wiredDNS3rd2_Click(object sender, EventArgs e)
        {
            this.wiredDNS3rd2.Select(this.wiredDNS3rd2.Text.Length, 0);

        }

        private void wiredDNS3rd3_Click(object sender, EventArgs e)
        {
            this.wiredDNS3rd3.Select(this.wiredDNS3rd3.Text.Length, 0);

        }

        private void wiredDNS3rd4_Click(object sender, EventArgs e)
        {
            this.wiredDNS3rd4.Select(this.wiredDNS3rd4.Text.Length, 0);

        }

        public static string FormatBytesToHumanReadable(long bytes)
        {
            if (bytes > 1073741824)
                return Math.Ceiling(bytes / 1073741824M).ToString("#,### GB");
            else if (bytes > 1048576)
                return Math.Ceiling(bytes / 1048576M).ToString("#,### MB");
            else if (bytes >= 1)
                return Math.Ceiling(bytes / 1024M).ToString("#,### KB");
            else if (bytes < 0)
                return "";
            else
                return bytes.ToString("#,### B");
        }

        private void createSettingsButton_Click(object sender, EventArgs e)
        {
            this.createSettingsFileDriveChoicePanel.Show();
            var driveList = DriveInfo.GetDrives();

            int i = 0;
            foreach (DriveInfo drive in driveList)
            {
                if (drive.DriveType == DriveType.Removable)
                {
                    this.driveGridView.Rows.Add(drive.Name + drive.VolumeLabel, drive.DriveFormat, FormatBytesToHumanReadable(drive.TotalSize));
                    RemovableDriveList[i] = drive.Name;
                    i++;
                }
            }
            this.createSettingsFileDriveChoicePanel.BringToFront();
        }

        private void CheckIfFileHasCorrectExtension(object sender, CancelEventArgs e)
        {
            SaveFileDialog sv = (sender as SaveFileDialog);
            if (sv.FileName != "systemConfig.hsu")
            {
                sv.FileName = "setupUtility.hsu";
                e.Cancel = true;
                MessageBox.Show("filename is restricted to setupUtility.hsu");
                return;
            }

        }

        private void folderBrowserDialog1_HelpRequest(object sender, EventArgs e)
        {

        }

        private void label26_Click(object sender, EventArgs e)
        {

        }

        private void button2_Click(object sender, EventArgs e)
        {
            foreach (DataGridViewRow row in this.driveGridView.Rows)
            {
                //if (!row.IsNewRow)
                    this.driveGridView.Rows.Remove(row);
             
            }
            foreach (DataGridViewRow row in this.driveGridView.Rows)
            {
                //if (!row.IsNewRow)
                this.driveGridView.Rows.Remove(row);

            }
            this.createSettingsFileDriveChoicePanel.Hide();
        }

        private void driveGridView_CellContentClick(object sender, DataGridViewCellEventArgs e)
        {

        }

        private void createFileButton_Click(object sender, EventArgs e)
        {
            // start making the hsu file
            //what is row selection
            int rowSelected = this.driveGridView.SelectedRows[0].Index;
            this.debugBox.AppendText(rowSelected.ToString());

            string drive;
            drive = RemovableDriveList[rowSelected];
            string dir = drive + "hobart";
            string filePath = dir + "\\setupUtility.hsu";
            if (!Directory.Exists(dir))
            {
                Directory.CreateDirectory(dir);
            }


                using(StreamWriter writetext = new StreamWriter(filePath))
                {    // write the file now
                    
                    //SSID : 
                    // EXAMPLE saveConfigs.txt
                    // -S-MOPEN-EWEP-K1-w0-x0-y0-z0-s1-I10.3.176.67
                    // -B255.255.248.0-G10.3.176.200-D0-110.3.72.151
                    // -210.3.72.152-364641964-eauto-f1-gauto-5-6-7-8-9
                    // -4-Zwoozal-L
                    string option = "";
                    option = "-S" + this.ssidBox.Text;
                    writetext.WriteLine(option);
                    option = "-M" + this.securityMethodCombo.SelectedItem.ToString();
                    writetext.WriteLine(option);
                    option = "-E" + this.encryptionCombo.SelectedItem.ToString();
                    writetext.WriteLine(option);
                    option = "-K";
                    if (this.key1Radio.Checked) option += "1";
                    else if (this.key2Radio.Checked) option += "2";
                    else if (this.key3Radio.Checked) option += "3";
                    else if (this.key4Radio.Checked) option += "4";
                    writetext.WriteLine(option);
                    option = "-w";
                    if (this.key1Box.Text == "") option += "0";
                    else option += this.key1Box.Text;
                    writetext.WriteLine(option);
                    option = "-x";
                    if (this.key2Box.Text == "") option += "0";
                    else option += this.key2Box.Text;
                    writetext.WriteLine(option);
                    option = "-y";
                    if (this.key3Box.Text == "") option += "0";
                    else option += this.key3Box.Text;
                    writetext.WriteLine(option);
                    option = "-z";
                    if (this.key4Box.Text == "") option += "0";
                    else option += this.key4Box.Text;
                    writetext.WriteLine(option);

                    
                    //writetext.WriteLine(unencryptedfile);
                    string IPaddress = this.wiredIPAddress1.Text + "." + this.wiredIPAddress2.Text + "." + this.wiredIPAddress3.Text + "." + this.wiredIPAddress4.Text;
                    string Subnet = this.wiredSubnet1.Text + "." + this.wiredSubnet2.Text + "." + this.wiredSubnet3.Text + "." + this.wiredSubnet4.Text;
                    string Gateway = this.wiredGateway1.Text + "." + this.wiredGateway2.Text + "." + this.wiredGateway3.Text + "." + this.wiredGateway4.Text;
                    string DNS1 = this.wiredDNS1st1.Text + "." + this.wiredDNS1st2.Text + "." + this.wiredDNS1st3.Text + "." + this.wiredDNS1st4.Text;
                    string DNS2 = this.wiredDNS2nd1.Text + "." + this.wiredDNS2nd2.Text + "." + this.wiredDNS2nd3.Text + "." + this.wiredDNS1st4.Text;
                    string DNS3 = this.wiredDNS3rd1.Text + "." + this.wiredDNS3rd2.Text + "." + this.wiredDNS3rd3.Text + "." + this.wiredDNS1st4.Text;
                    string StaticOrDHCP = "1";
                    if (this.wiredIPDHCPRadio.Checked)
                        StaticOrDHCP = "0";
                    string AutomaticOrManual = "0";
                    if (this.wiredDNSAutomaticRadio.Checked)
                        AutomaticOrManual = "1";
                    option = "-s" + StaticOrDHCP;
                    writetext.WriteLine(option);
                    option = "-I" + IPaddress;
                    writetext.WriteLine(option);
                    option = "-B" + Subnet;
                    writetext.WriteLine(option);
                    option = "-G" + Gateway;
                    writetext.WriteLine(option);
                    option = "-D" + AutomaticOrManual;
                    writetext.WriteLine(option);
                    if (DNS1 == "...") DNS1 = "64641964";
                    option = "-1" + DNS1;
                    writetext.WriteLine(option);
                    if (DNS2 == "...") DNS2 = "64641964";
                    option = "-2" + DNS2;
                    writetext.WriteLine(option);
                    if (DNS3 == "...") DNS3 = "64641964";
                    option = "-3" + DNS3;
                    writetext.WriteLine(option);

                    //linkrate
                    option = "-e" + this.linkrateCombo.SelectedItem.ToString();
                    writetext.WriteLine(option);
                    //auto linkrate
                    option = "-f";
                    if (this.linkRateAutoCheck.Checked) option += "1";
                    else option += "0";
                    writetext.WriteLine(option);
                    //channel
                    option += "-g" + this.channelCombo.SelectedItem.ToString();
                    writetext.WriteLine(option);
                    

                    //AUTHENTICATION : OPEN
                    //CIPHER : WEP
                    //DEFAULT : 1
                    //KEY 1 : 0
                    //KEY 2 : 0
                    //KEY 3 : 0
                    //KEY 4 : 0
                    //LINKRATE : auto
                    //AUTOMATICLINKRATE : 1
                    //CHANNEL : auto
                    //ISSTATIC : 1
                    //IPADDRESS : 10.3.176.67
                    //SUBNET : 255.255.248.0
                    //GATEWAY : 10.3.176.200
                    //AUTODNS : 0
                    //PRIMARY : 10.3.72.151
                    //SECONDARY : 10.3.72.152
                    //TERTIARY : 64641964
                    //DNSSUFFIX1 : 
                    //DNSSUFFIX2 : 
                    //DNSSUFFIX3 : 
                    //DNSSUFFIX4 : 
                    //DNSSUFFIX5 : 
                    //DNSSUFFIX6 : 

                    ;
                    /*//Test code
                    for (byte i = 0; i < 100; i++)
                    {
                        fs.WriteByte(i);
                    }
                     */
                }
            
        }

        private void label29_Click(object sender, EventArgs e)
        {

        }

        private void comboBox2_SelectedIndexChanged(object sender, EventArgs e)
        {

        }

        private void securityMethodCombo_SelectedIndexChanged(object sender, EventArgs e)
        {

        }

        private void securityMethodCombo_SelectedIndexChanged_1(object sender, EventArgs e)
        {
            this.openPanel.Hide();
            this.WPAPersonalPanel.Hide();
            this.EAPPanel.Hide();
            if (this.securityMethodCombo.SelectedIndex >= 1 && this.securityMethodCombo.SelectedIndex <= 8)
            {
                this.encryptionCombo.Items.Clear();
                this.encryptionCombo.Items.Add("AES");
                this.encryptionCombo.Items.Add("TKIP");
            }
            else if (this.securityMethodCombo.SelectedIndex == 0)
            {
                this.encryptionCombo.Items.Clear();
                this.encryptionCombo.Items.Add("WEP");
                this.encryptionCombo.Items.Add("None");
            }
            else if (this.securityMethodCombo.SelectedIndex >= 9)
            {
                this.encryptionCombo.Items.Clear();
                this.encryptionCombo.Items.Add("WEP");
            }
            if (this.securityMethodCombo.SelectedIndex == 0)
            {
                this.openPanel.Show();
            }
            else if (this.securityMethodCombo.SelectedIndex == 1 || this.securityMethodCombo.SelectedIndex == 5)
            {
                this.WPAPersonalPanel.Show();
            }
            else if (this.securityMethodCombo.SelectedIndex == 3 || this.securityMethodCombo.SelectedIndex == 4 || this.securityMethodCombo.SelectedIndex >= 7)
            {
                this.EAPPanel.Show();
            }


            this.encryptionCombo.SelectedIndex = 0;
            
        }

        private void label36_Click(object sender, EventArgs e)
        {

        }

        private void label37_Click(object sender, EventArgs e)
        {

        }

        private void button1_Click(object sender, EventArgs e)
        {
            this.advancedPanel.BringToFront();
            this.advancedPanel.Show();
        }

        private void button4_Click(object sender, EventArgs e)
        {
            this.dnsSuffix1Box.Text = "";
            this.dnsSuffix2Box.Text = "";
            this.dnsSuffix3Box.Text = "";
            this.dnsSuffix4Box.Text = "";
            this.dnsSuffix5Box.Text = "";
            this.dnsSuffix6Box.Text = "";
            this.linkrateCombo.SelectedIndex = 0;
            this.channelCombo.SelectedIndex = 0;
            this.linkRateAutoCheck.Checked = true;
        }

        private void applyBut_Click(object sender, EventArgs e)
        {
            this.advancedPanel.SendToBack();
            this.advancedPanel.Hide();
        }

        private void button3_Click(object sender, EventArgs e)
        {
            this.advancedPanel.SendToBack();
            this.advancedPanel.Hide();
        }

    }
}
