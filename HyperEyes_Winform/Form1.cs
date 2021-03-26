using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace KoreanEyes_Winform
{
    public partial class Form1 : Form
    {
        public Form1()
        {
            InitializeComponent();

            this.tmpBtn.Click += TmpBtn_Click;
            this.SizeChanged += Form1_SizeChanged;
        }

        private void Form1_SizeChanged(object sender, EventArgs e)
        {
            OnResize((uint)this.tmpPanel.Width, (uint)this.tmpPanel.Height);
        }

        private void TmpBtn_Click(object sender, EventArgs e)
        {
            CreateInstance(this.tmpPanel.Handle);
            Initialize();
            //OnRender();
            Play();
        }

        [DllImport("HyperRenderer.dll")]
        public static extern int CreateInstance(IntPtr hWnd);

        [DllImport("HyperRenderer.dll")]
        public static extern int Initialize();

        [DllImport("HyperRenderer.dll")]
        public static extern int OnRender();

        [DllImport("HyperRenderer.dll")]
        public static extern int OnResize(uint width, uint height);

        [DllImport("HyperRenderer.dll")]
        public static extern int Play();
    }
}
