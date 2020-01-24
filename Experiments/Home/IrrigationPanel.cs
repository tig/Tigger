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
	/// Summary description for IrrigationPanel.
	/// </summary>
	public class IrrigationPanel : System.Windows.Forms.UserControl
	{
		private System.Windows.Forms.Label label;
		/// <summary> 
		/// Required designer variable.
		/// </summary>
		private System.ComponentModel.Container components = null;
		private System.Windows.Forms.CheckBox checkSystemTest;
		private System.Windows.Forms.Label labelSystemStateLbl;
		private System.Windows.Forms.Button buttonSystemState;
		private System.Windows.Forms.Label labelSystemState;
		private System.Windows.Forms.CheckedListBox listZones;

		private IRemotePremiseObject m_Home=null;

		public IrrigationPanel()
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
					m_Home=null;
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
			this.labelSystemStateLbl = new System.Windows.Forms.Label();
			this.checkSystemTest = new System.Windows.Forms.CheckBox();
			this.buttonSystemState = new System.Windows.Forms.Button();
			this.labelSystemState = new System.Windows.Forms.Label();
			this.listZones = new System.Windows.Forms.CheckedListBox();
			this.SuspendLayout();
			// 
			// label
			// 
			this.label.Font = new System.Drawing.Font("Microstyle Bold Extended ATT", 14.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
			this.label.Location = new System.Drawing.Point(8, 8);
			this.label.Name = "label";
			this.label.Size = new System.Drawing.Size(432, 24);
			this.label.TabIndex = 1;
			this.label.Text = "Irrigation";
			this.label.Click += new System.EventHandler(this.label_Click);
			// 
			// labelSystemStateLbl
			// 
			this.labelSystemStateLbl.Font = new System.Drawing.Font("Verdana", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
			this.labelSystemStateLbl.Location = new System.Drawing.Point(32, 47);
			this.labelSystemStateLbl.Name = "labelSystemStateLbl";
			this.labelSystemStateLbl.Size = new System.Drawing.Size(104, 23);
			this.labelSystemStateLbl.TabIndex = 3;
			this.labelSystemStateLbl.Text = "System State:";
			this.labelSystemStateLbl.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
			// 
			// checkSystemTest
			// 
			this.checkSystemTest.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
			this.checkSystemTest.Font = new System.Drawing.Font("Verdana", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
			this.checkSystemTest.Location = new System.Drawing.Point(376, 48);
			this.checkSystemTest.Name = "checkSystemTest";
			this.checkSystemTest.Size = new System.Drawing.Size(128, 24);
			this.checkSystemTest.TabIndex = 2;
			this.checkSystemTest.Text = "Test Mode";
			this.checkSystemTest.CheckedChanged += new System.EventHandler(this.checkSystemTest_CheckedChanged);
			// 
			// buttonSystemState
			// 
			this.buttonSystemState.Font = new System.Drawing.Font("Verdana", 9.75F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
			this.buttonSystemState.Location = new System.Drawing.Point(32, 72);
			this.buttonSystemState.Name = "buttonSystemState";
			this.buttonSystemState.Size = new System.Drawing.Size(80, 24);
			this.buttonSystemState.TabIndex = 4;
			this.buttonSystemState.Text = "Start";
			this.buttonSystemState.Click += new System.EventHandler(this.buttonSystemState_Click);
			// 
			// labelSystemState
			// 
			this.labelSystemState.Font = new System.Drawing.Font("Verdana", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
			this.labelSystemState.Location = new System.Drawing.Point(136, 48);
			this.labelSystemState.Name = "labelSystemState";
			this.labelSystemState.TabIndex = 5;
			this.labelSystemState.Text = "state";
			this.labelSystemState.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
			// 
			// listZones
			// 
			this.listZones.BorderStyle = System.Windows.Forms.BorderStyle.None;
			this.listZones.ColumnWidth = 300;
			this.listZones.Font = new System.Drawing.Font("Verdana", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
			this.listZones.Location = new System.Drawing.Point(32, 112);
			this.listZones.MultiColumn = true;
			this.listZones.Name = "listZones";
			this.listZones.Size = new System.Drawing.Size(224, 270);
			this.listZones.TabIndex = 6;
			this.listZones.UseTabStops = false;
			this.listZones.SelectedIndexChanged += new System.EventHandler(this.listZones_SelectedIndexChanged);
			this.listZones.ItemCheck += new System.Windows.Forms.ItemCheckEventHandler(this.listZones_ItemCheck);
			// 
			// IrrigationPanel
			// 
			this.Controls.Add(this.listZones);
			this.Controls.Add(this.labelSystemState);
			this.Controls.Add(this.buttonSystemState);
			this.Controls.Add(this.labelSystemStateLbl);
			this.Controls.Add(this.label);
			this.Controls.Add(this.checkSystemTest);
			this.Name = "IrrigationPanel";
			this.Size = new System.Drawing.Size(848, 640);
			this.Load += new System.EventHandler(this.IrrigationPanel_Load);
			this.ResumeLayout(false);

		}
		#endregion

		public void premiseServer_Connected(object sender, PremiseServer.ConnectedEventArgs args)
		{
			m_Home = args.Root.GetObject("sys://Home");

			IRemotePremiseObject ir = m_Home.GetObject("sys://Home/Yard/Irrigation");
			IRemotePremiseObjectCollection props = ir.GetProperties(false);
			for(int i=0; i < props.Count; i++)
			{
				IRemotePremiseObject p = props.Item(i);
				if (p.GetValue("Name").ToString().Equals("SystemState"))
				{
					if ((bool)ir.GetValue("SystemState"))
					{
						labelSystemState.Text = p.GetValue("CaptionTrue").ToString();
						buttonSystemState.Text = "Stop";
					}
					else
					{
						labelSystemState.Text = p.GetValue("CaptionFalse").ToString();
						buttonSystemState.Text = "Start";
					}
					break;
				}
			}
			ir.SubscribeToProperty("SystemState", "OnSystemStateChanged", this);

			checkSystemTest.Checked = (bool)ir.GetValue("SystemTest");
			ir.SubscribeToProperty("SystemTest", "OnSystemTestChanged", this);

			// Zones
			IRemotePremiseObjectCollection zones = ir.GetChildren(false);
			for (int i=0; i < zones.Count; i++)
			{
				IRemotePremiseObject zone = zones.Item(i);
				listZones.SetItemCheckState(listZones.Items.Add(zone.GetValue("Name").ToString()), 
					((bool)zone.GetValue("ZoneState")) ? CheckState.Checked : CheckState.Unchecked);

				ir.SubscribeToProperty("ZoneState", "OnZoneStateChanged", this);
			}

		}

		public virtual void OnSystemStateChanged(int subID, IRemotePremiseObject objectChanged, IRemotePremiseObject property,  object newValue)
		{
			//buttonSystemState.Checked = (bool)newValue;
			if ((bool)newValue)
			{
				labelSystemState.Text = property.GetValue("CaptionTrue").ToString();
				buttonSystemState.Text = "Stop";
			}
			else
			{
				labelSystemState.Text = property.GetValue("CaptionFalse").ToString();
				buttonSystemState.Text = "Start";
			}
		}

		public virtual void OnZoneStateChanged(int subID, IRemotePremiseObject objectChanged, IRemotePremiseObject property,  object newValue)
		{
			listZones.SetItemCheckState(listZones.FindString(objectChanged.GetValue("Name").ToString()), ((bool)newValue) ? CheckState.Checked : CheckState.Unchecked);
		}

		private void label_Click(object sender, System.EventArgs e)
		{
		
		}

		private void IrrigationPanel_Load(object sender, System.EventArgs e)
		{
		
		}


		public virtual void OnSystemTestChanged(int subID, IRemotePremiseObject objectChanged, IRemotePremiseObject property,  object newValue)
		{
			checkSystemTest.Checked = (bool)newValue;
		}

		private void checkSystemTest_CheckedChanged(object sender, System.EventArgs e)
		{
			IRemotePremiseObject ir = m_Home.GetObject("sys://Home/Yard/Irrigation");
			ir.SetValue("SystemTest", checkSystemTest.Checked);
		}

		private void buttonSystemState_Click(object sender, System.EventArgs e)
		{
			IRemotePremiseObject ir = m_Home.GetObject("sys://Home/Yard/Irrigation");
			ir.SetValue("SystemState", !(bool)ir.GetValue("SystemState"));
		}

		private void listZones_SelectedIndexChanged(object sender, System.EventArgs e)
		{
		
		}

		private void listZones_ItemCheck(object sender, System.Windows.Forms.ItemCheckEventArgs e)
		{
			String t = listZones.GetItemText(listZones.Items[e.Index]);
			IRemotePremiseObject ir = m_Home.GetObject("sys://Home/Yard/Irrigation");
			IRemotePremiseObjectCollection zones = ir.GetObjectsByPropertyValue("Name", t, false);
			if (e.NewValue == CheckState.Checked)
				zones.Item(0).SetValue("ZoneState", true);
			else
				zones.Item(0).SetValue("ZoneState", false);
		}
	}
}
