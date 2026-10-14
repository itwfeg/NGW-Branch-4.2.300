namespace AppInstaller
{
    partial class frmMain
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
            this.components = new System.ComponentModel.Container();
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(frmMain));
            this.theStatusStrip = new System.Windows.Forms.StatusStrip();
            this.tsStatusLabel = new System.Windows.Forms.ToolStripStatusLabel();
            this.tsProgressBar = new System.Windows.Forms.ToolStripProgressBar();
            this.pbHLX = new System.Windows.Forms.PictureBox();
            this.timer1 = new System.Windows.Forms.Timer(this.components);
            this.pbAccess = new System.Windows.Forms.PictureBox();
            this.pbHobart = new System.Windows.Forms.PictureBox();
            this.theStatusStrip.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.pbHLX)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.pbAccess)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.pbHobart)).BeginInit();
            this.SuspendLayout();
            // 
            // theStatusStrip
            // 
            this.theStatusStrip.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.tsStatusLabel,
            this.tsProgressBar});
            this.theStatusStrip.Location = new System.Drawing.Point(0, 416);
            this.theStatusStrip.Name = "theStatusStrip";
            this.theStatusStrip.Size = new System.Drawing.Size(632, 30);
            this.theStatusStrip.TabIndex = 0;
            this.theStatusStrip.Text = "statusStrip1";
            // 
            // tsStatusLabel
            // 
            this.tsStatusLabel.AutoSize = false;
            this.tsStatusLabel.Font = new System.Drawing.Font("Arial", 12F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.tsStatusLabel.Name = "tsStatusLabel";
            this.tsStatusLabel.Size = new System.Drawing.Size(375, 25);
            this.tsStatusLabel.Text = "Please Wait...";
            // 
            // tsProgressBar
            // 
            this.tsProgressBar.Margin = new System.Windows.Forms.Padding(40, 3, 1, 3);
            this.tsProgressBar.Name = "tsProgressBar";
            this.tsProgressBar.Size = new System.Drawing.Size(200, 24);
            // 
            // timer1
            // 
            this.timer1.Interval = 2000;
            this.timer1.Tick += new System.EventHandler(this.timer1_Tick);
            // 
            // pbHLX
            // 
            this.pbHLX.BackColor = System.Drawing.Color.White;
            this.pbHLX.Dock = System.Windows.Forms.DockStyle.Fill;
            this.pbHLX.Image = ((System.Drawing.Image)(resources.GetObject("pbHLX.Image")));
            this.pbHLX.Location = new System.Drawing.Point(0, 0);
            this.pbHLX.Name = "pbHLX";
            this.pbHLX.Size = new System.Drawing.Size(632, 446);
            this.pbHLX.SizeMode = System.Windows.Forms.PictureBoxSizeMode.CenterImage;
            this.pbHLX.TabIndex = 2;
            this.pbHLX.TabStop = false;
            this.pbHLX.Visible = false;
            // 
            // pbAccess
            // 
            this.pbAccess.BackColor = System.Drawing.Color.White;
            this.pbAccess.Dock = System.Windows.Forms.DockStyle.Fill;
            this.pbAccess.Image = ((System.Drawing.Image)(resources.GetObject("pbAccess.Image")));
            this.pbAccess.Location = new System.Drawing.Point(0, 0);
            this.pbAccess.Name = "pbAccess";
            this.pbAccess.Size = new System.Drawing.Size(632, 446);
            this.pbAccess.SizeMode = System.Windows.Forms.PictureBoxSizeMode.CenterImage;
            this.pbAccess.TabIndex = 3;
            this.pbAccess.TabStop = false;
            this.pbAccess.Visible = false;
            // 
            // pbHobart
            // 
            this.pbHobart.BackColor = System.Drawing.Color.White;
            this.pbHobart.Dock = System.Windows.Forms.DockStyle.Fill;
            this.pbHobart.Image = ((System.Drawing.Image)(resources.GetObject("pbHobart.Image")));
            this.pbHobart.Location = new System.Drawing.Point(0, 0);
            this.pbHobart.Name = "pbHobart";
            this.pbHobart.Size = new System.Drawing.Size(632, 446);
            this.pbHobart.SizeMode = System.Windows.Forms.PictureBoxSizeMode.CenterImage;
            this.pbHobart.TabIndex = 4;
            this.pbHobart.TabStop = false;
            this.pbHobart.Visible = false;
            // 
            // frmMain
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.AutoSize = true;
            this.ClientSize = new System.Drawing.Size(632, 446);
            this.ControlBox = false;
            this.Controls.Add(this.theStatusStrip);
            this.Controls.Add(this.pbHobart);
            this.Controls.Add(this.pbAccess);
            this.Controls.Add(this.pbHLX);
            this.ImeMode = System.Windows.Forms.ImeMode.Disable;
            this.MaximizeBox = false;
            this.MinimizeBox = false;
            this.Name = "frmMain";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
            this.Text = "Hobart AppInstaller";
            this.TopMost = true;
            this.Load += new System.EventHandler(this.frmMain_Load);
            this.theStatusStrip.ResumeLayout(false);
            this.theStatusStrip.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.pbHLX)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.pbAccess)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.pbHobart)).EndInit();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.StatusStrip theStatusStrip;
        private System.Windows.Forms.ToolStripStatusLabel tsStatusLabel;
        private System.Windows.Forms.PictureBox pbHLX;
        private System.Windows.Forms.ToolStripProgressBar tsProgressBar;
        private System.Windows.Forms.Timer timer1;
        private System.Windows.Forms.PictureBox pbAccess;
        private System.Windows.Forms.PictureBox pbHobart;
    }
}

