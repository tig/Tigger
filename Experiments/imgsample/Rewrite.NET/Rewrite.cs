using System;
using System.IO;

namespace Rewrite.NET {
	public class Rewrite : System.Web.IHttpModule {

		/// <summary>
		/// Init is required from the IHttpModule interface
		/// </summary>
		/// <param name="Appl"></param>
		public void Init(System.Web.HttpApplication Appl) {
			//make sure to wire up to BeginRequest
			Appl.BeginRequest+=new System.EventHandler(Rewrite_BeginRequest);
		}

		/// <summary>
		/// Dispose is required from the IHttpModule interface
		/// </summary>
		public void Dispose() {
			//make sure you clean up after yourself
		}

		/// <summary>
		/// To handle the starting of the incoming request
		/// </summary>
		/// <param name="sender"></param>
		/// <param name="args"></param>
public void Rewrite_BeginRequest(object sender, System.EventArgs args) {
	//process rules here

	//cast the sender to a HttpApplication object
	System.Web.HttpApplication Appl=(System.Web.HttpApplication)sender;

	//load up the rules engine
	RulesEngine.Engine e = new RulesEngine.Engine(Appl);
	string r = e.Execute();		

	//only redirect if we have too
	if(r!="" && r.ToLower()!=RulesEngine.Engine.Getpath(Appl).ToLower() && !System.IO.File.Exists(Appl.Request.PhysicalPath))
			SendToNewUrl(r, Appl);
}

		public void SendToNewUrl(string url, System.Web.HttpApplication Appl) 
		{
			// If there's a query string, we have to call RewritePath(filePath, pathInfo, queryString)
			string [] split = url.Split(new Char[] {'?'}, 2);
			if (split.Length > 1)
			{
				String path = split[0];
				String query= split[1];
				Appl.Context.RewritePath(path, Appl.Context.Request.PathInfo, query);
			}
			else
				Appl.Context.RewritePath(url);
		}


	}
}
