namespace MainEditor.EditorForm.HLSLInformation
{
    partial class InputLayoutData
    {
        /// <summary> 
        /// 필수 디자이너 변수입니다.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary> 
        /// 사용 중인 모든 리소스를 정리합니다.
        /// </summary>
        /// <param name="disposing">관리되는 리소스를 삭제해야 하면 true이고, 그렇지 않으면 false입니다.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);

        }

        #region 구성 요소 디자이너에서 생성한 코드

        /// <summary> 
        /// 디자이너 지원에 필요한 메서드입니다. 
        /// 이 메서드의 내용을 코드 편집기로 수정하지 마세요.
        /// </summary>
        private void InitializeComponent()
        {
            this.LayoutDataPanel = new System.Windows.Forms.Panel();
            this.ControlPanel = new System.Windows.Forms.Panel();
            this.DataLabel = new System.Windows.Forms.Label();
            this.InputLayoutLabel = new System.Windows.Forms.Label();
            this.RemoveBt = new System.Windows.Forms.Button();
            this.AddBt = new System.Windows.Forms.Button();
            this.ControlPanel.SuspendLayout();
            this.SuspendLayout();
            // 
            // LayoutDataPanel
            // 
            this.LayoutDataPanel.BackColor = System.Drawing.SystemColors.ButtonShadow;
            this.LayoutDataPanel.Location = new System.Drawing.Point(0, 53);
            this.LayoutDataPanel.Name = "LayoutDataPanel";
            this.LayoutDataPanel.Size = new System.Drawing.Size(570, 147);
            this.LayoutDataPanel.TabIndex = 0;
            // 
            // ControlPanel
            // 
            this.ControlPanel.Controls.Add(this.DataLabel);
            this.ControlPanel.Controls.Add(this.InputLayoutLabel);
            this.ControlPanel.Controls.Add(this.RemoveBt);
            this.ControlPanel.Controls.Add(this.AddBt);
            this.ControlPanel.Location = new System.Drawing.Point(0, 0);
            this.ControlPanel.Name = "ControlPanel";
            this.ControlPanel.Size = new System.Drawing.Size(570, 50);
            this.ControlPanel.TabIndex = 1;
            // 
            // DataLabel
            // 
            this.DataLabel.AutoSize = true;
            this.DataLabel.Font = new System.Drawing.Font("굴림", 12F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.DataLabel.Location = new System.Drawing.Point(190, 17);
            this.DataLabel.Name = "DataLabel";
            this.DataLabel.Size = new System.Drawing.Size(71, 20);
            this.DataLabel.TabIndex = 3;
            this.DataLabel.Text = "(None)";
            // 
            // InputLayoutLabel
            // 
            this.InputLayoutLabel.AutoSize = true;
            this.InputLayoutLabel.Font = new System.Drawing.Font("굴림", 12F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.InputLayoutLabel.Location = new System.Drawing.Point(17, 17);
            this.InputLayoutLabel.Name = "InputLayoutLabel";
            this.InputLayoutLabel.Size = new System.Drawing.Size(167, 20);
            this.InputLayoutLabel.TabIndex = 2;
            this.InputLayoutLabel.Text = "입력 레이아웃 : ";
            // 
            // RemoveBt
            // 
            this.RemoveBt.Location = new System.Drawing.Point(482, 14);
            this.RemoveBt.Name = "RemoveBt";
            this.RemoveBt.Size = new System.Drawing.Size(75, 23);
            this.RemoveBt.TabIndex = 1;
            this.RemoveBt.Text = "삭제";
            this.RemoveBt.UseVisualStyleBackColor = true;
            // 
            // AddBt
            // 
            this.AddBt.Location = new System.Drawing.Point(388, 14);
            this.AddBt.Name = "AddBt";
            this.AddBt.Size = new System.Drawing.Size(75, 23);
            this.AddBt.TabIndex = 0;
            this.AddBt.Text = "추가";
            this.AddBt.UseVisualStyleBackColor = true;
            // 
            // InputLayoutData
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(8F, 15F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.Controls.Add(this.ControlPanel);
            this.Controls.Add(this.LayoutDataPanel);
            this.MinimumSize = new System.Drawing.Size(570, 200);
            this.Name = "InputLayoutData";
            this.Size = new System.Drawing.Size(570, 200);
            this.ControlPanel.ResumeLayout(false);
            this.ControlPanel.PerformLayout();
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.Panel LayoutDataPanel;
        private System.Windows.Forms.Panel ControlPanel;
        private System.Windows.Forms.Label DataLabel;
        private System.Windows.Forms.Label InputLayoutLabel;
        private System.Windows.Forms.Button RemoveBt;
        private System.Windows.Forms.Button AddBt;
    }
}
