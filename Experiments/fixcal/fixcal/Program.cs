using System;
using System.Diagnostics;
using System.Collections.Generic;
using System.Web;
using System.Net;
using Microsoft.Exchange.WebServices.Autodiscover;
using Microsoft.Exchange.WebServices.Data;

namespace fixcal
{
    class Program
    {
        public class ExchangeAutoDiscoveryException : Exception
        {
            public ExchangeAutoDiscoveryException() : base() { }
            public ExchangeAutoDiscoveryException(string message) : base(message) { }
            public ExchangeAutoDiscoveryException(string message, System.Exception inner) : base(message, inner) { }
            // A constructor is needed for serialization when an
            // exception propagates from a remoting server to the client. 
            protected ExchangeAutoDiscoveryException(System.Runtime.Serialization.SerializationInfo info,
                System.Runtime.Serialization.StreamingContext context) { }
        }

        public class ExTraceListener : ITraceListener
        {
            public List<string> traceMessages = new List<string>();
            public void Trace(string traceType, string traceMessage)
            {
                traceMessages.Add(traceType + ": " + traceMessage);
            }
        }

        static void Main(string[] args)
        {
            String username = "";
            String password = "";
            String domain = "";
            String replaceText = "Copy: ";  // Text to search for and replace in Subject lines

            ExchangeService service = null;

            Console.WriteLine("Charlie's Calendar Fixer");

            ExTraceListener listener = new ExTraceListener();
            try
            {
                // Test connection to provider.
                AutodiscoverService autodiscoverService = new AutodiscoverService();
                autodiscoverService.Credentials = new WebCredentials(username, password, domain);
                autodiscoverService.EnableScpLookup = false;
                autodiscoverService.RedirectionUrlValidationCallback = delegate { return true; };
                autodiscoverService.PreAuthenticate = true;
                autodiscoverService.TraceEnabled = true;
                autodiscoverService.TraceFlags = TraceFlags.All;
                autodiscoverService.TraceListener = listener;

                GetUserSettingsResponse userresponse = autodiscoverService.GetUserSettings(
                    username,
                    UserSettingName.ExternalEwsUrl,
                    UserSettingName.CasVersion,
                    UserSettingName.UserDisplayName);

                service = new ExchangeService();
                service.Credentials = new NetworkCredential(username, password);

                String url = userresponse.Settings[UserSettingName.ExternalEwsUrl].ToString();
                service.Url = new Uri(url);
                Console.WriteLine(String.Format("Exchange service url: {0}", service.Url));

                CalendarFolder calendar = (CalendarFolder)Folder.Bind(service, WellKnownFolderName.Calendar);
                FindItemsResults<Microsoft.Exchange.WebServices.Data.Appointment> findResultsUser =
                    calendar.FindAppointments(new CalendarView(DateTime.Today.AddDays(-1), DateTime.Today));
                //NameResolutionCollection resolvedNames = service.ResolveName(exchangeSettings.EmailAddress);
                NameResolutionCollection resolvedNames =
                    service.ResolveName(username, ResolveNameSearchLocation.DirectoryOnly, true);

                Console.WriteLine(String.Format("CAS Version: {0}", service.ServerInfo.VersionString));
            }
            catch (ServiceRequestException e)
            {
                string s = "";
                foreach (string trace in listener.traceMessages)
                    s = s + trace;
                throw new ExchangeAutoDiscoveryException("Autodiscovery failed. " + e.Message + " : " + s);
            }

            SearchFilter.SearchFilterCollection searchFilter = new SearchFilter.SearchFilterCollection();
            searchFilter.Add(new SearchFilter.IsGreaterThanOrEqualTo(AppointmentSchema.Start, new DateTime(1990, 1, 1)));
            searchFilter.Add(new SearchFilter.ContainsSubstring(ItemSchema.Subject, replaceText));
            ItemView view = new ItemView(2000);
            view.PropertySet = new PropertySet(BasePropertySet.FirstClassProperties,
                AppointmentSchema.Subject, 
                AppointmentSchema.Start, 
                AppointmentSchema.AppointmentType);

            // First, go through and update recurring masters
            FindItemsResults<Item> findResults = service.FindItems(WellKnownFolderName.Calendar, searchFilter, view);
            if (findResults.TotalCount > 0)
                service.LoadPropertiesForItems(findResults, new PropertySet(BasePropertySet.FirstClassProperties, AppointmentSchema.AppointmentType));
            foreach (Appointment a in findResults.Items)
            {
                if (a.IsRecurring && a.AppointmentType == AppointmentType.RecurringMaster)
                {
                    Console.Write(String.Format("RECURRING: {0}", a.Subject));
                    a.Subject = a.Subject.Remove(0, replaceText.Length);
                    a.Update(ConflictResolutionMode.AlwaysOverwrite, SendInvitationsOrCancellationsMode.SendToNone);
                    Console.WriteLine(String.Format(" -> {0}", a.Subject));
                }
            }

            // Then go through and fixup non-recurring items or exceptions.
            findResults = service.FindItems(WellKnownFolderName.Calendar, searchFilter, view);
            if (findResults.TotalCount > 0)
                service.LoadPropertiesForItems(findResults, new PropertySet(BasePropertySet.FirstClassProperties, AppointmentSchema.AppointmentType));
            foreach (Appointment a in findResults.Items)
            {
                Console.Write(String.Format("{0}", a.Subject));
                a.Subject = a.Subject.Remove(0, replaceText.Length);
                a.Update(ConflictResolutionMode.AlwaysOverwrite, SendInvitationsOrCancellationsMode.SendToNone);

                Console.WriteLine(String.Format(" -> {0}", a.Subject));
            }

            Console.WriteLine("Done.");
            Console.ReadKey();
        }
    }
}
