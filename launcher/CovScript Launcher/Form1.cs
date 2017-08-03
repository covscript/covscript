using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using System.Windows.Forms;
using System.Diagnostics;
using System.IO;

namespace CovScript_Launcher
{
    public partial class Form1 : Form
    {
        private string path="";
        public Form1()
        {
            InitializeComponent();
        }

        private void button1_Click(object sender, EventArgs e)
        {
            path = "";
            richTextBox1.Text = "";
        }

        private void button2_Click(object sender, EventArgs e)
        {
            OpenFileDialog ofd = new OpenFileDialog();
            ofd.Filter = "CovScript Source Code(*.csc)|*.csc|所有文件|*.*";
            ofd.ValidateNames = true;
            ofd.CheckPathExists = true;
            ofd.CheckFileExists = false;
            if (ofd.ShowDialog() == DialogResult.OK)
            {
                path = ofd.FileName;
                richTextBox1.Text = File.ReadAllText(path);
            }
        }

        private void button3_Click(object sender, EventArgs e)
        {
            if(path=="")
            {
                SaveFileDialog sfd = new SaveFileDialog();
                sfd.Filter = "CovScript Source Code(*.csc)|*.csc|所有文件|*.*";
                if (sfd.ShowDialog() == DialogResult.OK)
                {
                    path = sfd.FileName;
                    File.WriteAllText(path, richTextBox1.Text);
                }
            }else
                File.WriteAllText(path, richTextBox1.Text);
        }

        private void button4_Click(object sender, EventArgs e)
        {
            File.WriteAllText(Application.StartupPath + "/init.csc", richTextBox1.Text);
            Process.Start(Application.StartupPath + "/init.bat").WaitForExit();
            File.Delete(Application.StartupPath + "/init.csc");
        }
    }
}
