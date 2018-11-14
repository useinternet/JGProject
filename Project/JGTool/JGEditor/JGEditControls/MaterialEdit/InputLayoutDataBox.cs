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
    public struct ILData
    {
        public string Type;
        public string SementicName;
        public int    SementicIndex;
        public string Format;
        public int    InputSlot;
        public int    Offset;
        public string InputClass;
        public int    InstanceDataStepRate;
    }
    public delegate void BoxClickEvent(string Text);
    public partial class InputLayoutDataBox : UserControl
    {
        private BoxClickEvent boxClickEvent;
        public InputLayoutDataBox()
        {
            InitializeComponent();
            //  각 박스 마우스 휠 이벤트 추가
            #region

            hlslType.MouseWheel     += RemoveWheelEvent;
            SementicName.MouseWheel += RemoveWheelEvent;
            Format.MouseWheel       += RemoveWheelEvent;
            InputClass.MouseWheel   += RemoveWheelEvent;

            #endregion

            // 박스 각 포커스 이벤트 추가
            #region
            hlslType.GotFocus += Box_MouseClick;
            SementicName.GotFocus += Box_MouseClick;
            SementicIndex.GotFocus += Box_MouseClick;
            Format.GotFocus += Box_MouseClick;
            InputSlot.GotFocus += Box_MouseClick;
            Offset.GotFocus += Box_MouseClick;
            InputClass.GotFocus += Box_MouseClick;
            InstanceDataStepRate.GotFocus += Box_MouseClick;
            #endregion
        }
        private void Box_MouseClick(object sender, EventArgs e)
        {
            if(sender is TextBox)
            {
                boxClickEvent((sender as TextBox).Name);
            }
            else if (sender is ComboBox)
            {
                boxClickEvent((sender as ComboBox).Name);
            }
        }
        private void RemoveWheelEvent(object sender, MouseEventArgs e)
        {
            ((HandledMouseEventArgs)e).Handled = true;
        }
        public ILData OutputData()
        {
            ILData data = new ILData();
            data.Type = hlslType.SelectedText;
            data.SementicName = hlslType.SelectedText;
            data.SementicIndex = int.Parse(SementicIndex.Text);
            data.Format = Format.SelectedText;
            data.InputSlot = int.Parse(InputSlot.Text);
            data.Offset = int.Parse(Offset.Text);
            data.InputClass = InputClass.SelectedText;
            data.InstanceDataStepRate = int.Parse(InstanceDataStepRate.Text);
            return data;
        }
        public BoxClickEvent ClickEvent
        {
            set { boxClickEvent = value; }
        }
    }
}
