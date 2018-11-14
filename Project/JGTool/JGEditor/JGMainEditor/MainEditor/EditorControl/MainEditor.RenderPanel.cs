using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
namespace MainEditor
{
    public partial class MainEditor : Form
    {
        private void RenderPanel_Init()
        {
            DxWindowConfigCpp Data = new DxWindowConfigCpp();
            Data.hWnd = RenderPanel.Handle;
            Data.ScreenWidth = 1920;
            Data.ScreenHeight = 1080;
            Data.bFullScreen = false;
            Data.FarZ = 100000.0f;
            Data.NearZ = 0.001f;
            Data.Fov = 3.141592f / 4.0f;
            JGEngineCore.SendCommand(ED2EGCommand.ed_AddDxWindow, Data);
        }







    }
}
