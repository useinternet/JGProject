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
            this.components = new System.ComponentModel.Container();
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(MainEditor));
            this.RenderPanel = new System.Windows.Forms.Panel();
            this.EngineLog = new System.Windows.Forms.RichTextBox();
            this.DirFileList = new System.Windows.Forms.ListView();
            this.SearchBox = new System.Windows.Forms.TextBox();
            this.DirectoryViewer = new System.Windows.Forms.TreeView();
            this.FileIcons = new System.Windows.Forms.ImageList(this.components);
            this.DirFileListMenu = new System.Windows.Forms.ContextMenuStrip(this.components);
            this.머터리얼생성ToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.panel1 = new System.Windows.Forms.Panel();
            this.DirFileListMenu.SuspendLayout();
            this.SuspendLayout();
            // 
            // RenderPanel
            // 
            this.RenderPanel.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.RenderPanel.BackColor = System.Drawing.SystemColors.ActiveCaption;
            this.RenderPanel.Location = new System.Drawing.Point(0, 0);
            this.RenderPanel.Name = "RenderPanel";
            this.RenderPanel.Size = new System.Drawing.Size(900, 510);
            this.RenderPanel.TabIndex = 0;
            // 
            // EngineLog
            // 
            this.EngineLog.Anchor = System.Windows.Forms.AnchorStyles.None;
            this.EngineLog.Location = new System.Drawing.Point(0, 516);
            this.EngineLog.Name = "EngineLog";
            this.EngineLog.ReadOnly = true;
            this.EngineLog.Size = new System.Drawing.Size(518, 277);
            this.EngineLog.TabIndex = 1;
            this.EngineLog.Text = "";
            // 
            // DirFileList
            // 
            this.DirFileList.AllowDrop = true;
            this.DirFileList.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.DirFileList.Location = new System.Drawing.Point(744, 516);
            this.DirFileList.Name = "DirFileList";
            this.DirFileList.Size = new System.Drawing.Size(773, 277);
            this.DirFileList.TabIndex = 2;
            this.DirFileList.UseCompatibleStateImageBehavior = false;
            this.DirFileList.DoubleClick += new System.EventHandler(this.DirFileList_DoubleClick);
            this.DirFileList.MouseClick += new System.Windows.Forms.MouseEventHandler(this.DirFileList_MouseClick);
            // 
            // SearchBox
            // 
            this.SearchBox.Location = new System.Drawing.Point(524, 519);
            this.SearchBox.Name = "SearchBox";
            this.SearchBox.Size = new System.Drawing.Size(214, 25);
            this.SearchBox.TabIndex = 5;
            // 
            // DirectoryViewer
            // 
            this.DirectoryViewer.AllowDrop = true;
            this.DirectoryViewer.Location = new System.Drawing.Point(524, 550);
            this.DirectoryViewer.Name = "DirectoryViewer";
            this.DirectoryViewer.Size = new System.Drawing.Size(214, 243);
            this.DirectoryViewer.TabIndex = 6;
            this.DirectoryViewer.DoubleClick += new System.EventHandler(this.DirectoryViewer_DoubleClick);
            // 
            // FileIcons
            // 
            this.FileIcons.ImageStream = ((System.Windows.Forms.ImageListStreamer)(resources.GetObject("FileIcons.ImageStream")));
            this.FileIcons.TransparentColor = System.Drawing.Color.Transparent;
            this.FileIcons.Images.SetKeyName(0, "floder_Icon.png");
            this.FileIcons.Images.SetKeyName(1, "MaterialIcon.ico");
            // 
            // DirFileListMenu
            // 
            this.DirFileListMenu.ImageScalingSize = new System.Drawing.Size(20, 20);
            this.DirFileListMenu.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.머터리얼생성ToolStripMenuItem});
            this.DirFileListMenu.Name = "FileListMenu";
            this.DirFileListMenu.Size = new System.Drawing.Size(174, 28);
            this.DirFileListMenu.Text = "머터리얼 생성";
            // 
            // 머터리얼생성ToolStripMenuItem
            // 
            this.머터리얼생성ToolStripMenuItem.Name = "머터리얼생성ToolStripMenuItem";
            this.머터리얼생성ToolStripMenuItem.Size = new System.Drawing.Size(173, 24);
            this.머터리얼생성ToolStripMenuItem.Text = "머터리얼 생성";
            this.머터리얼생성ToolStripMenuItem.Click += new System.EventHandler(this.머터리얼생성ToolStripMenuItem_Click);
            // 
            // panel1
            // 
            this.panel1.BackColor = System.Drawing.SystemColors.ActiveBorder;
            this.panel1.Location = new System.Drawing.Point(943, 12);
            this.panel1.Name = "panel1";
            this.panel1.Size = new System.Drawing.Size(487, 408);
            this.panel1.TabIndex = 7;
            // 
            // MainEditor
            // 
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.None;
            this.AutoSizeMode = System.Windows.Forms.AutoSizeMode.GrowAndShrink;
            this.ClientSize = new System.Drawing.Size(1518, 794);
            this.Controls.Add(this.panel1);
            this.Controls.Add(this.DirectoryViewer);
            this.Controls.Add(this.SearchBox);
            this.Controls.Add(this.DirFileList);
            this.Controls.Add(this.EngineLog);
            this.Controls.Add(this.RenderPanel);
            this.DoubleBuffered = true;
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedToolWindow;
            this.Name = "MainEditor";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
            this.Text = "JGEngine";
            this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.MainEditor_FormClosing);
            this.SizeChanged += new System.EventHandler(this.MainEditor_SizeChanged);
            this.DirFileListMenu.ResumeLayout(false);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Panel RenderPanel;
        private System.Windows.Forms.RichTextBox EngineLog;
        private System.Windows.Forms.ListView DirFileList;
        private System.Windows.Forms.TextBox SearchBox;
        private System.Windows.Forms.TreeView DirectoryViewer;
        private System.Windows.Forms.ImageList FileIcons;
        private System.Windows.Forms.ContextMenuStrip DirFileListMenu;
        private System.Windows.Forms.ToolStripMenuItem 머터리얼생성ToolStripMenuItem;
        private System.Windows.Forms.Panel panel1;
    }
}

