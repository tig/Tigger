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
	/// Summary description for Media.
	/// </summary>
	public class MediaZonesPanel : System.Windows.Forms.UserControl
	{
		private System.Windows.Forms.Label label;
		private Home.MediaZone mzRecRoom;
		private Home.MediaZone mzKitchen;
		private Home.MediaZone mzDining;
		private Home.MediaZone mzMaster;
		private Home.MediaZone mzPorch;
		private Home.MediaZone mzDeck;
		private Home.MediaZone mzTerrace;
		private Home.MediaZone mzOffice;
		private Home.MediaZone mzExcercise;
		private Home.MediaZone mzSwingRoom;
		private Home.MediaZone mzStudy;
		/// <summary> 
		/// Required designer variable.
		/// </summary>
		private System.ComponentModel.Container components = null;

		public MediaZonesPanel()
		{
			// This call is required by the Windows.Forms Form Designer.
			InitializeComponent();

			// TODO: Add any initialization after the InitializeComponent call
			mzOffice.Text = "Office";
			mzRecRoom.Text = "Rec Room";
			mzKitchen.Text = "Kitchen";
			mzDining.Text = "Dining Room";
			mzMaster.Text = "Master Bedroom";
			mzPorch.Text = "Porch";
			mzDeck.Text = "Deck";
			mzTerrace.Text = "Terrace";
			mzExcercise.Text = "Exercise Room";
			mzSwingRoom.Text = "Swing Room";
			mzSwingRoom.Enabled = false;
			mzStudy.Text = "Julie's Study";
			mzStudy.Enabled = false;

			// Wire up event handlers for each zone
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
			this.mzOffice = new Home.MediaZone();
			this.mzRecRoom = new Home.MediaZone();
			this.mzKitchen = new Home.MediaZone();
			this.mzDining = new Home.MediaZone();
			this.mzMaster = new Home.MediaZone();
			this.mzPorch = new Home.MediaZone();
			this.mzDeck = new Home.MediaZone();
			this.mzTerrace = new Home.MediaZone();
			this.mzExcercise = new Home.MediaZone();
			this.mzSwingRoom = new Home.MediaZone();
			this.mzStudy = new Home.MediaZone();
			this.SuspendLayout();
			// 
			// label
			// 
			this.label.BackColor = System.Drawing.Color.Transparent;
			this.label.Font = new System.Drawing.Font("Microstyle Bold Extended ATT", 14.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
			this.label.Location = new System.Drawing.Point(16, 8);
			this.label.Name = "label";
			this.label.Size = new System.Drawing.Size(432, 24);
			this.label.TabIndex = 0;
			this.label.Text = "All House Audio";
			// 
			// mzOffice
			// 
			this.mzOffice.Location = new System.Drawing.Point(16, 32);
			this.mzOffice.Name = "mzOffice";
			this.mzOffice.Size = new System.Drawing.Size(288, 96);
			this.mzOffice.TabIndex = 1;
			// 
			// mzRecRoom
			// 
			this.mzRecRoom.Location = new System.Drawing.Point(16, 144);
			this.mzRecRoom.Name = "mzRecRoom";
			this.mzRecRoom.Size = new System.Drawing.Size(288, 96);
			this.mzRecRoom.TabIndex = 2;
			// 
			// mzKitchen
			// 
			this.mzKitchen.Location = new System.Drawing.Point(352, 32);
			this.mzKitchen.Name = "mzKitchen";
			this.mzKitchen.Size = new System.Drawing.Size(288, 96);
			this.mzKitchen.TabIndex = 2;
			// 
			// mzDining
			// 
			this.mzDining.Location = new System.Drawing.Point(16, 256);
			this.mzDining.Name = "mzDining";
			this.mzDining.Size = new System.Drawing.Size(288, 96);
			this.mzDining.TabIndex = 2;
			this.mzDining.Load += new System.EventHandler(this.mzDining_Load);
			// 
			// mzMaster
			// 
			this.mzMaster.Location = new System.Drawing.Point(16, 368);
			this.mzMaster.Name = "mzMaster";
			this.mzMaster.Size = new System.Drawing.Size(288, 96);
			this.mzMaster.TabIndex = 2;
			// 
			// mzPorch
			// 
			this.mzPorch.Location = new System.Drawing.Point(352, 144);
			this.mzPorch.Name = "mzPorch";
			this.mzPorch.Size = new System.Drawing.Size(288, 96);
			this.mzPorch.TabIndex = 2;
			// 
			// mzDeck
			// 
			this.mzDeck.Location = new System.Drawing.Point(352, 256);
			this.mzDeck.Name = "mzDeck";
			this.mzDeck.Size = new System.Drawing.Size(288, 96);
			this.mzDeck.TabIndex = 2;
			// 
			// mzTerrace
			// 
			this.mzTerrace.Location = new System.Drawing.Point(352, 368);
			this.mzTerrace.Name = "mzTerrace";
			this.mzTerrace.Size = new System.Drawing.Size(288, 96);
			this.mzTerrace.TabIndex = 2;
			// 
			// mzExcercise
			// 
			this.mzExcercise.Location = new System.Drawing.Point(352, 480);
			this.mzExcercise.Name = "mzExcercise";
			this.mzExcercise.Size = new System.Drawing.Size(288, 96);
			this.mzExcercise.TabIndex = 2;
			// 
			// mzSwingRoom
			// 
			this.mzSwingRoom.Location = new System.Drawing.Point(16, 480);
			this.mzSwingRoom.Name = "mzSwingRoom";
			this.mzSwingRoom.Size = new System.Drawing.Size(288, 96);
			this.mzSwingRoom.TabIndex = 2;
			// 
			// mzStudy
			// 
			this.mzStudy.Location = new System.Drawing.Point(16, 592);
			this.mzStudy.Name = "mzStudy";
			this.mzStudy.Size = new System.Drawing.Size(288, 96);
			this.mzStudy.TabIndex = 2;
			// 
			// MediaZonesPanel
			// 
			this.Controls.Add(this.mzRecRoom);
			this.Controls.Add(this.mzOffice);
			this.Controls.Add(this.label);
			this.Controls.Add(this.mzKitchen);
			this.Controls.Add(this.mzDining);
			this.Controls.Add(this.mzMaster);
			this.Controls.Add(this.mzPorch);
			this.Controls.Add(this.mzDeck);
			this.Controls.Add(this.mzTerrace);
			this.Controls.Add(this.mzExcercise);
			this.Controls.Add(this.mzSwingRoom);
			this.Controls.Add(this.mzStudy);
			this.Name = "MediaZonesPanel";
			this.Size = new System.Drawing.Size(1024, 768);
			this.Load += new System.EventHandler(this.MediaZonesPanel_Load);
			this.ResumeLayout(false);

		}
		#endregion

		private void MediaZonesPanel_Load(object sender, System.EventArgs e)
		{
		
		}

		public void premiseServer_Connected(object sender, PremiseServer.ConnectedEventArgs args)
		{
			mzOffice.PremiseObject = args.Root.GetObject("sys://Home/Office");
			mzRecRoom.PremiseObject = args.Root.GetObject("sys://Home/Downstairs/Rec Room/MediaZone");
		}

		private void mzDining_Load(object sender, System.EventArgs e)
		{
		
		}

	}
}
