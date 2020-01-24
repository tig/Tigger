using System;
using System.Drawing;
using System.Collections;
using System.ComponentModel;
using System.Windows.Forms;
using System.Data;
using System.Diagnostics;
using System.Net;
using System.Threading;
using System.Timers;
using Microsoft.Win32;
using System.IO;
using Premise;


namespace MainWindow
{
	/// <summary>
	/// Summary description for Form1.
	/// </summary>
	public class MainWindow : System.Windows.Forms.Form
	{
		private System.Windows.Forms.Button buttonStatus;
		private System.Windows.Forms.Button buttonMedia;
		private System.Windows.Forms.Button buttonSecurity;
		private System.Windows.Forms.Button buttonSources;
		private System.Windows.Forms.Button buttonIrrigation;

		private System.Windows.Forms.Button buttonClose;
		private PremiseServer.PremiseServer premiseServer;
		private System.Windows.Forms.Label labelHomeName;
		private System.Windows.Forms.Label labelDateTime;
		private System.Windows.Forms.Timer timer;
		private System.ComponentModel.IContainer components;
		private System.Windows.Forms.Panel panelTopLine;

		private Home.StatusPanel panelStatus;
		private Home.MediaZonesPanel panelMedia;
		private Home.IrrigationPanel panelIrrigation;

		private enum Panels {Status, Media, Sources, Security, Irrigation};
		private Panels m_CurrentPanel;

		IRemotePremiseObject Home = null;

		public MainWindow()
		{
			//
			// Required for Windows Form Designer support
			//
			InitializeComponent();

			//
			// TODO: Add any constructor code after InitializeComponent call
			//

			// Wire up event handlers for each panel
			m_CurrentPanel = Panels.Status;
			UpdateUI();

			this.premiseServer.Connected += new PremiseServer.ConnectedEventHandler(panelStatus.premiseServer_Connected);
			this.premiseServer.Connected += new PremiseServer.ConnectedEventHandler(panelMedia.premiseServer_Connected);
			this.premiseServer.Connected += new PremiseServer.ConnectedEventHandler(panelIrrigation.premiseServer_Connected);
			premiseServer.Connect();

			labelDateTime.Text = DateTime.Now.ToLongDateString() + "\n" + DateTime.Now.ToShortTimeString();

		}

		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		protected override void Dispose( bool disposing )
		{
			if( disposing )
			{
				if (components != null) 
				{
					components.Dispose();
					Home = null;
				}
		}
			base.Dispose( disposing );
		}

		#region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		private void InitializeComponent()
		{
			this.components = new System.ComponentModel.Container();
			this.buttonMedia = new System.Windows.Forms.Button();
			this.buttonSecurity = new System.Windows.Forms.Button();
			this.buttonStatus = new System.Windows.Forms.Button();
			this.buttonClose = new System.Windows.Forms.Button();
			this.panelMedia = new Home.MediaZonesPanel();
			this.premiseServer = new PremiseServer.PremiseServer();
			this.labelHomeName = new System.Windows.Forms.Label();
			this.labelDateTime = new System.Windows.Forms.Label();
			this.timer = new System.Windows.Forms.Timer(this.components);
			this.buttonSources = new System.Windows.Forms.Button();
			this.panelTopLine = new System.Windows.Forms.Panel();
			this.buttonIrrigation = new System.Windows.Forms.Button();
			this.panelStatus = new Home.StatusPanel();
			this.panelIrrigation = new Home.IrrigationPanel();
			this.SuspendLayout();
			// 
			// buttonMedia
			// 
			this.buttonMedia.Font = new System.Drawing.Font("Verdana", 9.75F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
			this.buttonMedia.Location = new System.Drawing.Point(8, 120);
			this.buttonMedia.Name = "buttonMedia";
			this.buttonMedia.Size = new System.Drawing.Size(96, 64);
			this.buttonMedia.TabIndex = 2;
			this.buttonMedia.Text = "All House Audio";
			this.buttonMedia.Click += new System.EventHandler(this.buttonMedia_Click);
			// 
			// buttonSecurity
			// 
			this.buttonSecurity.Font = new System.Drawing.Font("Verdana", 9.75F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
			this.buttonSecurity.Location = new System.Drawing.Point(8, 264);
			this.buttonSecurity.Name = "buttonSecurity";
			this.buttonSecurity.Size = new System.Drawing.Size(96, 64);
			this.buttonSecurity.TabIndex = 4;
			this.buttonSecurity.Text = "Security";
			this.buttonSecurity.Click += new System.EventHandler(this.buttonSecurity_Click);
			// 
			// buttonStatus
			// 
			this.buttonStatus.Font = new System.Drawing.Font("Verdana", 9.75F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
			this.buttonStatus.Location = new System.Drawing.Point(8, 48);
			this.buttonStatus.Name = "buttonStatus";
			this.buttonStatus.Size = new System.Drawing.Size(96, 64);
			this.buttonStatus.TabIndex = 1;
			this.buttonStatus.Text = "Status";
			this.buttonStatus.Click += new System.EventHandler(this.buttonStatus_Click);
			// 
			// buttonClose
			// 
			this.buttonClose.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
			this.buttonClose.Location = new System.Drawing.Point(992, 0);
			this.buttonClose.Name = "buttonClose";
			this.buttonClose.Size = new System.Drawing.Size(24, 24);
			this.buttonClose.TabIndex = 3;
			this.buttonClose.Text = "X";
			this.buttonClose.Click += new System.EventHandler(this.buttonClose_Click);
			// 
			// panelMedia
			// 
			this.panelMedia.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
				| System.Windows.Forms.AnchorStyles.Left) 
				| System.Windows.Forms.AnchorStyles.Right)));
			this.panelMedia.Location = new System.Drawing.Point(120, 32);
			this.panelMedia.Name = "panelMedia";
			this.panelMedia.Size = new System.Drawing.Size(896, 704);
			this.panelMedia.TabIndex = 5;
			// 
			// premiseServer
			// 
			this.premiseServer.Dock = System.Windows.Forms.DockStyle.Bottom;
			this.premiseServer.Font = new System.Drawing.Font("Verdana", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
			this.premiseServer.Location = new System.Drawing.Point(0, 710);
			this.premiseServer.Name = "premiseServer";
			this.premiseServer.Size = new System.Drawing.Size(1016, 24);
			this.premiseServer.TabIndex = 0;
			this.premiseServer.Load += new System.EventHandler(this.premiseServer_Load);
			this.premiseServer.Connected += new PremiseServer.ConnectedEventHandler(this.premiseServer_Connected);
			// 
			// labelHomeName
			// 
			this.labelHomeName.Font = new System.Drawing.Font("Microstyle Bold Extended ATT", 18F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
			this.labelHomeName.Location = new System.Drawing.Point(8, 0);
			this.labelHomeName.Name = "labelHomeName";
			this.labelHomeName.Size = new System.Drawing.Size(456, 32);
			this.labelHomeName.TabIndex = 0;
			this.labelHomeName.Text = "home name";
			// 
			// labelDateTime
			// 
			this.labelDateTime.Font = new System.Drawing.Font("Verdana", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
			this.labelDateTime.Location = new System.Drawing.Point(752, 0);
			this.labelDateTime.Name = "labelDateTime";
			this.labelDateTime.Size = new System.Drawing.Size(224, 32);
			this.labelDateTime.TabIndex = 0;
			this.labelDateTime.Text = "date, time";
			this.labelDateTime.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
			// 
			// timer
			// 
			this.timer.Enabled = true;
			this.timer.Interval = 30000;
			this.timer.Tick += new System.EventHandler(this.timer_Tick);
			// 
			// buttonSources
			// 
			this.buttonSources.Font = new System.Drawing.Font("Verdana", 9.75F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
			this.buttonSources.Location = new System.Drawing.Point(8, 192);
			this.buttonSources.Name = "buttonSources";
			this.buttonSources.Size = new System.Drawing.Size(96, 64);
			this.buttonSources.TabIndex = 3;
			this.buttonSources.Text = "Media Sources";
			this.buttonSources.Click += new System.EventHandler(this.buttonSources_Click);
			// 
			// panelTopLine
			// 
			this.panelTopLine.BackColor = System.Drawing.Color.White;
			this.panelTopLine.Location = new System.Drawing.Point(0, 32);
			this.panelTopLine.Name = "panelTopLine";
			this.panelTopLine.Size = new System.Drawing.Size(1024, 4);
			this.panelTopLine.TabIndex = 9;
			// 
			// buttonIrrigation
			// 
			this.buttonIrrigation.Font = new System.Drawing.Font("Verdana", 9.75F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
			this.buttonIrrigation.Location = new System.Drawing.Point(8, 336);
			this.buttonIrrigation.Name = "buttonIrrigation";
			this.buttonIrrigation.Size = new System.Drawing.Size(96, 64);
			this.buttonIrrigation.TabIndex = 4;
			this.buttonIrrigation.Text = "Irrigation";
			this.buttonIrrigation.Click += new System.EventHandler(this.buttonIrrigation_Click);
			// 
			// panelStatus
			// 
			this.panelStatus.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
				| System.Windows.Forms.AnchorStyles.Left) 
				| System.Windows.Forms.AnchorStyles.Right)));
			this.panelStatus.Location = new System.Drawing.Point(120, 40);
			this.panelStatus.Name = "panelStatus";
			this.panelStatus.Size = new System.Drawing.Size(672, 496);
			this.panelStatus.TabIndex = 10;
			// 
			// panelIrrigation
			// 
			this.panelIrrigation.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
				| System.Windows.Forms.AnchorStyles.Left) 
				| System.Windows.Forms.AnchorStyles.Right)));
			this.panelIrrigation.Location = new System.Drawing.Point(120, 40);
			this.panelIrrigation.Name = "panelIrrigation";
			this.panelIrrigation.Size = new System.Drawing.Size(848, 640);
			this.panelIrrigation.TabIndex = 0;
			// 
			// MainWindow
			// 
			this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
			this.BackColor = System.Drawing.Color.DarkSlateBlue;
			this.ClientSize = new System.Drawing.Size(1016, 734);
			this.Controls.Add(this.panelIrrigation);
			this.Controls.Add(this.panelStatus);
			this.Controls.Add(this.panelTopLine);
			this.Controls.Add(this.labelDateTime);
			this.Controls.Add(this.labelHomeName);
			this.Controls.Add(this.panelMedia);
			this.Controls.Add(this.buttonClose);
			this.Controls.Add(this.buttonMedia);
			this.Controls.Add(this.buttonSecurity);
			this.Controls.Add(this.buttonStatus);
			this.Controls.Add(this.premiseServer);
			this.Controls.Add(this.buttonSources);
			this.Controls.Add(this.buttonIrrigation);
			this.ForeColor = System.Drawing.Color.White;
			this.Name = "MainWindow";
			this.SizeGripStyle = System.Windows.Forms.SizeGripStyle.Hide;
			this.Text = "Kindel MainWindow";
			this.SizeChanged += new System.EventHandler(this.MainWindow_SizeChanged);
			this.Load += new System.EventHandler(this.MainWindow_Load);
			this.ResumeLayout(false);

		}
		#endregion

		/// <summary>
		/// The main entry point for the application.
		/// </summary>
		[STAThread]
		static void Main() 
		{
			Application.Run(new MainWindow());
		}

		private void UpdateUI()
		{
			panelStatus.Hide();
			buttonStatus.BackColor = this.BackColor;

			panelMedia.Hide();
			buttonMedia.BackColor = this.BackColor;
			
			//panelSources.Hide();
			buttonSources.BackColor = this.BackColor;
			
			//panelSecurity.Hide();
			buttonSecurity.BackColor = this.BackColor;
			
			panelIrrigation.Hide();
			buttonIrrigation.BackColor = this.BackColor;

			switch (m_CurrentPanel)
			{
				case Panels.Status:
					panelStatus.Show();
					buttonStatus.BackColor = Color.Blue;
					break;

				case Panels.Media:
					panelMedia.Show();
					buttonMedia.BackColor = Color.Blue;
					break;

				case Panels.Sources:
					//panelSources.Show();
					buttonSources.BackColor = Color.Blue;
					break;

				case Panels.Security:
					//panelSecurity.Show();
					buttonSecurity.BackColor = Color.Blue;
					break;

				case Panels.Irrigation:
					panelIrrigation.Show();
					buttonIrrigation.BackColor = Color.Blue;
					break;
			}

		}

		private void buttonStatus_Click(object sender, System.EventArgs e)
		{
			m_CurrentPanel = Panels.Status;
			UpdateUI();

		}

		private void buttonMedia_Click(object sender, System.EventArgs e)
		{
			m_CurrentPanel = Panels.Media;
			UpdateUI();

		}

		private void buttonSources_Click(object sender, System.EventArgs e)
		{
			m_CurrentPanel = Panels.Sources;
			UpdateUI();
		
		}

		private void buttonSecurity_Click(object sender, System.EventArgs e)
		{
			m_CurrentPanel = Panels.Security;

			UpdateUI();
		
		}

		private void buttonIrrigation_Click(object sender, System.EventArgs e)
		{
			m_CurrentPanel = Panels.Irrigation;

			UpdateUI();
		
		}

		private void buttonClose_Click(object sender, System.EventArgs e)
		{
			Close();
		
		}


		private void MainWindow_SizeChanged(object sender, System.EventArgs e)
		{
			if (this.WindowState == FormWindowState.Maximized)
				this.FormBorderStyle = FormBorderStyle.None;
			else
				this.FormBorderStyle = FormBorderStyle.Sizable;
		}

		private void MainWindow_Load(object sender, System.EventArgs e)
		{
		
		}

		private void premiseServer_Load(object sender, System.EventArgs e)
		{
		
		}

		public void premiseServer_Connected(object sender, PremiseServer.ConnectedEventArgs args)
		{
			IRemotePremiseObject Root = args.Root; 

			if (Root != null) 
			{
				try 
				{
					Home = Root.GetObject("sys://Home");
				}
				catch (Exception e)
				{
					MessageBox.Show(e.Message + " GetObject failed on sys://Home.","Problem" , MessageBoxButtons.OK, MessageBoxIcon.Asterisk);
				}
				if (Home != null) 
				{
					try 
					{
						labelHomeName.Text = Home.GetValue("DisplayName").ToString();
						try 
						{
							Home.SubscribeToProperty("DisplayName", "OnDisplayNameChanged", this);
							//AdjustUI();
							//TestCode();
						}
						catch (Exception e)
						{
							MessageBox.Show(e.Message + " SubscribeToProperty failed.","Problem", MessageBoxButtons.OK, MessageBoxIcon.Asterisk);
						}
					}
					catch (Exception e)
					{
						MessageBox.Show(e.Message + " GetProperty failed. ","Problem" , MessageBoxButtons.OK, MessageBoxIcon.Asterisk);
					}
				}
			}
		}

		/// <summary>
		/// OnDisplayNameChanged is a typical minibroker subscription callback function
		/// This allows you to "monitor" a specific property for changes in the Premise schema
		/// This specific function is called when the DisplayName property is changed on the
		/// Home object. See the Connect() function for how to subscribe.
		/// </summary>
		/// <param name="subID"></param>			the internal subscription ID
		/// <param name="objectChanged"></param>	the premise object that changed
		/// <param name="property"></param>			the property that changed
		/// <param name="newValue"></param>			the new value of the property
		public virtual void OnDisplayNameChanged(int subID, IRemotePremiseObject objectChanged, IRemotePremiseObject property,  object newValue)
		{
			if (objectChanged.Equals(Home))
				labelHomeName.Text = newValue.ToString();
		}

		private void timer_Tick(object sender, System.EventArgs e)
		{
			labelDateTime.Text = DateTime.Now.ToLongDateString() + "\n" + DateTime.Now.ToShortTimeString();
		}


	}
}

