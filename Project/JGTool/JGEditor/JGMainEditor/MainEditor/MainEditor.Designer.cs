namespace MainEditor
{
    partial class MainEditor
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

        #region Windows Form 디자이너에서 생성한 코드

        /// <summary>
        /// 디자이너 지원에 필요한 메서드입니다. 
        /// 이 메서드의 내용을 코드 편집기로 수정하지 마세요.
        /// </summary>
        private void InitializeComponent()
        {
            this.MainEditorTab = new System.Windows.Forms.TabControl();
            this.MainTabPage = new System.Windows.Forms.TabPage();
            this.MainEditorTab.SuspendLayout();
            this.SuspendLayout();
            // 
            // MainEditorTab
            // 
            this.MainEditorTab.Controls.Add(this.MainTabPage);
            this.MainEditorTab.Dock = System.Windows.Forms.DockStyle.Fill;
            this.MainEditorTab.Location = new System.Drawing.Point(0, 0);
            this.MainEditorTab.Name = "MainEditorTab";
            this.MainEditorTab.SelectedIndex = 0;
            this.MainEditorTab.Size = new System.Drawing.Size(800, 450);
            this.MainEditorTab.TabIndex = 0;
            // 
            // MainTabPage
            // 
            this.MainTabPage.Location = new System.Drawing.Point(4, 25);
            this.MainTabPage.Name = "MainTabPage";
            this.MainTabPage.Padding = new System.Windows.Forms.Padding(3);
            this.MainTabPage.Size = new System.Drawing.Size(792, 421);
            this.MainTabPage.TabIndex = 0;
            this.MainTabPage.Text = "Main";
            this.MainTabPage.UseVisualStyleBackColor = true;
            // 
            // MainEditor
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(8F, 15F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(800, 450);
            this.Controls.Add(this.MainEditorTab);
            this.Name = "MainEditor";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
            this.Text = "JGEngine";
            this.WindowState = System.Windows.Forms.FormWindowState.Maximized;
            this.Shown += new System.EventHandler(this.MainEditor_Shown);
            this.MainEditorTab.ResumeLayout(false);
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.TabControl MainEditorTab;
        private System.Windows.Forms.TabPage MainTabPage;
    }
}

