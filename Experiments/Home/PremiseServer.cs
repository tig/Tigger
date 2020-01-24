using System;
using System.Collections;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Windows.Forms;
using System.Diagnostics;
using System.Net;
using System.Threading;
using System.Timers;
using Microsoft.Win32;
using System.IO;
using Premise;

namespace PremiseServer
{
	public delegate void ConnectedEventHandler(object sender, ConnectedEventArgs e);


	/// <summary>
	/// Summary description for PremiseServer.
	/// </summary>
	public class PremiseServer : System.Windows.Forms.UserControl
	{
		public event ConnectedEventHandler Connected;

		private System.Windows.Forms.Label labelStatus;
		/// <summary> 
		/// Required designer variable.
		/// </summary>
		private System.ComponentModel.Container components = null;

		private ISYSMiniBroker m_sysBroker = null;				// minibroker instance
//		private System.Timers.Timer m_reconnectTimer = null;	// connect timer
		private string m_stServerHostName = "kindelsys.kindel.com";		// login defaults
		private string m_stUserName = "admin";
		private string m_stUserPassword = "Premise!";
		public IRemotePremiseObject RemotePremiseObject = null;			// server-side RemotePremiseObject object

		public PremiseServer()
		{
			// This call is required by the Windows.Forms Form Designer.
			InitializeComponent();

			// TODO: Add any initialization after the InitializeComponent call
			labelStatus.Top = 0;
			labelStatus.Left = 0;
			labelStatus.Size = this.Size;
			labelStatus.Text = "Disconnected";
		}

		/// <summary> 
		/// Clean up any resources being used.
		/// </summary>
		protected override void Dispose( bool disposing )
		{
			if( disposing )
			{
				if(components != null)
				{
					components.Dispose();
					RemotePremiseObject = null;
					m_sysBroker = null;
				}
			}
			base.Dispose( disposing );
		}

		#region Component Designer generated code
		/// <summary> 
		/// Required method for Designer support - do not modify 
		/// the contents of this method with the code editor.
		/// </summary>
		private void InitializeComponent()
		{
			this.labelStatus = new System.Windows.Forms.Label();
			this.SuspendLayout();
			// 
			// labelStatus
			// 
			this.labelStatus.Location = new System.Drawing.Point(0, 0);
			this.labelStatus.Name = "labelStatus";
			this.labelStatus.Size = new System.Drawing.Size(24, 23);
			this.labelStatus.TabIndex = 0;
			this.labelStatus.Text = "status";
			this.labelStatus.Click += new System.EventHandler(this.labelStatus_DoubleClick);
			// 
			// PremiseServer
			// 
			this.Controls.Add(this.labelStatus);
			this.Name = "PremiseServer";
			this.Size = new System.Drawing.Size(200, 32);
			this.Load += new System.EventHandler(this.PremiseServer_Load);
			this.ResumeLayout(false);

		}
		#endregion

		private void PremiseServer_SizeChanged(object sender, System.EventArgs e)
		{
			labelStatus.Top = 0;
			labelStatus.Left = 0;
			labelStatus.Size = this.Size;
		}

		private void PremiseServer_Load(object sender, System.EventArgs e)
		{
		
		}

		public void Connect()
		{
			// get an instance of the minibroker remote client
			m_sysBroker = new SYSMiniBrokerClass();

			// subscribe to connection change notifications
			m_sysBroker.SubscribeToConnection("OnConnectionStateChanged", this);

			// subscribe to discovery change notifications
			m_sysBroker.SubscribeToDiscovery("OnServerDiscoveryProc", this);

			// create a reconnect timer
//			m_reconnectTimer = new System.Timers.Timer();
//			m_reconnectTimer.Elapsed+=new ElapsedEventHandler(ReconnectTimerProc);

			// Set the interval to 60 seconds.
//			m_reconnectTimer.Interval=20000;
//			m_reconnectTimer.Enabled = false;
			
			// connect to the remote server
			//ConnectToPremise();
		}

		/// <summary>
		/// SYS server discovery callback procedure
		/// </summary>
		public virtual void OnServerDiscoveryProc(string location, string serverInfo, string IPAddress, long status)
		{
			Debug.WriteLine("ServerDiscoveryProc: " + location + " status = " + status);
			if (location.ToLower().Equals(m_stServerHostName.ToLower())) // check if this is the server we are interested in
			{
				if (status != 0) // server service shutdown
				{
					//m_bnormShutdown = true;  // signal this is a true shutdown
//					m_reconnectTimer.Enabled = true;					
					labelStatus.Text = "Disconnected";
					RemotePremiseObject = null;
				}
				else // server service has started
				{
					ConnectToPremise();
				}
			}
		}

		/// <summary>
		/// SYS Server connection state callback
		/// </summary>
		/// <param name="status"></param>
		/// <param name="msg"></param>
		public virtual void OnConnectionStateChanged(long status, String msg)
		{
			Debug.WriteLine("OnConnectionStateChanged " + msg);
			switch (status)
			{
				case 0: // miniBroker Disconnected
//					m_reconnectTimer.Enabled  = true;
					RemotePremiseObject = null;
					labelStatus.Text = "Disconnected";
					break;
				case 2: // miniBroker Failed To Connect
//					m_reconnectTimer.Enabled  = true;
					RemotePremiseObject = null;
					labelStatus.Text = "Failed To Connect";
					break;
				case 4: // miniBrokerConnectionTerminated
//					m_reconnectTimer.Enabled  = true;
					RemotePremiseObject = null;
					labelStatus.Text = "Terminated";
					break;

				case 1: // miniBroker Connecting
//					m_reconnectTimer.Enabled = false;
					labelStatus.Text = "Connecting";
					break;
				case 3: // miniBroker Connected
					labelStatus.Text = "Connected";
//					m_reconnectTimer.Enabled = false;
					break;
				default:
					break;
			}
			Debug.WriteLine(labelStatus.Text);
		}


//		public virtual void ReconnectTimerProc(object source, ElapsedEventArgs e)
//		{
//			Debug.WriteLine("ReconnectTimerProc");
//			RemotePremiseObject = null;
//			ConnectToPremise();
//		}
		
		/// <summary>
		/// Connects and establishes a session with the SYS server and initializes the media m_player.
		/// </summary>
		private void ConnectToPremise()
		{

			if ((m_stServerHostName == "") || (m_sysBroker == null))
				return;

			// Connect to the Server and get the RemotePremiseObject object
			RemotePremiseObject = m_sysBroker.Connect(m_stServerHostName,m_stUserName,m_stUserPassword);
			if (RemotePremiseObject == null) 
			{
				Debug.WriteLine("Unable to connect to Premise Server: " + m_stServerHostName);
				labelStatus.Text = "Disconnected";
//				m_reconnectTimer.Enabled = true;
				return;
			}
			// stop the reconnect timer
//			m_reconnectTimer.Enabled = false;
			
			Debug.WriteLine("Connected to Premise Server: " + m_stServerHostName);
			OnConnected(new ConnectedEventArgs(RemotePremiseObject));
			return;
		}

		// Invoke the Connected event; called whenever list changes
		protected virtual void OnConnected(ConnectedEventArgs e) 
		{
			if (Connected != null)
				Connected(this, e);
		}

		private void labelStatus_DoubleClick(object sender, System.EventArgs e)
		{
			if (m_sysBroker == null)
				Connect();
		}

	}
	public class ConnectedEventArgs : EventArgs
	{
		public IRemotePremiseObject Root = null;

		public ConnectedEventArgs(IRemotePremiseObject sysObject)
		{
			Root = sysObject;
		}
	}

}
