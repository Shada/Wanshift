namespace Launcher
{
	partial class Launcher
	{
		/// <summary>
		/// Required designer variable.
		/// </summary>
		private System.ComponentModel.IContainer components = null;

		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		/// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
		protected override void Dispose(bool disposing)
		{
			if (disposing && (components != null))
			{
				components.Dispose();
			}
			base.Dispose(disposing);
		}

		#region Windows Form Designer generated code

		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		private void InitializeComponent()
		{
			this.components = new System.ComponentModel.Container();
			this.btn_close = new System.Windows.Forms.PictureBox();
			this.btn_launch = new System.Windows.Forms.PictureBox();
			this.label1 = new System.Windows.Forms.Label();
			this.label2 = new System.Windows.Forms.Label();
			this.label3 = new System.Windows.Forms.Label();
			this.label4 = new System.Windows.Forms.Label();
			this.lb_resRight = new System.Windows.Forms.Label();
			this.lb_resLeft = new System.Windows.Forms.Label();
			this.lb_resolution = new System.Windows.Forms.Label();
			this.lb_screenMode = new System.Windows.Forms.Label();
			this.label5 = new System.Windows.Forms.Label();
			this.label6 = new System.Windows.Forms.Label();
			this.label7 = new System.Windows.Forms.Label();
			this.label8 = new System.Windows.Forms.Label();
			this.label9 = new System.Windows.Forms.Label();
			this.label10 = new System.Windows.Forms.Label();
			this.label11 = new System.Windows.Forms.Label();
			this.label12 = new System.Windows.Forms.Label();
			this.lb_keyUse = new System.Windows.Forms.Label();
			this.lb_keyJump = new System.Windows.Forms.Label();
			this.lb_keySprint = new System.Windows.Forms.Label();
			this.lb_keyRight = new System.Windows.Forms.Label();
			this.lb_keyLeft = new System.Windows.Forms.Label();
			this.lb_keyback = new System.Windows.Forms.Label();
			this.lb_keyFwd = new System.Windows.Forms.Label();
			this.lb_keyChange = new System.Windows.Forms.Label();
			this.timer = new System.Windows.Forms.Timer(this.components);
			((System.ComponentModel.ISupportInitialize)(this.btn_close)).BeginInit();
			((System.ComponentModel.ISupportInitialize)(this.btn_launch)).BeginInit();
			this.SuspendLayout();
			// 
			// btn_close
			// 
			this.btn_close.BackColor = System.Drawing.Color.Transparent;
			this.btn_close.BackgroundImageLayout = System.Windows.Forms.ImageLayout.None;
			this.btn_close.Image = global::Launcher.Properties.Resources.close;
			this.btn_close.Location = new System.Drawing.Point(1184, 9);
			this.btn_close.Name = "btn_close";
			this.btn_close.Size = new System.Drawing.Size(40, 40);
			this.btn_close.TabIndex = 0;
			this.btn_close.TabStop = false;
			this.btn_close.Click += new System.EventHandler(this.btn_close_Click);
			this.btn_close.MouseEnter += new System.EventHandler(this.btn_close_MouseEnter);
			this.btn_close.MouseLeave += new System.EventHandler(this.btn_close_MouseLeave);
			// 
			// btn_launch
			// 
			this.btn_launch.BackColor = System.Drawing.Color.Transparent;
			this.btn_launch.BackgroundImageLayout = System.Windows.Forms.ImageLayout.Zoom;
			this.btn_launch.Image = global::Launcher.Properties.Resources.Launch;
			this.btn_launch.Location = new System.Drawing.Point(924, 506);
			this.btn_launch.Name = "btn_launch";
			this.btn_launch.Size = new System.Drawing.Size(300, 100);
			this.btn_launch.SizeMode = System.Windows.Forms.PictureBoxSizeMode.Zoom;
			this.btn_launch.TabIndex = 4;
			this.btn_launch.TabStop = false;
			this.btn_launch.Click += new System.EventHandler(this.btn_launch_Click);
			this.btn_launch.MouseEnter += new System.EventHandler(this.btn_launch_MouseEnter);
			this.btn_launch.MouseLeave += new System.EventHandler(this.btn_launch_MouseLeave);
			// 
			// label1
			// 
			this.label1.AutoSize = true;
			this.label1.BackColor = System.Drawing.Color.Transparent;
			this.label1.Font = new System.Drawing.Font("Calibri", 72F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.label1.ForeColor = System.Drawing.Color.Sienna;
			this.label1.Location = new System.Drawing.Point(12, 9);
			this.label1.Name = "label1";
			this.label1.Size = new System.Drawing.Size(364, 117);
			this.label1.TabIndex = 10;
			this.label1.Text = "Settings";
			// 
			// label2
			// 
			this.label2.AutoSize = true;
			this.label2.BackColor = System.Drawing.Color.Transparent;
			this.label2.Font = new System.Drawing.Font("Calibri", 30F, System.Drawing.FontStyle.Bold);
			this.label2.ForeColor = System.Drawing.Color.Sienna;
			this.label2.Location = new System.Drawing.Point(23, 126);
			this.label2.Name = "label2";
			this.label2.Size = new System.Drawing.Size(165, 49);
			this.label2.TabIndex = 11;
			this.label2.Text = "Graphics";
			// 
			// label3
			// 
			this.label3.AutoSize = true;
			this.label3.BackColor = System.Drawing.Color.Transparent;
			this.label3.Font = new System.Drawing.Font("Calibri", 20F, System.Drawing.FontStyle.Bold);
			this.label3.ForeColor = System.Drawing.Color.Sienna;
			this.label3.Location = new System.Drawing.Point(52, 175);
			this.label3.Name = "label3";
			this.label3.Size = new System.Drawing.Size(169, 33);
			this.label3.TabIndex = 12;
			this.label3.Text = "Screen mode:";
			// 
			// label4
			// 
			this.label4.AutoSize = true;
			this.label4.BackColor = System.Drawing.Color.Transparent;
			this.label4.Font = new System.Drawing.Font("Calibri", 20F, System.Drawing.FontStyle.Bold);
			this.label4.ForeColor = System.Drawing.Color.Sienna;
			this.label4.Location = new System.Drawing.Point(52, 208);
			this.label4.Name = "label4";
			this.label4.Size = new System.Drawing.Size(143, 33);
			this.label4.TabIndex = 13;
			this.label4.Text = "Resolution:";
			// 
			// lb_resRight
			// 
			this.lb_resRight.AutoSize = true;
			this.lb_resRight.BackColor = System.Drawing.Color.Transparent;
			this.lb_resRight.Font = new System.Drawing.Font("Wingdings", 20F);
			this.lb_resRight.ForeColor = System.Drawing.Color.Sienna;
			this.lb_resRight.Location = new System.Drawing.Point(433, 210);
			this.lb_resRight.Name = "lb_resRight";
			this.lb_resRight.Size = new System.Drawing.Size(42, 30);
			this.lb_resRight.TabIndex = 15;
			this.lb_resRight.Text = "è";
			this.lb_resRight.Click += new System.EventHandler(this.lb_resRight_Click);
			this.lb_resRight.MouseEnter += new System.EventHandler(this.lb_resRight_MouseEnter);
			this.lb_resRight.MouseLeave += new System.EventHandler(this.lb_resRight_MouseLeave);
			// 
			// lb_resLeft
			// 
			this.lb_resLeft.AutoSize = true;
			this.lb_resLeft.BackColor = System.Drawing.Color.Transparent;
			this.lb_resLeft.Font = new System.Drawing.Font("Wingdings", 20F);
			this.lb_resLeft.ForeColor = System.Drawing.Color.Sienna;
			this.lb_resLeft.Location = new System.Drawing.Point(228, 210);
			this.lb_resLeft.Name = "lb_resLeft";
			this.lb_resLeft.Size = new System.Drawing.Size(42, 30);
			this.lb_resLeft.TabIndex = 16;
			this.lb_resLeft.Text = "ç";
			this.lb_resLeft.Click += new System.EventHandler(this.lb_resLeft_Click);
			this.lb_resLeft.MouseEnter += new System.EventHandler(this.lb_resLeft_MouseEnter);
			this.lb_resLeft.MouseLeave += new System.EventHandler(this.lb_resLeft_MouseLeave);
			// 
			// lb_resolution
			// 
			this.lb_resolution.BackColor = System.Drawing.Color.Transparent;
			this.lb_resolution.Font = new System.Drawing.Font("Calibri", 20F, System.Drawing.FontStyle.Bold);
			this.lb_resolution.ForeColor = System.Drawing.Color.Sienna;
			this.lb_resolution.Location = new System.Drawing.Point(276, 208);
			this.lb_resolution.Name = "lb_resolution";
			this.lb_resolution.Size = new System.Drawing.Size(151, 33);
			this.lb_resolution.TabIndex = 17;
			this.lb_resolution.Text = "800 x 600";
			this.lb_resolution.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
			// 
			// lb_screenMode
			// 
			this.lb_screenMode.AutoSize = true;
			this.lb_screenMode.BackColor = System.Drawing.Color.Transparent;
			this.lb_screenMode.Font = new System.Drawing.Font("Calibri", 20F, System.Drawing.FontStyle.Bold);
			this.lb_screenMode.ForeColor = System.Drawing.Color.Sienna;
			this.lb_screenMode.Location = new System.Drawing.Point(227, 175);
			this.lb_screenMode.Name = "lb_screenMode";
			this.lb_screenMode.Size = new System.Drawing.Size(129, 33);
			this.lb_screenMode.TabIndex = 18;
			this.lb_screenMode.Text = "Fullscreen";
			this.lb_screenMode.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
			this.lb_screenMode.Click += new System.EventHandler(this.lb_screenMode_Click);
			this.lb_screenMode.MouseEnter += new System.EventHandler(this.lb_screenMode_MouseEnter);
			this.lb_screenMode.MouseLeave += new System.EventHandler(this.lb_screenMode_MouseLeave);
			// 
			// label5
			// 
			this.label5.AutoSize = true;
			this.label5.BackColor = System.Drawing.Color.Transparent;
			this.label5.Font = new System.Drawing.Font("Calibri", 30F, System.Drawing.FontStyle.Bold);
			this.label5.ForeColor = System.Drawing.Color.Sienna;
			this.label5.Location = new System.Drawing.Point(23, 241);
			this.label5.Name = "label5";
			this.label5.Size = new System.Drawing.Size(162, 49);
			this.label5.TabIndex = 19;
			this.label5.Text = "Controls";
			// 
			// label6
			// 
			this.label6.AutoSize = true;
			this.label6.BackColor = System.Drawing.Color.Transparent;
			this.label6.Font = new System.Drawing.Font("Calibri", 20F, System.Drawing.FontStyle.Bold);
			this.label6.ForeColor = System.Drawing.Color.Sienna;
			this.label6.Location = new System.Drawing.Point(52, 290);
			this.label6.Name = "label6";
			this.label6.Size = new System.Drawing.Size(109, 33);
			this.label6.TabIndex = 20;
			this.label6.Text = "Forward";
			// 
			// label7
			// 
			this.label7.AutoSize = true;
			this.label7.BackColor = System.Drawing.Color.Transparent;
			this.label7.Font = new System.Drawing.Font("Calibri", 20F, System.Drawing.FontStyle.Bold);
			this.label7.ForeColor = System.Drawing.Color.Sienna;
			this.label7.Location = new System.Drawing.Point(52, 323);
			this.label7.Name = "label7";
			this.label7.Size = new System.Drawing.Size(124, 33);
			this.label7.TabIndex = 21;
			this.label7.Text = "Backward";
			// 
			// label8
			// 
			this.label8.AutoSize = true;
			this.label8.BackColor = System.Drawing.Color.Transparent;
			this.label8.Font = new System.Drawing.Font("Calibri", 20F, System.Drawing.FontStyle.Bold);
			this.label8.ForeColor = System.Drawing.Color.Sienna;
			this.label8.Location = new System.Drawing.Point(52, 356);
			this.label8.Name = "label8";
			this.label8.Size = new System.Drawing.Size(130, 33);
			this.label8.TabIndex = 22;
			this.label8.Text = "Strafe Left";
			// 
			// label9
			// 
			this.label9.AutoSize = true;
			this.label9.BackColor = System.Drawing.Color.Transparent;
			this.label9.Font = new System.Drawing.Font("Calibri", 20F, System.Drawing.FontStyle.Bold);
			this.label9.ForeColor = System.Drawing.Color.Sienna;
			this.label9.Location = new System.Drawing.Point(52, 389);
			this.label9.Name = "label9";
			this.label9.Size = new System.Drawing.Size(146, 33);
			this.label9.TabIndex = 23;
			this.label9.Text = "Strafe Right";
			// 
			// label10
			// 
			this.label10.AutoSize = true;
			this.label10.BackColor = System.Drawing.Color.Transparent;
			this.label10.Font = new System.Drawing.Font("Calibri", 20F, System.Drawing.FontStyle.Bold);
			this.label10.ForeColor = System.Drawing.Color.Sienna;
			this.label10.Location = new System.Drawing.Point(52, 422);
			this.label10.Name = "label10";
			this.label10.Size = new System.Drawing.Size(82, 33);
			this.label10.TabIndex = 24;
			this.label10.Text = "Sprint";
			// 
			// label11
			// 
			this.label11.AutoSize = true;
			this.label11.BackColor = System.Drawing.Color.Transparent;
			this.label11.Font = new System.Drawing.Font("Calibri", 20F, System.Drawing.FontStyle.Bold);
			this.label11.ForeColor = System.Drawing.Color.Sienna;
			this.label11.Location = new System.Drawing.Point(52, 455);
			this.label11.Name = "label11";
			this.label11.Size = new System.Drawing.Size(74, 33);
			this.label11.TabIndex = 25;
			this.label11.Text = "Jump";
			// 
			// label12
			// 
			this.label12.AutoSize = true;
			this.label12.BackColor = System.Drawing.Color.Transparent;
			this.label12.Font = new System.Drawing.Font("Calibri", 20F, System.Drawing.FontStyle.Bold);
			this.label12.ForeColor = System.Drawing.Color.Sienna;
			this.label12.Location = new System.Drawing.Point(52, 488);
			this.label12.Name = "label12";
			this.label12.Size = new System.Drawing.Size(152, 33);
			this.label12.TabIndex = 26;
			this.label12.Text = "Use / Attack";
			// 
			// lb_keyUse
			// 
			this.lb_keyUse.AutoSize = true;
			this.lb_keyUse.BackColor = System.Drawing.Color.Transparent;
			this.lb_keyUse.Font = new System.Drawing.Font("Calibri", 20F, System.Drawing.FontStyle.Bold);
			this.lb_keyUse.ForeColor = System.Drawing.Color.Sienna;
			this.lb_keyUse.Location = new System.Drawing.Point(210, 488);
			this.lb_keyUse.Name = "lb_keyUse";
			this.lb_keyUse.Size = new System.Drawing.Size(28, 33);
			this.lb_keyUse.TabIndex = 27;
			this.lb_keyUse.Text = "E";
			this.lb_keyUse.Click += new System.EventHandler(this.lb_keyUse_Click);
			this.lb_keyUse.MouseEnter += new System.EventHandler(this.lb_keyUse_MouseEnter);
			this.lb_keyUse.MouseLeave += new System.EventHandler(this.lb_keyUse_MouseLeave);
			// 
			// lb_keyJump
			// 
			this.lb_keyJump.AutoSize = true;
			this.lb_keyJump.BackColor = System.Drawing.Color.Transparent;
			this.lb_keyJump.Font = new System.Drawing.Font("Calibri", 20F, System.Drawing.FontStyle.Bold);
			this.lb_keyJump.ForeColor = System.Drawing.Color.Sienna;
			this.lb_keyJump.Location = new System.Drawing.Point(210, 455);
			this.lb_keyJump.Name = "lb_keyJump";
			this.lb_keyJump.Size = new System.Drawing.Size(83, 33);
			this.lb_keyJump.TabIndex = 28;
			this.lb_keyJump.Text = "SPACE";
			this.lb_keyJump.Click += new System.EventHandler(this.lb_keyJump_Click);
			this.lb_keyJump.MouseEnter += new System.EventHandler(this.lb_keyJump_MouseEnter);
			this.lb_keyJump.MouseLeave += new System.EventHandler(this.lb_keyJump_MouseLeave);
			// 
			// lb_keySprint
			// 
			this.lb_keySprint.AutoSize = true;
			this.lb_keySprint.BackColor = System.Drawing.Color.Transparent;
			this.lb_keySprint.Font = new System.Drawing.Font("Calibri", 20F, System.Drawing.FontStyle.Bold);
			this.lb_keySprint.ForeColor = System.Drawing.Color.Sienna;
			this.lb_keySprint.Location = new System.Drawing.Point(210, 422);
			this.lb_keySprint.Name = "lb_keySprint";
			this.lb_keySprint.Size = new System.Drawing.Size(68, 33);
			this.lb_keySprint.TabIndex = 29;
			this.lb_keySprint.Text = "CTRL";
			this.lb_keySprint.Click += new System.EventHandler(this.lb_keySprint_Click);
			this.lb_keySprint.MouseEnter += new System.EventHandler(this.lb_keySprint_MouseEnter);
			this.lb_keySprint.MouseLeave += new System.EventHandler(this.lb_keySprint_MouseLeave);
			// 
			// lb_keyRight
			// 
			this.lb_keyRight.AutoSize = true;
			this.lb_keyRight.BackColor = System.Drawing.Color.Transparent;
			this.lb_keyRight.Font = new System.Drawing.Font("Calibri", 20F, System.Drawing.FontStyle.Bold);
			this.lb_keyRight.ForeColor = System.Drawing.Color.Sienna;
			this.lb_keyRight.Location = new System.Drawing.Point(210, 389);
			this.lb_keyRight.Name = "lb_keyRight";
			this.lb_keyRight.Size = new System.Drawing.Size(32, 33);
			this.lb_keyRight.TabIndex = 30;
			this.lb_keyRight.Text = "D";
			this.lb_keyRight.Click += new System.EventHandler(this.lb_keyRight_Click);
			this.lb_keyRight.MouseEnter += new System.EventHandler(this.lb_keyRight_MouseEnter);
			this.lb_keyRight.MouseLeave += new System.EventHandler(this.lb_keyRight_MouseLeave);
			// 
			// lb_keyLeft
			// 
			this.lb_keyLeft.AutoSize = true;
			this.lb_keyLeft.BackColor = System.Drawing.Color.Transparent;
			this.lb_keyLeft.Font = new System.Drawing.Font("Calibri", 20F, System.Drawing.FontStyle.Bold);
			this.lb_keyLeft.ForeColor = System.Drawing.Color.Sienna;
			this.lb_keyLeft.Location = new System.Drawing.Point(210, 356);
			this.lb_keyLeft.Name = "lb_keyLeft";
			this.lb_keyLeft.Size = new System.Drawing.Size(31, 33);
			this.lb_keyLeft.TabIndex = 31;
			this.lb_keyLeft.Text = "A";
			this.lb_keyLeft.Click += new System.EventHandler(this.lb_keyLeft_Click);
			this.lb_keyLeft.MouseEnter += new System.EventHandler(this.lb_keyLeft_MouseEnter);
			this.lb_keyLeft.MouseLeave += new System.EventHandler(this.lb_keyLeft_MouseLeave);
			// 
			// lb_keyback
			// 
			this.lb_keyback.AutoSize = true;
			this.lb_keyback.BackColor = System.Drawing.Color.Transparent;
			this.lb_keyback.Font = new System.Drawing.Font("Calibri", 20F, System.Drawing.FontStyle.Bold);
			this.lb_keyback.ForeColor = System.Drawing.Color.Sienna;
			this.lb_keyback.Location = new System.Drawing.Point(215, 323);
			this.lb_keyback.Name = "lb_keyback";
			this.lb_keyback.Size = new System.Drawing.Size(28, 33);
			this.lb_keyback.TabIndex = 32;
			this.lb_keyback.Text = "S";
			this.lb_keyback.Click += new System.EventHandler(this.lb_keyback_Click);
			this.lb_keyback.MouseEnter += new System.EventHandler(this.lb_keyback_MouseEnter);
			this.lb_keyback.MouseLeave += new System.EventHandler(this.lb_keyback_MouseLeave);
			// 
			// lb_keyFwd
			// 
			this.lb_keyFwd.AutoSize = true;
			this.lb_keyFwd.BackColor = System.Drawing.Color.Transparent;
			this.lb_keyFwd.Font = new System.Drawing.Font("Calibri", 20F, System.Drawing.FontStyle.Bold);
			this.lb_keyFwd.ForeColor = System.Drawing.Color.Sienna;
			this.lb_keyFwd.Location = new System.Drawing.Point(210, 290);
			this.lb_keyFwd.Name = "lb_keyFwd";
			this.lb_keyFwd.Size = new System.Drawing.Size(39, 33);
			this.lb_keyFwd.TabIndex = 33;
			this.lb_keyFwd.Text = "W";
			this.lb_keyFwd.Click += new System.EventHandler(this.lb_keyFwd_Click);
			this.lb_keyFwd.MouseEnter += new System.EventHandler(this.lb_keyFwd_MouseEnter);
			this.lb_keyFwd.MouseLeave += new System.EventHandler(this.lb_keyFwd_MouseLeave);
			// 
			// lb_keyChange
			// 
			this.lb_keyChange.AutoSize = true;
			this.lb_keyChange.BackColor = System.Drawing.Color.Transparent;
			this.lb_keyChange.Font = new System.Drawing.Font("Calibri", 12F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.lb_keyChange.ForeColor = System.Drawing.Color.Red;
			this.lb_keyChange.Location = new System.Drawing.Point(54, 590);
			this.lb_keyChange.Name = "lb_keyChange";
			this.lb_keyChange.Size = new System.Drawing.Size(214, 19);
			this.lb_keyChange.TabIndex = 34;
			this.lb_keyChange.Text = "Press the key you want to use";
			this.lb_keyChange.Visible = false;
			// 
			// timer
			// 
			this.timer.Interval = 3000;
			this.timer.Tick += new System.EventHandler(this.timer_Tick);
			// 
			// Launcher
			// 
			this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.BackColor = System.Drawing.Color.Black;
			this.BackgroundImage = global::Launcher.Properties.Resources.wanshift_promo;
			this.ClientSize = new System.Drawing.Size(1236, 618);
			this.Controls.Add(this.lb_keyChange);
			this.Controls.Add(this.lb_keyFwd);
			this.Controls.Add(this.lb_keyback);
			this.Controls.Add(this.lb_keyLeft);
			this.Controls.Add(this.lb_keyRight);
			this.Controls.Add(this.lb_keySprint);
			this.Controls.Add(this.lb_keyJump);
			this.Controls.Add(this.lb_keyUse);
			this.Controls.Add(this.label12);
			this.Controls.Add(this.label11);
			this.Controls.Add(this.label10);
			this.Controls.Add(this.label9);
			this.Controls.Add(this.label8);
			this.Controls.Add(this.label7);
			this.Controls.Add(this.label6);
			this.Controls.Add(this.label5);
			this.Controls.Add(this.lb_screenMode);
			this.Controls.Add(this.lb_resolution);
			this.Controls.Add(this.lb_resLeft);
			this.Controls.Add(this.lb_resRight);
			this.Controls.Add(this.label4);
			this.Controls.Add(this.label3);
			this.Controls.Add(this.label2);
			this.Controls.Add(this.label1);
			this.Controls.Add(this.btn_launch);
			this.Controls.Add(this.btn_close);
			this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.None;
			this.Name = "Launcher";
			this.TransparencyKey = System.Drawing.Color.Transparent;
			this.KeyDown += new System.Windows.Forms.KeyEventHandler(this.Launcher_KeyDown);
			((System.ComponentModel.ISupportInitialize)(this.btn_close)).EndInit();
			((System.ComponentModel.ISupportInitialize)(this.btn_launch)).EndInit();
			this.ResumeLayout(false);
			this.PerformLayout();

		}

		#endregion

		private System.Windows.Forms.PictureBox btn_close;
		private System.Windows.Forms.PictureBox btn_launch;
		private System.Windows.Forms.Label label1;
		private System.Windows.Forms.Label label2;
		private System.Windows.Forms.Label label3;
		private System.Windows.Forms.Label label4;
		private System.Windows.Forms.Label lb_resRight;
		private System.Windows.Forms.Label lb_resLeft;
		private System.Windows.Forms.Label lb_resolution;
		private System.Windows.Forms.Label lb_screenMode;
		private System.Windows.Forms.Label label5;
		private System.Windows.Forms.Label label6;
		private System.Windows.Forms.Label label7;
		private System.Windows.Forms.Label label8;
		private System.Windows.Forms.Label label9;
		private System.Windows.Forms.Label label10;
		private System.Windows.Forms.Label label11;
		private System.Windows.Forms.Label label12;
		private System.Windows.Forms.Label lb_keyUse;
		private System.Windows.Forms.Label lb_keyJump;
		private System.Windows.Forms.Label lb_keySprint;
		private System.Windows.Forms.Label lb_keyRight;
		private System.Windows.Forms.Label lb_keyLeft;
		private System.Windows.Forms.Label lb_keyback;
		private System.Windows.Forms.Label lb_keyFwd;
		private System.Windows.Forms.Label lb_keyChange;
		private System.Windows.Forms.Timer timer;
	}
}

