using System;

namespace RulesEngine
{
	/// <summary>
	/// Summary description for RegExpRule.
	/// </summary>
	public class RegExp: RulesEngine.IRules
	{
		public RegExp()
		{
		}

		public string Execute(System.Web.HttpApplication Appl, string Path, string SettingsSection)
		{
			string input = RulesEngine.Engine.Getpath(Appl);

			System.Collections.Specialized.NameValueCollection Settings = (System.Collections.Specialized.NameValueCollection)System.Configuration.ConfigurationSettings.GetConfig(SettingsSection);

			//see if we have a match
			for(int x=0;x<Settings.Count;x++) 
			{

				string pattern=Settings.GetKey(x);

				string replacement = Settings.Get(x);

				input = System.Text.RegularExpressions.Regex.Replace(input, pattern, replacement);

			}
			return input;
		}
	}
}
