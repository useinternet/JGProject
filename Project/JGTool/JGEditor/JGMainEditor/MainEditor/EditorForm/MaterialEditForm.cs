using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using MainEditor;
namespace MainEditor.EditorForm
{
    public partial class MaterialEditForm : Form
    {
        public MaterialEditForm()
        {
            InitializeComponent();
            DxWindowConfigCpp Data = new DxWindowConfigCpp();
            Data.hWnd = MatPanel.Handle;
            Data.ScreenWidth = MatPanel.Width;
            Data.ScreenHeight = MatPanel.Height;
            Data.bFullScreen = false;
            Data.FarZ = 100000.0f;
            Data.NearZ = 0.001f;
            Data.Fov = 3.141592f / 4.0f;
            JGEngineCore.SendCommand(ED2EGCommand.ed_AddDxWindow, Data);
      
        }
    }
}
