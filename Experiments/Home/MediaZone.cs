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
	/// Summary description for MediaZone.
	/// </summary>
	public class MediaZone : System.Windows.Forms.UserControl
	{
		private System.Windows.Forms.ComboBox cbSource;
		private System.Windows.Forms.CheckBox checkMute;
		private System.Windows.Forms.TrackBar sliderVolume;
		private System.Windows.Forms.GroupBox groupBox;
		private IRemotePremiseObject m_mz = null;
		private System.Windows.Forms.CheckBox cbPower;
		
		/// <summary> 
		/// Required designer variable.
		/// </summary>
		private System.ComponentModel.Container components = null;

		public override String Text
		{
			set
			{
				groupBox.Text = value;
			}
			get
			{
				return groupBox.Text;
			}
		}

		public MediaZone()
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
					m_mz = null;
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
			this.cbSource = new System.Windows.Forms.ComboBox();
			this.checkMute = new System.Windows.Forms.CheckBox();
			this.sliderVolume = new System.Windows.Forms.TrackBar();
			this.groupBox = new System.Windows.Forms.GroupBox();
			this.cbPower = new System.Windows.Forms.CheckBox();
			((System.ComponentModel.ISupportInitialize)(this.sliderVolume)).BeginInit();
			this.groupBox.SuspendLayout();
			this.SuspendLayout();
			// 
			// cbSource
			// 
			this.cbSource.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
			this.cbSource.Font = new System.Drawing.Font("Verdana", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
			this.cbSource.Location = new System.Drawing.Point(88, 24);
			this.cbSource.Name = "cbSource";
			this.cbSource.Size = new System.Drawing.Size(184, 22);
			this.cbSource.TabIndex = 13;
			this.cbSource.SelectedIndexChanged += new System.EventHandler(this.cbSource_SelectedIndexChanged);
			// 
			// checkMute
			// 
			this.checkMute.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
			this.checkMute.Font = new System.Drawing.Font("Verdana", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
			this.checkMute.Location = new System.Drawing.Point(8, 48);
			this.checkMute.Name = "checkMute";
			this.checkMute.Size = new System.Drawing.Size(56, 24);
			this.checkMute.TabIndex = 11;
			this.checkMute.Text = "Mute";
			this.checkMute.CheckedChanged += new System.EventHandler(this.checkMute_CheckedChanged);
			// 
			// sliderVolume
			// 
			this.sliderVolume.Location = new System.Drawing.Point(80, 48);
			this.sliderVolume.Maximum = 100;
			this.sliderVolume.Name = "sliderVolume";
			this.sliderVolume.Size = new System.Drawing.Size(192, 45);
			this.sliderVolume.TabIndex = 10;
			this.sliderVolume.TickFrequency = 10;
			this.sliderVolume.TickStyle = System.Windows.Forms.TickStyle.Both;
			this.sliderVolume.Scroll += new System.EventHandler(this.sliderVolume_Scroll);
			// 
			// groupBox
			// 
			this.groupBox.Controls.Add(this.cbPower);
			this.groupBox.Controls.Add(this.checkMute);
			this.groupBox.Controls.Add(this.sliderVolume);
			this.groupBox.Controls.Add(this.cbSource);
			this.groupBox.Font = new System.Drawing.Font("Microstyle Bold Extended ATT", 12F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
			this.groupBox.Location = new System.Drawing.Point(0, 0);
			this.groupBox.Name = "groupBox";
			this.groupBox.Size = new System.Drawing.Size(288, 96);
			this.groupBox.TabIndex = 8;
			this.groupBox.TabStop = false;
			this.groupBox.Text = "Zone Name";
			this.groupBox.Enter += new System.EventHandler(this.groupBox_Enter);
			// 
			// cbPower
			// 
			this.cbPower.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
			this.cbPower.Font = new System.Drawing.Font("Verdana", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
			this.cbPower.Location = new System.Drawing.Point(8, 24);
			this.cbPower.Name = "cbPower";
			this.cbPower.Size = new System.Drawing.Size(64, 24);
			this.cbPower.TabIndex = 11;
			this.cbPower.Text = "Power";
			// 
			// MediaZone
			// 
			this.Controls.Add(this.groupBox);
			this.Name = "MediaZone";
			this.Size = new System.Drawing.Size(288, 96);
			((System.ComponentModel.ISupportInitialize)(this.sliderVolume)).EndInit();
			this.groupBox.ResumeLayout(false);
			this.ResumeLayout(false);

		}
		#endregion

		public IRemotePremiseObject PremiseObject
		{
			set
			{
				m_mz = value;
				if (m_mz != null) 
				{
					try 
					{
						groupBox.Text = m_mz.GetValue("DisplayName").ToString();
						try 
						{
							m_mz.SubscribeToProperty("DisplayName", "OnDisplayNameChanged", this);
							//							AdjustUI();
							//							TestCode();
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
				// Get sources
				IRemotePremiseObject PVID = m_mz.GetObject("sys://Devices/CustomDevices/PVIDSwitcher");
				IRemotePremiseObjectCollection inputs = PVID.GetObjectsByType("sys://Schema/Modules/PVID/Classes/PVIDInput", false);
				cbSource.Items.Clear();
				int n = 0;
				foreach (IRemotePremiseObject input in inputs)
				{
					if (++n < inputs.Count)
					{
						String s = input.GetValue("DisplayName").ToString();
						if (s.Length > 0) 
							cbSource.Items.Add(s);
					}
				}

				try 
				{
					IRemotePremiseObject avo = (IRemotePremiseObject)m_mz.GetValue("AudioVideoOutput");
					avo.SubscribeToProperty("CurrentSource", "OnCurrentSourceChanged", this);
				}
				catch (Exception e)
				{
					MessageBox.Show(e.Message + " SubscribeToProperty failed.","Problem", MessageBoxButtons.OK, MessageBoxIcon.Asterisk);
				}
				BindToSource();
			}
		}

		void BindToSource()
		{
			IRemotePremiseObject avo = (IRemotePremiseObject)m_mz.GetValue("AudioVideoOutput");

			sliderVolume.Value = (int)((double)m_mz.GetValue("Volume")*100);
			avo.SubscribeToProperty("Volume", "OnVolumeChanged", this);

			checkMute.Checked = (bool)m_mz.GetValue("Mute");
			avo.SubscribeToProperty("Mute", "OnMuteChanged", this);

			// 
			IRemotePremiseObjectCollection props = avo.GetProperties(true);
			Debug.WriteLine(avo.Path);
			for (int i=0; i < props.Count; i++)
			{
				IRemotePremiseObject prop= props.Item(i);
				Debug.WriteLine(i.ToString() + ":" + prop.GetValue("Name").ToString());
			}

			IRemotePremiseObject avosrc = (IRemotePremiseObject)avo.GetValue("CurrentSource");
			if (avosrc != null)
			{
				IRemotePremiseObject zonesrc = (IRemotePremiseObject)avo.GetValue("Source");
				props = zonesrc.GetProperties(true);
				Debug.WriteLine(zonesrc.Path);
				for (int i=0; i < props.Count; i++)
				{
					IRemotePremiseObject prop= props.Item(i);
					Debug.WriteLine(i.ToString() + ":" + prop.GetValue("Name").ToString());
				}

				IRemotePremiseObject source = (IRemotePremiseObject)zonesrc.GetValue("CurrentSource");
				int n = cbSource.FindStringExact(source.GetValue("DisplayName").ToString());
				cbSource.SelectedIndex = n;
			}
		}

		public virtual void OnDisplayNameChanged(int subID, IRemotePremiseObject objectChanged, IRemotePremiseObject property,  object newValue)
		{
			groupBox.Text = newValue.ToString();
		}

		public virtual void OnVolumeChanged(int subID, IRemotePremiseObject objectChanged, IRemotePremiseObject property,  object newValue)
		{
			sliderVolume.Value = (int)((double)newValue*100);
		}

		private void sliderVolume_Scroll(object sender, System.EventArgs e)
		{
			if (m_mz != null)
			{
				double vol = (double)sliderVolume.Value/100;
				m_mz.SetValue("Volume", vol);
			}
		}

		public virtual void OnCurrentSourceChanged(int subID, IRemotePremiseObject objectChanged, IRemotePremiseObject property,  object newValue)
		{
			BindToSource();
		}

		public virtual void OnMuteChanged(int subID, IRemotePremiseObject objectChanged, IRemotePremiseObject property,  object newValue)
		{
			checkMute.Checked = (bool)newValue;
		}

		private void checkMute_CheckedChanged(object sender, System.EventArgs e)
		{
			if (m_mz != null)
				m_mz.SetValue("Mute", checkMute.Checked);
		
		}

		private void cbSource_SelectedIndexChanged(object sender, System.EventArgs e)
		{
			String s = cbSource.GetItemText(cbSource.SelectedItem);
			//TODO: Change to grovel hierachy
			IRemotePremiseObject PVID = m_mz.GetObject("sys://Devices/CustomDevices/PVIDSwitcher");
			IRemotePremiseObjectCollection inputs = PVID.GetObjectsByTypeAndPropertyValue("sys://Schema/Modules/PVID/Classes/PVIDInput", "DisplayName", s, false);
			IRemotePremiseObject avo = m_mz.GetObject("sys://Devices/CustomDevices/PVIDSwitcher/PVIDOutputOffice");
			IRemotePremiseObject input = inputs.Item(0);
			avo.SetValue("CurrentSource", input);		
		}

		private void groupBox_Enter(object sender, System.EventArgs e)
		{
		
		}

		private void progressBar1_Click(object sender, System.EventArgs e)
		{
		
		}


	}
}
