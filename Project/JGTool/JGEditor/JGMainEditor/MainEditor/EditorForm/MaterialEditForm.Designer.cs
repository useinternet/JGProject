namespace MainEditor.EditorForm
{
    partial class MaterialEditForm
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
            JGEngineCore.End();
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
            this.MatPanel = new System.Windows.Forms.Panel();
            this.inputLayoutData1 = new JGEditControls.MaterialEdit.InputLayoutData();
            this.cBufferEditor1 = new JGEditControls.MaterialEdit.CBufferEditor();
            this.SuspendLayout();
            // 
            // MatPanel
            // 
            this.MatPanel.BackColor = System.Drawing.SystemColors.ActiveCaption;
            this.MatPanel.Location = new System.Drawing.Point(3, 3);
            this.MatPanel.Name = "MatPanel";
            this.MatPanel.Size = new System.Drawing.Size(400, 400);
            this.MatPanel.TabIndex = 0;
            // 
            // inputLayoutData1
            // 
            this.inputLayoutData1.Location = new System.Drawing.Point(1147, 26);
            this.inputLayoutData1.MinimumSize = new System.Drawing.Size(570, 200);
            this.inputLayoutData1.Name = "inputLayoutData1";
            this.inputLayoutData1.Size = new System.Drawing.Size(570, 200);
            this.inputLayoutData1.TabIndex = 12;
            // 
            // cBufferEditor1
            // 
            this.cBufferEditor1.BackColor = System.Drawing.SystemColors.ControlDark;
            this.cBufferEditor1.Location = new System.Drawing.Point(1147, 252);
            this.cBufferEditor1.Name = "cBufferEditor1";
            this.cBufferEditor1.Size = new System.Drawing.Size(590, 700);
            this.cBufferEditor1.TabIndex = 13;
            // 
            // MaterialEditForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(8F, 15F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.AutoSize = true;
            this.AutoSizeMode = System.Windows.Forms.AutoSizeMode.GrowAndShrink;
            this.ClientSize = new System.Drawing.Size(1518, 794);
            this.Controls.Add(this.cBufferEditor1);
            this.Controls.Add(this.inputLayoutData1);
            this.Controls.Add(this.MatPanel);
            this.DoubleBuffered = true;
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedToolWindow;
            this.Name = "MaterialEditForm";
            this.StartPosition = System.Windows.Forms.FormStartPosition.Manual;
            this.Text = "MaterialEditor";
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.Panel MatPanel;
        private JGEditControls.MaterialEdit.InputLayoutData inputLayoutData1;
        private JGEditControls.MaterialEdit.CBufferEditor cBufferEditor1;
    }
}