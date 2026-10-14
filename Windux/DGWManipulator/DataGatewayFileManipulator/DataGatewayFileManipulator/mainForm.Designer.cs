namespace DataGatewayFileManipulator
{
    partial class mainForm
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.chooseFileButton = new System.Windows.Forms.Button();
            this.dgwFileTextBox = new System.Windows.Forms.TextBox();
            this.manipulateFileButton = new System.Windows.Forms.Button();
            this.closeButton = new System.Windows.Forms.Button();
            this.SuspendLayout();
            // 
            // chooseFileButton
            // 
            this.chooseFileButton.Font = new System.Drawing.Font("Microsoft Sans Serif", 10F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.chooseFileButton.Location = new System.Drawing.Point(102, 633);
            this.chooseFileButton.Name = "chooseFileButton";
            this.chooseFileButton.Size = new System.Drawing.Size(213, 39);
            this.chooseFileButton.TabIndex = 0;
            this.chooseFileButton.Text = "Choose File To Manipulate";
            this.chooseFileButton.UseVisualStyleBackColor = true;
            this.chooseFileButton.Click += new System.EventHandler(this.chooseFileButton_Click);
            // 
            // dgwFileTextBox
            // 
            this.dgwFileTextBox.AcceptsReturn = true;
            this.dgwFileTextBox.AcceptsTab = true;
            this.dgwFileTextBox.BackColor = System.Drawing.SystemColors.Window;
            this.dgwFileTextBox.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.dgwFileTextBox.Location = new System.Drawing.Point(28, 29);
            this.dgwFileTextBox.MaxLength = 0;
            this.dgwFileTextBox.Multiline = true;
            this.dgwFileTextBox.Name = "dgwFileTextBox";
            this.dgwFileTextBox.ReadOnly = true;
            this.dgwFileTextBox.ScrollBars = System.Windows.Forms.ScrollBars.Both;
            this.dgwFileTextBox.Size = new System.Drawing.Size(765, 587);
            this.dgwFileTextBox.TabIndex = 1;
            this.dgwFileTextBox.WordWrap = false;
            // 
            // manipulateFileButton
            // 
            this.manipulateFileButton.Font = new System.Drawing.Font("Microsoft Sans Serif", 10F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.manipulateFileButton.Location = new System.Drawing.Point(321, 633);
            this.manipulateFileButton.Name = "manipulateFileButton";
            this.manipulateFileButton.Size = new System.Drawing.Size(191, 39);
            this.manipulateFileButton.TabIndex = 2;
            this.manipulateFileButton.Text = "Manipulate File";
            this.manipulateFileButton.UseVisualStyleBackColor = true;
            this.manipulateFileButton.Click += new System.EventHandler(this.manipulateFileButton_Click);
            // 
            // closeButton
            // 
            this.closeButton.Font = new System.Drawing.Font("Microsoft Sans Serif", 10F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.closeButton.Location = new System.Drawing.Point(518, 633);
            this.closeButton.Name = "closeButton";
            this.closeButton.Size = new System.Drawing.Size(191, 39);
            this.closeButton.TabIndex = 3;
            this.closeButton.Text = "Close";
            this.closeButton.UseVisualStyleBackColor = true;
            this.closeButton.Click += new System.EventHandler(this.closeButton_Click);
            // 
            // mainForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(814, 684);
            this.ControlBox = false;
            this.Controls.Add(this.closeButton);
            this.Controls.Add(this.manipulateFileButton);
            this.Controls.Add(this.dgwFileTextBox);
            this.Controls.Add(this.chooseFileButton);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog;
            this.MaximizeBox = false;
            this.MinimizeBox = false;
            this.Name = "mainForm";
            this.ShowInTaskbar = false;
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
            this.Text = "Data Gateway File Manipulator";
            this.Load += new System.EventHandler(this.mainForm_Load);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Button chooseFileButton;
        private System.Windows.Forms.TextBox dgwFileTextBox;
        private System.Windows.Forms.Button manipulateFileButton;
        private System.Windows.Forms.Button closeButton;

    }
}

