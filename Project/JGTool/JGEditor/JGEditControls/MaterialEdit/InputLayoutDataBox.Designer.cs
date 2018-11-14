namespace JGEditControls.MaterialEdit
{
    partial class InputLayoutDataBox
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
            this.hlslType = new System.Windows.Forms.ComboBox();
            this.SementicName = new System.Windows.Forms.ComboBox();
            this.Format = new System.Windows.Forms.ComboBox();
            this.SementicIndex = new System.Windows.Forms.TextBox();
            this.InputSlot = new System.Windows.Forms.TextBox();
            this.Offset = new System.Windows.Forms.TextBox();
            this.InputClass = new System.Windows.Forms.ComboBox();
            this.InstanceDataStepRate = new System.Windows.Forms.TextBox();
            this.SuspendLayout();
            // 
            // hlslType
            // 
            this.hlslType.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.hlslType.FormattingEnabled = true;
            this.hlslType.Items.AddRange(new object[] {
            "float",
            "float2",
            "float3",
            "float4"});
            this.hlslType.Location = new System.Drawing.Point(0, 1);
            this.hlslType.Name = "hlslType";
            this.hlslType.Size = new System.Drawing.Size(60, 23);
            this.hlslType.TabIndex = 0;
            // 
            // SementicName
            // 
            this.SementicName.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.SementicName.FormattingEnabled = true;
            this.SementicName.Items.AddRange(new object[] {
            "POSITION",
            "COLOR",
            "NORMAL",
            "TEXCOORD",
            "TANGENT",
            "BINORMAL"});
            this.SementicName.Location = new System.Drawing.Point(66, 1);
            this.SementicName.Name = "SementicName";
            this.SementicName.Size = new System.Drawing.Size(110, 23);
            this.SementicName.TabIndex = 1;
            // 
            // Format
            // 
            this.Format.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.Format.FormattingEnabled = true;
            this.Format.Items.AddRange(new object[] {
            "R32G32B32A32_F32",
            "R32G32B32_F32",
            "R32G32_F32",
            "R32_F32\""});
            this.Format.Location = new System.Drawing.Point(214, 1);
            this.Format.Name = "Format";
            this.Format.Size = new System.Drawing.Size(160, 23);
            this.Format.TabIndex = 2;
            // 
            // SementicIndex
            // 
            this.SementicIndex.Location = new System.Drawing.Point(183, 0);
            this.SementicIndex.MaxLength = 3;
            this.SementicIndex.Name = "SementicIndex";
            this.SementicIndex.Size = new System.Drawing.Size(25, 25);
            this.SementicIndex.TabIndex = 3;
            // 
            // InputSlot
            // 
            this.InputSlot.Location = new System.Drawing.Point(380, 0);
            this.InputSlot.MaxLength = 3;
            this.InputSlot.Name = "InputSlot";
            this.InputSlot.Size = new System.Drawing.Size(25, 25);
            this.InputSlot.TabIndex = 4;
            // 
            // Offset
            // 
            this.Offset.Location = new System.Drawing.Point(411, 0);
            this.Offset.MaxLength = 3;
            this.Offset.Name = "Offset";
            this.Offset.Size = new System.Drawing.Size(25, 25);
            this.Offset.TabIndex = 5;
            // 
            // InputClass
            // 
            this.InputClass.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.InputClass.FormattingEnabled = true;
            this.InputClass.Items.AddRange(new object[] {
            "Vertex",
            "Instance"});
            this.InputClass.Location = new System.Drawing.Point(442, 1);
            this.InputClass.Name = "InputClass";
            this.InputClass.Size = new System.Drawing.Size(90, 23);
            this.InputClass.TabIndex = 6;
            // 
            // InstanceDataStepRate
            // 
            this.InstanceDataStepRate.Location = new System.Drawing.Point(538, 0);
            this.InstanceDataStepRate.MaxLength = 3;
            this.InstanceDataStepRate.Name = "InstanceDataStepRate";
            this.InstanceDataStepRate.Size = new System.Drawing.Size(25, 25);
            this.InstanceDataStepRate.TabIndex = 7;
            // 
            // InputLayoutDataBox
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(8F, 15F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.Controls.Add(this.InstanceDataStepRate);
            this.Controls.Add(this.InputClass);
            this.Controls.Add(this.Offset);
            this.Controls.Add(this.InputSlot);
            this.Controls.Add(this.SementicIndex);
            this.Controls.Add(this.Format);
            this.Controls.Add(this.SementicName);
            this.Controls.Add(this.hlslType);
            this.Name = "InputLayoutDataBox";
            this.Size = new System.Drawing.Size(570, 26);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.ComboBox hlslType;
        private System.Windows.Forms.ComboBox SementicName;
        private System.Windows.Forms.ComboBox Format;
        private System.Windows.Forms.TextBox SementicIndex;
        private System.Windows.Forms.TextBox InputSlot;
        private System.Windows.Forms.TextBox Offset;
        private System.Windows.Forms.ComboBox InputClass;
        private System.Windows.Forms.TextBox InstanceDataStepRate;
    }
}
