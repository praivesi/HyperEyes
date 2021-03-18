namespace KoreanEyes_Winform
{
    partial class Form1
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
            this.tmpBtn = new System.Windows.Forms.Button();
            this.tmpPanel = new System.Windows.Forms.Panel();
            this.SuspendLayout();
            // 
            // tmpBtn
            // 
            this.tmpBtn.Location = new System.Drawing.Point(136, 69);
            this.tmpBtn.Name = "tmpBtn";
            this.tmpBtn.Size = new System.Drawing.Size(75, 23);
            this.tmpBtn.TabIndex = 0;
            this.tmpBtn.Text = "temp";
            this.tmpBtn.UseVisualStyleBackColor = true;
            // 
            // tmpPanel
            // 
            this.tmpPanel.BackColor = System.Drawing.SystemColors.ActiveCaption;
            this.tmpPanel.Dock = System.Windows.Forms.DockStyle.Bottom;
            this.tmpPanel.Location = new System.Drawing.Point(0, 180);
            this.tmpPanel.Name = "tmpPanel";
            this.tmpPanel.Size = new System.Drawing.Size(284, 81);
            this.tmpPanel.TabIndex = 1;
            // 
            // Form1
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(7F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(284, 261);
            this.Controls.Add(this.tmpPanel);
            this.Controls.Add(this.tmpBtn);
            this.Name = "Form1";
            this.Text = "Form1";
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.Button tmpBtn;
        private System.Windows.Forms.Panel tmpPanel;
    }
}

