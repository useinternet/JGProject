using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.Drawing;


namespace JGEngineEditor.Source.MainApp
{
    public partial class RenderBoxController
    {
        /*  
         enum : CollisionPartType
         Exp : 현재 마우스 포인터가 렌더링 박스와 어디부분에 충돌하고있는지를 알려주는 타입
             */
        public enum CollisionPartType
        {
            Up,       Down,
            Right,    Left,
            LeftUp,   LeftDown,
            RightUp,  RightDown,
            NoneCollision
        }
        private PictureBox m_Controller = null;
        CollisionPartType m_CrrentCollisionType = CollisionPartType.NoneCollision;
        private Rectangle m_CollisonBox = new Rectangle(0, 0, 0, 0);
        private int m_CollisonRange = 15;
        private bool m_bChainging = false;
        private Point m_MousePos;
        public CollisionPartType GetCurrentCollisionType()
        {
            return m_CrrentCollisionType;
        }
        public void ConnectRenderBox(ref PictureBox Box)
        {
            m_Controller = Box;
        }

        // 마우스 포인터와 충돌 체크
        public void MousePointCollisonCheck(MouseEventArgs e)
        {
            m_CollisonBox.X = m_Controller.Location.X - m_CollisonRange;
            m_CollisonBox.Y = m_Controller.Location.Y - m_CollisonRange;
            m_CollisonBox.Width = m_Controller.Width + (m_CollisonRange * 2);
            m_CollisonBox.Height = m_Controller.Height + (m_CollisonRange * 2);
            if(m_bChainging)
            {
                ChangeingBoxSize(e);
                return;
            }
            // 보기 편하게 다시 정의
            int CBLeft = m_CollisonBox.X;
            int CBRight = CBLeft + m_CollisonBox.Width;
            int CBTop = m_CollisonBox.Y;
            int CBBottom = CBTop + m_CollisonBox.Height;
            int RBLeft = m_Controller.Location.X;
            int RBRight = RBLeft + m_Controller.Width; ;
            int RBTop = m_Controller.Location.Y;
            int RBBottom = RBTop + m_Controller.Height;

            bool result = true;
            m_CrrentCollisionType = CollisionPartType.NoneCollision;
            // TopBottom SizeNS
            result = MouseAndRenderBox_CollisionJugdement(e,
                     CBTop, RBLeft,
                     RBTop, RBRight);
            if (result)
            {
                m_CrrentCollisionType = CollisionPartType.Up;
                Cursor.Current = Cursors.SizeNS;
            }
            result = MouseAndRenderBox_CollisionJugdement(e,
                     RBBottom, RBLeft,
                     CBBottom, RBRight);
            if (result)
            {
                m_CrrentCollisionType = CollisionPartType.Down;
                Cursor.Current = Cursors.SizeNS;
            }
            // RightLeft SizeWE
            result = MouseAndRenderBox_CollisionJugdement(e,
                     RBTop, CBLeft,
                     RBBottom, RBLeft);
            if (result)
            {
                m_CrrentCollisionType = CollisionPartType.Left;
                Cursor.Current = Cursors.SizeWE;
            }
            result = MouseAndRenderBox_CollisionJugdement(e,
                     RBTop, RBRight,
                     RBBottom, CBRight);
            if (result)
            {
                m_CrrentCollisionType = CollisionPartType.Right;
                Cursor.Current = Cursors.SizeWE;
            }


            // TopLeft / RightBootm =  SizeNWSE
            result = MouseAndRenderBox_CollisionJugdement(e,
                     CBTop, CBLeft,
                     RBTop, RBLeft);
            if (result)
            {
                m_CrrentCollisionType = CollisionPartType.LeftUp;
                Cursor.Current = Cursors.SizeNWSE;
            }
            result = MouseAndRenderBox_CollisionJugdement(e,
                     RBBottom, RBRight,
                     CBBottom, CBRight);
            if (result)
            {
                m_CrrentCollisionType = CollisionPartType.RightDown;
                Cursor.Current = Cursors.SizeNWSE;
            }
            // TopRight / LeftBottom = SizeNESW
            result = MouseAndRenderBox_CollisionJugdement(e,
                     CBTop, RBRight,
                     RBTop, CBRight);
            if (result)
            {
                m_CrrentCollisionType = CollisionPartType.RightUp;
                Cursor.Current = Cursors.SizeNESW;
            }
            result = MouseAndRenderBox_CollisionJugdement(e,
                     RBBottom, CBLeft,
                     CBBottom, RBLeft);
            if (result)
            {
                m_CrrentCollisionType = CollisionPartType.LeftDown;
                Cursor.Current = Cursors.SizeNESW;
            }
        }
        public void ChangeingBoxSize(MouseEventArgs e)
        {
            int deltaX = m_MousePos.X - e.X;
            int deltaY = m_MousePos.Y - e.Y;

            int deltaArgX = 0;
            int deltaArgY = 0;
            if (deltaX != 0 && deltaY != 0)
            {
                deltaArgX = (deltaX * 5) / (Math.Abs(deltaX) + Math.Abs(deltaY));
                deltaArgY = (deltaY * 5) / (Math.Abs(deltaX) + Math.Abs(deltaY));
            }
            m_MousePos = e.Location;


            switch (m_CrrentCollisionType)
            {
                case CollisionPartType.Up:
                    ChangeUp(deltaY);
                    break;
                case CollisionPartType.Down:
                    ChangeDown(deltaY);
                    break;
                case CollisionPartType.Left:
                    ChangeLeft(deltaX);
                    break;
                case CollisionPartType.Right:
                    ChangeRight(deltaX);
                    break;
                case CollisionPartType.LeftUp:
                    ChangeLeft(deltaArgX);
                    ChangeUp(deltaArgY);
                    break;
                case CollisionPartType.LeftDown:
                    ChangeLeft(deltaX);
                    ChangeDown(deltaY);
                    break;
                case CollisionPartType.RightUp:
                    ChangeRight(deltaX);
                    ChangeUp(deltaArgY);
                    break;
                case CollisionPartType.RightDown:
                    ChangeRight(deltaX);
                    ChangeDown(deltaY);
                    break;
            }
        }
        public void ChangeUp(int deltaY)
        {
            m_Controller.Location = new Point(
            m_Controller.Location.X, m_Controller.Location.Y - deltaY);
            m_Controller.Height += deltaY;
        }
        public void ChangeDown(int deltaY)
        {
            m_Controller.Height -= deltaY;
        }
        public void ChangeLeft(int deltaX)
        {
            m_Controller.Location = new Point(
            m_Controller.Location.X - deltaX, m_Controller.Location.Y);
            m_Controller.Width += deltaX;
        }
        public void ChangeRight(int deltaX)
        {
            m_Controller.Width -= deltaX;
        }
        public void ChangeBoxSizeEnd()
        {
            m_bChainging = false;
        }
        public void ChangeBoxSizeStart(MouseEventArgs e)
        {
            m_MousePos = e.Location;
            m_bChainging = true;
        }

        // 충돌 판정
        private bool MouseAndRenderBox_CollisionJugdement(MouseEventArgs e, int top, int left, int bottom, int right)
        {
            if ((left < e.X && e.X < right) &&
                (top < e.Y && e.Y < bottom))
            {
                return true;
            }
            return false;
        }

    }

}