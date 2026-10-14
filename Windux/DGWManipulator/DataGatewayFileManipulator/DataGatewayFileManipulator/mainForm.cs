using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

namespace DataGatewayFileManipulator
{
    public partial class mainForm : Form
    {
        public mainForm()
        {
            InitializeComponent();
        }

        //We need a manipulator class instantiated
        private DataGatewayFileManipulator myManipulator = new DataGatewayFileManipulator();

        private void chooseFileButton_Click(object sender, EventArgs e)
        {
            OpenFileDialog newDialog = new OpenFileDialog();
            newDialog.InitialDirectory = Application.ExecutablePath;
            newDialog.Title = "Please select a data gateweigh file to manipulate!";
            newDialog.Filter = "STD Files|*.std";

            if (newDialog.ShowDialog() == DialogResult.OK)
            {
                myManipulator.SetAndReadDGWFile(newDialog.FileName);
                dgwFileTextBox.Lines = myManipulator.GetDGWFileLines.ToArray();
            }
        }

        private void manipulateFileButton_Click(object sender, EventArgs e)
        {
            myManipulator.ManipulateDGWFile();

            string manipulatedFile = myManipulator.DGWManipulatedFile;

            if ( (manipulatedFile != null) && (manipulatedFile.Length > 0) )
            {
                dgwFileTextBox.Text = manipulatedFile;
                MessageBox.Show("DGW File has been manipulated and overwritten!!", "Manipulation Complete");
            }
        }

        private void closeButton_Click(object sender, EventArgs e)
        {
            Application.Exit();
        }

        private void mainForm_Load(object sender, EventArgs e)
        {
            //Nothing to do for now  
        }
    }
}