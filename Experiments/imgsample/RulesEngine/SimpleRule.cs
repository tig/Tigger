using System;

namespace RulesEngine {
	/// <summary>
	/// Summary description for SimpleRule.
	/// </summary>
	public class SimpleRule : IRules {
		public string Execute(System.Web.HttpApplication Appl, string Path, string SettingsSection){
			//load the settings in
			System.Collections.Specialized.NameValueCollection SimpleSettings = (System.Collections.Specialized.NameValueCollection)System.Configuration.ConfigurationSettings.GetConfig(SettingsSection);

			//see if we have a match
			for(int x=0;x<SimpleSettings.Count;x++) {
				string source=SimpleSettings.GetKey(x);
				string dest = SimpleSettings.Get(x);
				if(Path.ToLower() == source.ToLower()) return dest;
			}
			return "";
		}
	}
}
