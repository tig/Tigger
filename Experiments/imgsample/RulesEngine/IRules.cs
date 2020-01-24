using System;

namespace RulesEngine {
	public interface IRules 
	{
		string Execute(System.Web.HttpApplication Appl, string Path, string SettingsSection);
	}
}
