using System;
using System.Collections.Generic;
using System.Net;
using System.IO;
using System.Diagnostics;
//using System.Windows.Browser;

namespace Premise.Model
{
    /*
     * There is an issue with HttpWebRequest and WebRequest.  If the web service does not return anything
     * in the message body, the object will still linger and new requests will start to fail after about 8.
     * So, the "responseExpected" field is used to Abort the request after a short period.  This allows the
     * request to go out, but then aborts waiting for a response.  SYS's response to a "e" POST will not
     * contain a message body.
    */

    public class HttpHelper
    {
        /// <summary>
        /// OPTIMIZATION: Requests that expect a response are tracked in this list so that
        /// we do not request mulitple times. This is only for requests with "response = true"
        /// </summary>
        //static private List<Uri> OutstandingResponseRequests = new List<Uri>();

        private HttpWebRequest request;
        private System.Windows.Threading.DispatcherTimer canceltimer;

        // Content contains the data to be sent in the message body.
        public string content;
        /*
         * Some requests do not send a response.  This is set in those cases so that we 
         * can "cancel" the request after a short period of time (i.e. long enough for the request
         * to be sent over.
         */
        public bool responseExpected;

        public Object context;

        public event HttpResponseCompleteEventHandler ResponseComplete;
        private void OnResponseComplete(HttpResponseCompleteEventArgs e)
        {
            if (ResponseComplete != null)
                ResponseComplete(e);
        }
        public HttpHelper(Uri requestUri, string HttpMethod, string content, bool response)
            : this(requestUri, HttpMethod, content, response, null)
        {
        }

        public HttpHelper(Uri requestUri, string HttpMethod, string content, bool response, object context) 
        {
            //Debug.WriteLine("HttpHelper: requestURI = <{0}>, content = {1}, responseExpected = {2}", requestUri, content, response);

            this.request = (HttpWebRequest)WebRequest.Create(requestUri);
            this.request.ContentType = "application/x-www-form-urlencoded";
            this.request.Method = HttpMethod;

            this.content = content;
            this.responseExpected = response; 
            this.context = context;
        }

        public HttpHelper(Uri requestUri, string HttpMethod, string content, bool response, object context, string username, string password)
            : this(requestUri, HttpMethod, content, response, context) 
        {
            this.request.Credentials = new NetworkCredential(username, password); 
        }


        void OnCancel(object sender, EventArgs e)
        {
            Debug.WriteLine("HttpHelper::OnCancel");

            canceltimer.Stop();
            request.Abort();
        }
        public void Execute()
        {
            //Debug.WriteLine("HttpHelper.Execute: {0}", this.request.RequestUri);
            request.BeginGetRequestStream(new AsyncCallback(HttpHelper.BeginRequest), this);
            if (!responseExpected)
            {
                canceltimer = new System.Windows.Threading.DispatcherTimer();
                canceltimer.Interval = new TimeSpan(0, 0, 0, 0, 1000); // 200 msec should be long enough for request to happen.
                canceltimer.Tick += new EventHandler(OnCancel);
                canceltimer.Start();
            }
        }

        private static int _HttpRequestsOutstanding = 0;
        private static int HttpRequestsOutstanding
        {
            get
            {
                return _HttpRequestsOutstanding;
            }
            set
            {
                _HttpRequestsOutstanding = value;
                ViewModel.ViewModelLocator.MainViewModelStatic.ProgressIndicatorVisible = ((HttpRequestsOutstanding > 0) ? System.Windows.Visibility.Visible : System.Windows.Visibility.Collapsed);
            }
        }

        private static void BeginRequest(IAsyncResult ar)
        {
            //Debug.WriteLine("BeginRequest");
            HttpHelper helper = ar.AsyncState as HttpHelper;
            if (helper != null)
            {
                if (helper.request == null)
                {
                    Debug.WriteLine("request null");
                    return;
                }

                //if (helper.responseExpected && OutstandingResponseRequests.Contains(helper.request.RequestUri))
                //{
                //    Debug.WriteLine("Request already outstanding: {0}", helper.request.RequestUri);
                //    return;
                //}

                if (helper.content != null)
                {
                    using (StreamWriter writer = new StreamWriter(helper.request.EndGetRequestStream(ar)))
                    {
                        writer.Write(helper.content);
                    }
                }

                //if (helper.responseExpected)
                //    OutstandingResponseRequests.Add(helper.request.RequestUri);

                ++HttpRequestsOutstanding;
                Debug.WriteLine("BeginGetResponse: {0}", helper.request.RequestUri); 
                helper.request.BeginGetResponse(new AsyncCallback(HttpHelper.BeginResponse), helper);
            }
        }

        private static void BeginResponse(IAsyncResult ar)
        {
            //Debug.WriteLine("BeginResponse");
            HttpHelper helper = ar.AsyncState as HttpHelper;

            // When we aren't expecting a response, the timer will call Abort and we fall into here.
            // Check the response flag to see whether we should continue.
            if ((helper != null) && (helper.responseExpected))
            {
//                Debug.WriteLine("BeginResponse: uri = <{0}>", helper.request.RequestUri.ToString());

                try
                {
                    HttpWebResponse response = (HttpWebResponse)helper.request.EndGetResponse(ar);
                    if (response != null)
                    {
                        Stream stream = response.GetResponseStream();
                        if (stream != null)
                        {
                            using (StreamReader reader = new StreamReader(stream))
                            {
                                helper.OnResponseComplete(new HttpResponseCompleteEventArgs(reader.ReadToEnd(), helper.context, true));
                            }
                        }
                    }
                }
                catch (System.Net.WebException e)
                {
                    Debug.WriteLine(e.Status + " + " + e.Message);
                    helper.OnResponseComplete(new HttpResponseCompleteEventArgs(e.Status + " + " + e.Message, helper.context, false));
                    
                    helper.request.Abort();
                }
                finally
                {
                    //if (helper.responseExpected && OutstandingResponseRequests.Contains(helper.request.RequestUri))
                    //    OutstandingResponseRequests.Remove(helper.request.RequestUri);
                    --HttpRequestsOutstanding;
                }
            }
        }

    }

    public delegate void HttpResponseCompleteEventHandler(HttpResponseCompleteEventArgs e);
    public class HttpResponseCompleteEventArgs : EventArgs
    {
        public string Response;
        public Object context;
        public bool Succeeded;
        public HttpResponseCompleteEventArgs(string response, Object context, bool Succeeded)
        {
            this.Response = response;
            this.context = context;
            this.Succeeded = Succeeded;
        }
    }
}