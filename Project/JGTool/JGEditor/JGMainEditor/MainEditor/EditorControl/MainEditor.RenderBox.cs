using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.Drawing;
using MainRenderBox;
namespace MainEditor
{
    /*
      렌더링 박스 관련 파셜 클래스 */
    public partial class MainEditor : Form
    {
        RenderBox m_RenderBox;
        Point m_RenderBoxLocation = new Point();

        Size m_RenderBoxSize = new Size();
        public void CreateRenderBox()
        {
            m_RenderBoxLocation.Offset(0, 0);
            m_RenderBoxSize.Width = 900;
            m_RenderBoxSize.Height = 510;
            // 렌더 박스 추가
            m_RenderBox = new RenderBox();
            m_RenderBox.FormBorderStyle = FormBorderStyle.FixedDialog;
            m_RenderBox.Width = m_RenderBoxSize.Width;
            m_RenderBox.Height = m_RenderBoxSize.Height;
            m_RenderBox.TopLevel = false;
            m_RenderBox.Location = m_RenderBoxLocation;
            m_RenderBox.Show();
            MainTabPage.Controls.Add(m_RenderBox);
        }
    }
}
