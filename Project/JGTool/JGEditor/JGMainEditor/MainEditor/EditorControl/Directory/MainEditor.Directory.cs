using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.IO;
// 파일 선택 했을때 다른 메뉴
// 파일 선택 안했을때 다른 메뉴
namespace MainEditor
{
    // 파일 이동 자유롭게..
    public partial class MainEditor : Form
    {
        private string RootEntryPath = "../../../../../../..";
        private string RootPath = "../../../../../../../Contents/";
        TreeNode RootNode = new TreeNode("Contents");
        private void Directory_Init()
        {
            // 디렉 토리 검색
            if (Directory.Exists(RootPath))
            {
                DirectoryInfo RootDir = new DirectoryInfo(RootPath);
                DirectorySearch(RootDir.GetDirectories(),ref RootNode);
            }
            DirectoryViewer.Nodes.Add(RootNode);
            DirectoryViewer.ImageList = FileIcons;
            DirFileList.LargeImageList = FileIcons;
            DirFileList.ContextMenuStrip = DirFileListMenu;
        }
        private string GetCurrentPath(TreeNode node, string CurPath)
        {
            if (node == null)
            {
                return CurPath;
            }
            CurPath = GetCurrentPath(node.Parent, CurPath);
            return CurPath + "/" + node.Text;
        }
        private void DirectorySearch(DirectoryInfo[] Dirs,ref TreeNode node)
        {
            if(Dirs.Length == 0)
            {
                return;
            }
            foreach(DirectoryInfo dir in Dirs)
            {
                TreeNode tmp = new TreeNode(dir.ToString());
                
                DirectorySearch(dir.GetDirectories(), ref tmp);
                node.Nodes.Add(tmp);
            }
        }
        private void FileSearch(TreeNode CurrentNode)
        {
            DirFileList.Items.Clear();
            // 엔트리 패스
            string CurPath = RootEntryPath;
            CurPath = GetCurrentPath(CurrentNode, CurPath);

            // 각 파일항목들을 검색해 뿌린다.
            DirectoryInfo CurrentDir = new DirectoryInfo(CurPath);
            foreach (var dir in CurrentDir.GetDirectories())
            {
                ListViewItem item = new ListViewItem();
                item.ImageIndex = 0;
                item.Text = dir.Name;
                DirFileList.Items.Add(item);

            }
            foreach (var file in CurrentDir.GetFiles())
            {
                ListViewItem item = new ListViewItem();
                item.ImageIndex = 1;
                item.Text = file.Name;
                DirFileList.Items.Add(item);

            }
        }
        private void DirFileList_MouseClick(object sender, MouseEventArgs e)
        {
            if(e.Button == MouseButtons.Right)
            {
                DirFileList.ContextMenuStrip = DirFileListMenu;
                DirFileList.ContextMenuStrip.Show(e.X,e.Y);
            }
            else
            {
                DirFileList.ContextMenuStrip.Visible = false;
            }
        }
        private void DirectoryViewer_DoubleClick(object sender, EventArgs e)
        {
            FileSearch(DirectoryViewer.SelectedNode);
        }
        // 디렉토리 뷰어 이벤트
        private void DirFileList_DoubleClick(object sender, EventArgs e)
        {
            if (DirFileList.SelectedItems.Count != 1) return;
            // 선택된 아이템
            ListViewItem item = DirFileList.SelectedItems[0];
            // 선택된 노드(디렉토리)
            TreeNode SelectNode = DirectoryViewer.SelectedNode;
            string CurPath = RootEntryPath;
            CurPath = GetCurrentPath(SelectNode, CurPath); // 현재 디렉토리 경로를 가져온다.
            CurPath += "/" + item.Text;
            if (Directory.Exists(CurPath))
            {
                foreach (TreeNode n in SelectNode.Nodes)
                {
                    if (n.Text == item.Text)
                    {
                        DirectoryViewer.SelectedNode = n;
                        FileSearch(n);
                    }
                }
            }
        }
    }
}
