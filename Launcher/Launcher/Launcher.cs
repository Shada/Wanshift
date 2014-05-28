using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.IO;
using System.Diagnostics;

namespace Launcher
{
	public partial class Launcher : Form
	{
#region magic trickery
		private const int WM_NCHITTEST = 0x84;
		private const int HTCLIENT = 0x1;
		private const int HTCAPTION = 0x2;

		protected override void WndProc(ref Message message)
		{
			base.WndProc(ref message);

			if (message.Msg == WM_NCHITTEST && (int)message.Result == HTCLIENT)
				message.Result = (IntPtr)HTCAPTION;
		}		
#endregion

		enum ScreenMode : int
		{
			FULLSCREEN = 0,
			WINDOWED
		}

		enum ScreenResolution : int
		{
			_800X600,
			_1024X768,
			_1280X720,
			_1280X1024,
			_1366X768,
			_1440X900,
			_1680X1050,
			_1920X1080,
			_1920X1200
		}

		enum ControlKey : int
		{
			NON,
			FORWARD,
			BACKWARD,
			STRAFE_LEFT,
			STRAFE_RIGHT,
			SPRINT,
			JUMP,
			USE
		}

		struct Key
		{
			public string name;
			public int value;
		}

		struct ResolutionPair
		{
			public string showName, fileName;

			public ResolutionPair(string _show, string _file)
			{
				showName = _show;
				fileName = _file;
			}
		}

		ScreenMode scMode;
		ScreenResolution ScRes = ScreenResolution._800X600;

		Key fwd, back, strafeL, strafeR, sprint, jump, use;

		int scResIndex = 0;
		List<string> resText = new List<string>();
		ControlKey currKey = ControlKey.NON;
		Dictionary<int, string> keysDict = new Dictionary<int, string>();
		Dictionary<ScreenResolution, ResolutionPair> resDict = new Dictionary<ScreenResolution, ResolutionPair>();
		Dictionary<string, ScreenResolution> showResDict = new Dictionary<string, ScreenResolution>();

		public Launcher()
		{
			InitializeComponent();
			this.SetStyle(ControlStyles.SupportsTransparentBackColor, true);
			lb_keyChange.Visible = false;
#region Init dicts

			resDict.Add(ScreenResolution._800X600, new ResolutionPair("800 x 600", "800:600"));
			resDict.Add(ScreenResolution._1024X768, new ResolutionPair("1024 x 768", "1024:768"));
			resDict.Add(ScreenResolution._1280X720, new ResolutionPair("1280 x 720", "1280:720"));
			resDict.Add(ScreenResolution._1280X1024, new ResolutionPair("1280 x 1024", "1280:1024"));
			resDict.Add(ScreenResolution._1366X768, new ResolutionPair("1366 x 768", "1366:768"));
			resDict.Add(ScreenResolution._1440X900, new ResolutionPair("1440 x 900", "1440:900"));
			resDict.Add(ScreenResolution._1680X1050, new ResolutionPair("1680 x 1050", "1680:1050"));
			resDict.Add(ScreenResolution._1920X1080, new ResolutionPair("1920 x 1080", "1920:1080"));
			resDict.Add(ScreenResolution._1920X1200, new ResolutionPair("1920 x 1200", "1920:1200"));

			showResDict.Add("800:600", ScreenResolution._800X600);
			showResDict.Add("1024:768", ScreenResolution._1024X768);
			showResDict.Add("1280 x 720", ScreenResolution._1280X720);
			showResDict.Add("1280:1024", ScreenResolution._1280X1024);
			showResDict.Add("1366:768", ScreenResolution._1366X768);
			showResDict.Add("1440:900", ScreenResolution._1440X900);
			showResDict.Add("1680:1050", ScreenResolution._1680X1050);
			showResDict.Add("1920:1080", ScreenResolution._1920X1080);
			showResDict.Add("1920:1200", ScreenResolution._1920X1200);

			keysDict.Add(8, "Backspace");
			keysDict.Add(9, "Tab");
			keysDict.Add(13, "Enter");
			keysDict.Add(16, "Shift");
			keysDict.Add(17, "Ctrl");
			keysDict.Add(18, "Alt");
			keysDict.Add(32, "Space");
			keysDict.Add(37, "Left Arrow");
			keysDict.Add(38, "Up Arrow");
			keysDict.Add(39, "Right Arrow");
			keysDict.Add(40, "Down Arrow");
			keysDict.Add(48, "0");
			keysDict.Add(49, "1");
			keysDict.Add(50, "2");
			keysDict.Add(51, "3");
			keysDict.Add(52, "4");
			keysDict.Add(53, "5");
			keysDict.Add(54, "6");
			keysDict.Add(55, "7");
			keysDict.Add(56, "8");
			keysDict.Add(57, "9");
			keysDict.Add(96, "Number pad 0");
			keysDict.Add(97, "Number pad 1");
			keysDict.Add(98, "Number pad 2");
			keysDict.Add(99, "Number pad 3");
			keysDict.Add(100, "Number pad 4");
			keysDict.Add(101, "Number pad 5");
			keysDict.Add(102, "Number pad 6");
			keysDict.Add(103, "Number pad 7");
			keysDict.Add(104, "Number pad 8");
			keysDict.Add(105, "Number pad 9");
			keysDict.Add(106, "Number pad multiply");
			keysDict.Add(107, "Number pad plus");
			keysDict.Add(109, "Number pad minus");
			keysDict.Add(111, "Number pad divide");
			keysDict.Add(112, "F1");
			keysDict.Add(113, "F2");
			keysDict.Add(114, "F3");
			keysDict.Add(115, "F4");
			keysDict.Add(116, "F5");
			keysDict.Add(117, "F6");
			keysDict.Add(118, "F7");
			keysDict.Add(119, "F8");
			keysDict.Add(120, "F9");
			keysDict.Add(121, "F10");
			keysDict.Add(122, "F11");
			keysDict.Add(123, "F12");
			keysDict.Add(187, "Plus");
			keysDict.Add(188, "Comma");
			keysDict.Add(189, "Minus");
			keysDict.Add(190, "Period");
			keysDict.Add(192, "Single quote");
#endregion

			List<string> file = new List<string>();

			using (StreamReader reader = new StreamReader(Environment.GetFolderPath(Environment.SpecialFolder.ApplicationData) + "\\WanShift\\user.ini"))
			{
				while (!reader.EndOfStream)
				{
					file.Add(reader.ReadLine());
				}
			}

			ResolutionPair resPair;

			showResDict.TryGetValue(getInitValue("resolution", file), out ScRes);
			resDict.TryGetValue(ScRes, out resPair);

			lb_resolution.Text = resPair.showName;
			scMode = (ScreenMode)Convert.ToInt32(getInitValue("windowed", file));

			fwd = getKey("fwd", file);
			back = getKey("back", file);
			strafeL = getKey("strafeL", file);
			strafeR = getKey("strafeR", file);
			sprint = getKey("sprint", file);
			jump = getKey("jump", file);
			use = getKey("use", file);

			lb_keyFwd.Text = fwd.name;
			lb_keyback.Text = back.name;
			lb_keyLeft.Text = strafeL.name;
			lb_keyRight.Text = strafeR.name;
			lb_keySprint.Text = sprint.name;
			lb_keyJump.Text = jump.name;
			lb_keyUse.Text = use.name;
		}

		private void btn_close_Click(object sender, EventArgs e)
		{
			this.Close();
		}

		private void btn_launch_Click(object sender, EventArgs e)
		{
			ResolutionPair res;
			resDict.TryGetValue(ScRes, out res);
			List<string> file = new List<string>();

			file.Add("# This is a user ini. It will override any attributes");
			file.Add("# already loaded. New attributes will be appended.");
			file.Add("");
			file.Add("[Graphics]");
			file.Add("resolution\t= " + res.fileName);
			file.Add("windowed\t= " + (int)scMode);
			file.Add("");
			file.Add("[Controls]");
			file.Add("fwd\t\t\t= " + fwd.value.ToString() + "\t# " + fwd.name);
			file.Add("back\t\t= " + back.value.ToString() + "\t# " + back.name);
			file.Add("strafeL\t\t= " + strafeL.value.ToString() + "\t# " + strafeL.name);
			file.Add("strafeR\t\t= " + strafeR.value.ToString() + "\t# " + strafeR.name);
			file.Add("sprint\t\t= " + sprint.value.ToString() + "\t# " + sprint.name);
			file.Add("jump\t\t= " + jump.value.ToString() + "\t# " + jump.name);
			file.Add("use\t\t\t= " + use.value.ToString() + "\t# " + use.name);

            using (StreamWriter writer = new StreamWriter(Environment.GetFolderPath(Environment.SpecialFolder.ApplicationData) + "\\WanShift\\user.ini"))
			{
				foreach(string s in file)
				{
					writer.WriteLine(s);
				}
			}

			//Process.Start(System.Environment.CurrentDirectory + "\\ClientIPC.exe");
			//System.Threading.Thread.Sleep(1000);
			Process.Start(System.Environment.CurrentDirectory + "\\WanShift.exe");
		}

		private void btn_close_MouseEnter(object sender, EventArgs e)
		{
			btn_close.Image = global::Launcher.Properties.Resources.close_down;
		}

		private void btn_close_MouseLeave(object sender, EventArgs e)
		{
			btn_close.Image = global::Launcher.Properties.Resources.close;
		}

		private void btn_launch_MouseEnter(object sender, EventArgs e)
		{
			btn_launch.Image = global::Launcher.Properties.Resources.Launch_down;
		}

		private void btn_launch_MouseLeave(object sender, EventArgs e)
		{
			btn_launch.Image = global::Launcher.Properties.Resources.Launch;
		}

		private void lb_resLeft_Click(object sender, EventArgs e)
		{
			if (scResIndex > 0)
			{
				scResIndex--;
				ScreenResolution scRes = (ScreenResolution)scResIndex;
				ResolutionPair res; 
				resDict.TryGetValue(scRes, out res);
				lb_resolution.Text = res.showName;
				ScRes = (ScreenResolution)scResIndex;
			}
		}

		private void lb_resRight_Click(object sender, EventArgs e)
		{
			if (scResIndex < 8)
			{
				scResIndex++;
				ScreenResolution scRes = (ScreenResolution)scResIndex;
				ResolutionPair res;
				resDict.TryGetValue(scRes, out res);
				lb_resolution.Text = res.showName;
				ScRes = (ScreenResolution)scResIndex;
			}
		}

		private void lb_screenMode_Click(object sender, EventArgs e)
		{
			if(scMode == ScreenMode.FULLSCREEN)
			{
				scMode = ScreenMode.WINDOWED;
				lb_screenMode.Text = "Windowed";
			}
			else if(scMode == ScreenMode.WINDOWED)
			{
				scMode = ScreenMode.FULLSCREEN;
				lb_screenMode.Text = "Fullscreen";
			}
		}

		private void lb_screenMode_MouseEnter(object sender, EventArgs e)
		{
			lb_screenMode.Font = new Font("Calibri", 19.0f, FontStyle.Bold);
		}

		private void lb_screenMode_MouseLeave(object sender, EventArgs e)
		{
			lb_screenMode.Font = new Font("Calibri", 20.0f, FontStyle.Bold);
		}

		private void lb_resLeft_MouseEnter(object sender, EventArgs e)
		{
			lb_resLeft.Font = new Font("Wingdings", 19.0f, FontStyle.Bold);
		}

		private void lb_resLeft_MouseLeave(object sender, EventArgs e)
		{
			lb_resLeft.Font = new Font("Wingdings", 20.0f, FontStyle.Bold);
		}

		private void lb_resRight_MouseEnter(object sender, EventArgs e)
		{
			lb_resRight.Font = new Font("Wingdings", 19.0f, FontStyle.Bold);
		}

		private void lb_resRight_MouseLeave(object sender, EventArgs e)
		{
			lb_resRight.Font = new Font("Wingdings", 20.0f, FontStyle.Bold);
		}

		private void lb_keyFwd_Click(object sender, EventArgs e)
		{
			controlKeyChange(ControlKey.FORWARD);
		}

		private void lb_keyFwd_MouseEnter(object sender, EventArgs e)
		{
			lb_keyFwd.Font = new Font("Calibri", 19.0f, FontStyle.Bold);
		}

		private void lb_keyFwd_MouseLeave(object sender, EventArgs e)
		{
			lb_keyFwd.Font = new Font("Calibri", 20.0f, FontStyle.Bold);
		}

		private void lb_keyback_Click(object sender, EventArgs e)
		{
			controlKeyChange(ControlKey.BACKWARD);
		}

		private void lb_keyback_MouseEnter(object sender, EventArgs e)
		{
			lb_keyback.Font = new Font("Calibri", 19.0f, FontStyle.Bold);
		}

		private void lb_keyback_MouseLeave(object sender, EventArgs e)
		{
			lb_keyback.Font = new Font("Calibri", 20.0f, FontStyle.Bold);
		}

		private void lb_keyLeft_Click(object sender, EventArgs e)
		{
			controlKeyChange(ControlKey.STRAFE_LEFT);
		}

		private void lb_keyLeft_MouseEnter(object sender, EventArgs e)
		{
			lb_keyLeft.Font = new Font("Calibri", 19.0f, FontStyle.Bold);
		}

		private void lb_keyLeft_MouseLeave(object sender, EventArgs e)
		{
			lb_keyLeft.Font = new Font("Calibri", 20.0f, FontStyle.Bold);
		}

		private void lb_keyRight_Click(object sender, EventArgs e)
		{
			controlKeyChange(ControlKey.STRAFE_RIGHT);
		}

		private void lb_keyRight_MouseEnter(object sender, EventArgs e)
		{
			lb_keyRight.Font = new Font("Calibri", 19.0f, FontStyle.Bold);
		}

		private void lb_keyRight_MouseLeave(object sender, EventArgs e)
		{
			lb_keyRight.Font = new Font("Calibri", 20.0f, FontStyle.Bold);
		}

		private void lb_keySprint_Click(object sender, EventArgs e)
		{
			controlKeyChange(ControlKey.SPRINT);
		}

		private void lb_keySprint_MouseEnter(object sender, EventArgs e)
		{
			lb_keySprint.Font = new Font("Calibri", 19.0f, FontStyle.Bold);
		}

		private void lb_keySprint_MouseLeave(object sender, EventArgs e)
		{
			lb_keySprint.Font = new Font("Calibri", 20.0f, FontStyle.Bold);
		}

		private void lb_keyJump_Click(object sender, EventArgs e)
		{
			controlKeyChange(ControlKey.JUMP);
		}

		private void lb_keyJump_MouseEnter(object sender, EventArgs e)
		{
			lb_keyJump.Font = new Font("Calibri", 19.0f, FontStyle.Bold);
		}

		private void lb_keyJump_MouseLeave(object sender, EventArgs e)
		{
			lb_keyJump.Font = new Font("Calibri", 20.0f, FontStyle.Bold);
		}

		private void lb_keyUse_Click(object sender, EventArgs e)
		{
			controlKeyChange(ControlKey.USE);
		}

		private void lb_keyUse_MouseEnter(object sender, EventArgs e)
		{
			lb_keyUse.Font = new Font("Calibri", 19.0f, FontStyle.Bold);
		}

		private void lb_keyUse_MouseLeave(object sender, EventArgs e)
		{
			lb_keyUse.Font = new Font("Calibri", 20.0f, FontStyle.Bold);
		}

		private void controlKeyChange(ControlKey _key)
		{
			if (timer.Enabled)
			{
				timer.Enabled = false;
				lb_keyChange.Text = "Press the key you want to use";
			}

			if (currKey != ControlKey.NON && currKey != _key)
			{
				setLabelColor(currKey, Color.DarkGray);
				currKey = _key;
				setLabelColor(_key, Color.Wheat);
				lb_keyChange.Visible = true;
			}
			else if (currKey == _key)
			{
				currKey = ControlKey.NON;
				setLabelColor(_key, Color.DarkGray);
				lb_keyChange.Visible = false;
			}
			else
			{
				currKey = _key;
				setLabelColor(_key, Color.Wheat);
				lb_keyChange.Visible = true;
			}
		}

		private void setLabelColor(ControlKey _key, Color _col)
		{
			switch (_key)
			{
				case ControlKey.NON:
					{
						break;
					}
				case ControlKey.FORWARD:
					{
						lb_keyFwd.ForeColor = _col;
						break;
					}
				case ControlKey.BACKWARD:
					{
						lb_keyback.ForeColor = _col;
						break;
					}
				case ControlKey.STRAFE_LEFT:
					{
						lb_keyLeft.ForeColor = _col;
						break;
					}
				case ControlKey.STRAFE_RIGHT:
					{
						lb_keyRight.ForeColor = _col;
						break;
					}
				case ControlKey.SPRINT:
					{
						lb_keySprint.ForeColor = _col;
						break;
					}
				case ControlKey.JUMP:
					{
						lb_keyJump.ForeColor = _col;
						break;
					}
				case ControlKey.USE:
					{
						lb_keyUse.ForeColor = _col;
						break;
					}
			}
		}

		private void Launcher_KeyDown(object sender, KeyEventArgs e)
		{
			if (currKey != ControlKey.NON)
			{
				string s = translateKeyCode(e);

				if(s == "")
				{
					lb_keyChange.Text = "INVALID KEY!";
					timer.Enabled = true;
					setLabelColor(currKey, Color.DarkGray);
					currKey = ControlKey.NON;
					return;
				}

				switch (currKey)
				{
					case ControlKey.FORWARD:
						{
							fwd.name = s;
							lb_keyFwd.Text = fwd.name;
							lb_keyFwd.ForeColor = Color.DarkGray;
							break;
						}
					case ControlKey.BACKWARD:
						{
							back.name = s;
							lb_keyback.Text = back.name;
							lb_keyback.ForeColor = Color.DarkGray;
							break;
						}
					case ControlKey.STRAFE_LEFT:
						{
							strafeL.name = s;
							lb_keyLeft.Text = strafeL.name;
							lb_keyLeft.ForeColor = Color.DarkGray;
							break;
						}
					case ControlKey.STRAFE_RIGHT:
						{
							strafeR.name = s;
							lb_keyRight.Text = strafeR.name;
							lb_keyRight.ForeColor = Color.DarkGray;
							break;
						}
					case ControlKey.SPRINT:
						{
							sprint.name = s;
							lb_keySprint.Text = sprint.name;
							lb_keySprint.ForeColor = Color.DarkGray;
							break;
						}
					case ControlKey.JUMP:
						{
							jump.name = s;
							lb_keyJump.Text = jump.name;
							lb_keyJump.ForeColor = Color.DarkGray;
							break;
						}
					case ControlKey.USE:
						{
							use.name = s;
							lb_keyUse.Text = use.name;
							lb_keyUse.ForeColor = Color.DarkGray;
							break;
						}
				}

				currKey = ControlKey.NON;
				lb_keyChange.Visible = false;
			}
		}

		private string translateKeyCode(KeyEventArgs _key)
		{
			if(_key.KeyValue >= 65 && _key.KeyValue <= 90)
			{
				return _key.KeyData.ToString();
			}
			else
			{
				string res = "";

				if (keysDict.TryGetValue(_key.KeyValue, out res))
				{
					return res.ToUpper();
				}
				else
				{
					return "";
				}
			}
		}

		private void timer_Tick(object sender, EventArgs e)
		{
			lb_keyChange.Visible = false;
			timer.Enabled = false;
			lb_keyChange.Text = "Press the key you want to use";
		}

		private string getInitValue(string _name, List<string> _iniFile)
		{
			foreach(string s in _iniFile)
			{
				if (s == "")
					continue;

				string unwanted = s.Substring(0, 1);

				if (unwanted == "#" || unwanted == "[")
					continue;

				List<string> tmpArr = s.Split('\t').ToList();

				for(int i = 0; i  < tmpArr.Count; i++)
				{
					if (tmpArr[i] == "" && i < tmpArr.Count)
					{
						tmpArr.RemoveAt(i);
						i--;
					}
				}

				if (tmpArr[0] != _name)
					continue;

				string tmpStr = tmpArr[1].Replace('=', ' ');
				return tmpStr.TrimStart(' ');
			}

			return "";
		}

		private Key getKey(string _name, List<string> _iniFile)
		{
			string tmpStr = "";
			int tmpInt = 0;

			tmpInt = Convert.ToInt32(getInitValue(_name, _iniFile));

			if (tmpInt >= 65 && tmpInt <= 90)
			{
				char c = (char)tmpInt;
				tmpStr = c.ToString();
			}
			else
			{
				keysDict.TryGetValue(tmpInt, out tmpStr);
			}

			Key k = new Key();
			k.name = tmpStr;
			k.value = tmpInt;

			return k;
		}
	}
}
