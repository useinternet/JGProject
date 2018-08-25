using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using System.Windows.Forms;
using JGEngineEditor.Source.MainApp;

namespace JGEngineEditor
{
    static class Program
    {
        /// <summary>
        /// 해당 응용 프로그램의 주 진입점입니다.
        /// </summary>
        [STAThread]
        static void Main()
        {
            Application.EnableVisualStyles();
            Application.SetCompatibleTextRenderingDefault(false);

            using (MainForm mainForm = new MainForm())
            using (MainApp mainApp = new MainApp())
            {
                if(mainApp.InitializeApplication(mainForm))
                {
                    mainForm.Show();


                    while(mainForm.Created)
                    {
                        mainApp.MainLoop();

                        Application.DoEvents();
                    }
                }
                else
                {

                }
                
            }
        }
    }
}
