using System;
using System.Collections;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Windows.Forms;
using System.Diagnostics;
using Premise;

namespace Home
{
	/// <summary>
	/// Summary description for StatusPanel.
	/// </summary>
	public class StatusPanel : System.Windows.Forms.UserControl
	{
		private System.Windows.Forms.Label label;
		/// <summary> 
		/// Required designer variable.
		/// </summary>
		private System.ComponentModel.Container components = null;

		private IRemotePremiseObject m_Home = null;

		public StatusPanel()
		{
			// This call is required by the Windows.Forms Form Designer.
			InitializeComponent();

			// TODO: Add any initialization after the InitializeComponent call

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
					m_Home = null;
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
			this.label = new System.Windows.Forms.Label();
			this.SuspendLayout();
			// 
			// label
			// 
			this.label.Font = new System.Drawing.Font("Microstyle Bold Extended ATT", 14.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
			this.label.Location = new System.Drawing.Point(0, 0);
			this.label.Name = "label";
			this.label.Size = new System.Drawing.Size(432, 24);
			this.label.TabIndex = 1;
			this.label.Text = "Status";
			// 
			// StatusPanel
			// 
			this.Controls.Add(this.label);
			this.Name = "StatusPanel";
			this.Size = new System.Drawing.Size(672, 496);
			this.Load += new System.EventHandler(this.StatusPanel_Load);
			this.ResumeLayout(false);

		}
		#endregion

		public void premiseServer_Connected(object sender, PremiseServer.ConnectedEventArgs args)
		{
			m_Home = args.Root.GetObject("sys://Home");
		}

		private void StatusPanel_Load(object sender, System.EventArgs e)
		{
		
		}
	}
}
