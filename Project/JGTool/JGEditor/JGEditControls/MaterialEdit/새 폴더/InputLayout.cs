using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace MainEditor.EditorForm.HLSLInformation
{
    
    class InputLayout
    {
        static private readonly uint MaxCount = 20;
        static private readonly int  YPos = 10;
        static private readonly int  Gap = 3;
        static private readonly int TPWidth   = 60;
        static private readonly int SNWidth   = 110;
        static private readonly int FMWidth   = 160;
        static private readonly int SCWidth = 90;
        static private readonly int TextWidth = 25;
        static uint CurrentCount = 0;
        private Panel MainPanel = new Panel();
        private ComboBox[] Type = new ComboBox[MaxCount];
        private ComboBox[] SementicName  = new ComboBox[MaxCount];
        private TextBox[]  SementicIndex = new TextBox[MaxCount];
        private ComboBox[] Format        = new ComboBox[MaxCount];
        private TextBox[]  InputSlot     = new TextBox[MaxCount];
        private TextBox[]  Offset        = new TextBox[MaxCount];
        private ComboBox[] SlotClass     = new ComboBox[MaxCount];
        private TextBox[]  InstanceDataStepRate = new TextBox[MaxCount];
        private ScrollBar  vScrollBar = new VScrollBar();
        private Label InputLabel = new Label();
        private Label LogoLabel = new Label();

        public void Init(Form form)
        {
            // 각 컨트롤러 초기화
            #region
            MainPanelInit();
            int XPos = 7;
            // 타입
            InitComboBox(Type, new object[]{"float","float2","float3","float4"}, "HLSLType", XPos, TPWidth);
            XPos += TPWidth + Gap;
            // 시멘틱 이름
            InitComboBox(SementicName, new object[]{"POSITION","TEXCOORD","NORMAL","TANGENT","BINORMAL","COLOR"},
                "SementicName",XPos, SNWidth);
            XPos += SNWidth + Gap;
            // 시멘틱 인덱스
            InitTextBox(SementicIndex, "SementicIndex", XPos);
            XPos += TextWidth + Gap;
            // 포맷
            InitComboBox(Format, new object[] {"R32G32B32A32_F32","R32G32B32_F32","R32G32_F32","R32_F32" },
                "Format", XPos, FMWidth);
            XPos += FMWidth + Gap;
            // 입력 슬롯
            InitTextBox(InputSlot, "InputSlot", XPos);
            XPos += TextWidth + Gap;
            //오프셋
            InitTextBox(Offset, "Offset", XPos);
            XPos += TextWidth + Gap;
            // 슬롯 클래스
            InitComboBox(SlotClass, new object[] { "Vertex", "Instance"}, "SlotClass", XPos, SCWidth);
            XPos += SCWidth + Gap;
            InitTextBox(InstanceDataStepRate, "InstanceDataStepRate", XPos);
            MainPanel.Show();
            form.Controls.Add(MainPanel);
            #endregion
            // 라벨 달기 ( 입력 레이아웃 )
            #region 
            InputLabel.Font = new System.Drawing.Font("굴림", 10.2F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            InputLabel.Location = new System.Drawing.Point(1200, 10);
            InputLabel.Name = "InputLabel";
            InputLabel.Size = new System.Drawing.Size(158, 23);
            InputLabel.TabIndex = 10;
            InputLabel.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            LogoLabel.Font = new System.Drawing.Font("굴림", 10.2F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            LogoLabel.Location = new System.Drawing.Point(1000, 10);
            LogoLabel.Name = "InputLabel";
            LogoLabel.Size = new System.Drawing.Size(158, 23);
            LogoLabel.TabIndex = 10;
            LogoLabel.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            LogoLabel.Text = "입력 레이아웃";
            form.Controls.Add(InputLabel);
            form.Controls.Add(LogoLabel);
            #endregion 
        }

        // 입력 레이아웃 컨트롤 생성
        public void Create()
        {
            if(CurrentCount >= MaxCount)
            {
                return;
            }
            Type[CurrentCount].Show();
            SementicName[CurrentCount].Show();
            SementicIndex[CurrentCount].Show();
            Format[CurrentCount].Show();
            InputSlot[CurrentCount].Show();
            Offset[CurrentCount].Show();
            SlotClass[CurrentCount].Show();
            InstanceDataStepRate[CurrentCount].Show();
            CurrentCount++;
        }

        // 입력 레이아웃 컨트롤 삭제
        public void Delete()
        {
            if(CurrentCount <= 0)
            {
                return;
            }
            CurrentCount--;
            Type[CurrentCount].Hide();
            SementicName[CurrentCount].Hide();
            SementicIndex[CurrentCount].Hide();
            Format[CurrentCount].Hide();
            InputSlot[CurrentCount].Hide();
            Offset[CurrentCount].Hide();
            SlotClass[CurrentCount].Hide();
            InstanceDataStepRate[CurrentCount].Hide();

        }

        // 컨트롤 안에서 일어나는 일들
        #region 
        // 콤보박스 추가( MainPanel )
        private void AddControlComboBox(ComboBox[] boxes)
        {
            for(int i=0; i<boxes.Length; ++i)
            {
                boxes[i] = new ComboBox();
                MainPanel.Controls.Add(boxes[i]);
            }
        }
        // 텍스트박스 추가( MainPanel )
        private void AddControlTextBox(TextBox[] textBoxes)
        {
            for (int i = 0; i < textBoxes.Length; ++i)
            {
                textBoxes[i] = new TextBox();
                MainPanel.Controls.Add(textBoxes[i]);
            }
        }
        // 콤보 박스 초기화
        private void InitComboBox(ComboBox[] comboBoxes, object[] elements,string name, int Xpos, int Width)
        {
            int accYPos = YPos;
            foreach (ComboBox box in comboBoxes)
            {
                box.FormattingEnabled = true;
                box.Items.AddRange(elements);
                box.Location = new System.Drawing.Point(Xpos, accYPos);
                box.Name = name;
                box.Size = new System.Drawing.Size(Width, 23);
                box.TabIndex = 8;
                box.DropDownStyle = ComboBoxStyle.DropDownList;
                box.Hide();
                box.MouseWheel += Box_MouseWheel;
                box.GotFocus += BoxGotFocus;
                accYPos += box.Size.Height + Gap + 1;
            }
        }
        // 텍스트 박스 초기화
        private void InitTextBox(TextBox[] textBoxes, string name, int Xpos)
        {
            int accYPos = YPos;
            foreach (TextBox box in textBoxes)
            {
                box.Location = new System.Drawing.Point(Xpos, accYPos);
                box.Name = name;
                box.Size = new System.Drawing.Size(TextWidth, 25);
                box.TabIndex = 1;
                box.MaxLength = 3;
                box.Text = "0";
                box.GotFocus += BoxGotFocus;
                box.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
                box.Hide();
                accYPos += box.Size.Height + Gap;
            }

        }
        // 박스에 포커스가 일어날때 이벤트
        private void BoxGotFocus(object sender, EventArgs e)
        {
            if(sender is TextBox)
            {
                InputLabel.Text = (sender as TextBox).Name;
            }
            if(sender is ComboBox)
            {
                InputLabel.Text = (sender as ComboBox).Name;
            }
        }
        // 박스에 마우스 휠 이벤트
        private void Box_MouseWheel(object sender, MouseEventArgs e)
        {
            MainPanel.Focus();
            ((HandledMouseEventArgs)e).Handled = true;
        }
        // 메인 패널 초기화
        private void MainPanelInit()
        {
            MainPanel.BackColor = System.Drawing.SystemColors.AppWorkspace;
            MainPanel.Location = new System.Drawing.Point(960, 35);
            MainPanel.Name = "InputLayoutPanel";
            MainPanel.Size = new System.Drawing.Size(570, 150);
            MainPanel.TabIndex = 1;
            MainPanel.AutoScroll = false;
            MainPanel.HorizontalScroll.Enabled = false;
            MainPanel.HorizontalScroll.Visible = false;
            MainPanel.AutoScroll = true;
            AddControlComboBox(Type);
            AddControlComboBox(SementicName);
            AddControlTextBox(SementicIndex);
            AddControlComboBox(Format);
            AddControlTextBox(InputSlot);
            AddControlTextBox(Offset);
            AddControlComboBox(SlotClass);
            AddControlTextBox(InstanceDataStepRate);
        }
        #endregion
    }
}
