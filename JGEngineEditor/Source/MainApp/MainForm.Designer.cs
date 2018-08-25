namespace JGEngineEditor.Source.MainApp
{
    partial class MainForm
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
            this.RenderBox = new System.Windows.Forms.PictureBox();
            this.DebugTextBox = new System.Windows.Forms.TextBox();
            this.LogBox = new System.Windows.Forms.RichTextBox();
            ((System.ComponentModel.ISupportInitialize)(this.RenderBox)).BeginInit();
            this.SuspendLayout();
            // 
            // RenderBox
            // 
            this.RenderBox.Location = new System.Drawing.Point(100, 100);
            this.RenderBox.Name = "RenderBox";
            this.RenderBox.Size = new System.Drawing.Size(800, 600);
            this.RenderBox.SizeMode = System.Windows.Forms.PictureBoxSizeMode.AutoSize;
            this.RenderBox.TabIndex = 0;
            this.RenderBox.TabStop = false;
            // 
            // DebugTextBox
            // 
            this.DebugTextBox.Location = new System.Drawing.Point(25, 23);
            this.DebugTextBox.Name = "DebugTextBox";
            this.DebugTextBox.Size = new System.Drawing.Size(719, 25);
            this.DebugTextBox.TabIndex = 1;
            // 
            // LogBox
            // 
            this.LogBox.BackColor = System.Drawing.SystemColors.ScrollBar;
            this.LogBox.BorderStyle = System.Windows.Forms.BorderStyle.None;
            this.LogBox.Location = new System.Drawing.Point(1057, 87);
            this.LogBox.Name = "LogBox";
            this.LogBox.ScrollBars = System.Windows.Forms.RichTextBoxScrollBars.Vertical;
            this.LogBox.Size = new System.Drawing.Size(661, 902);
            this.LogBox.TabIndex = 3;
            this.LogBox.Text = "";
            // 
            // MainForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(8F, 15F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.AutoSize = true;
            this.ClientSize = new System.Drawing.Size(1782, 1003);
            this.Controls.Add(this.LogBox);
            this.Controls.Add(this.DebugTextBox);
            this.Controls.Add(this.RenderBox);
            this.Name = "MainForm";
            this.StartPosition = System.Windows.Forms.FormStartPosition.Manual;
            this.Text = "JGEngine";
            this.Load += new System.EventHandler(this.MainForm_Load);
            this.MouseDown += new System.Windows.Forms.MouseEventHandler(this.MainForm_MouseDown);
            this.MouseMove += new System.Windows.Forms.MouseEventHandler(this.MainForm_MouseMove);
            this.MouseUp += new System.Windows.Forms.MouseEventHandler(this.MainForm_MouseUp);
            ((System.ComponentModel.ISupportInitialize)(this.RenderBox)).EndInit();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        public System.Windows.Forms.PictureBox RenderBox;
        public System.Windows.Forms.TextBox DebugTextBox;
        private System.Windows.Forms.RichTextBox LogBox;
    }
}