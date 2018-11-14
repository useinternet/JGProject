using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
namespace JGEditControls.MaterialEdit
{
    public partial class InputLayoutData : UserControl
    {
        static readonly uint MaxBoxCount = 20;
        uint CurrentCount = 0;
        InputLayoutDataBox[] LayoutBox = new InputLayoutDataBox[MaxBoxCount];
        public InputLayoutData()
        {

            InitializeComponent();
            // 각 컨트롤 초기화
            #region
            Point pos = new Point(0, 3);
            LayoutDataPanel.AutoScroll = false;
            LayoutDataPanel.HorizontalScroll.Enabled = false;
            LayoutDataPanel.HorizontalScroll.Visible = false;
            LayoutDataPanel.AutoScroll = true;
            for (int i=0; i<MaxBoxCount; ++i)
            {
                LayoutBox[i] = new InputLayoutDataBox();
                LayoutBox[i].Location = pos;
                pos.Y += LayoutBox[i].Height;
                LayoutBox[i].Hide();
                LayoutBox[i].ClickEvent = ChangeDataLabel;
                this.LayoutDataPanel.Controls.Add(LayoutBox[i]);
            }
            #endregion
        }
        private void AddBt_Click(object sender, EventArgs e)
        {
            if(CurrentCount >= MaxBoxCount)
            {
                return;
            }
            LayoutBox[CurrentCount].Show();
            CurrentCount++;
        }
        private void RemoveBt_Click(object sender, EventArgs e)
        {
            if(CurrentCount <= 0)
            {
                return;
            }
            CurrentCount--;
            LayoutBox[CurrentCount].Hide();
        }
        private void ChangeDataLabel(string text)
        {
            DataLabel.Text = text;
          

        }
    }
}
