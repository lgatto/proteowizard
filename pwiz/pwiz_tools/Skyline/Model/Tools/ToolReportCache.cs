﻿/*
 * Original author: Trevor Killeen <killeent .at. u.washington.edu>,
 *                  MacCoss Lab, Department of Genome Sciences, UW
 *
 * Copyright 2013 University of Washington - Seattle, WA
 * 
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
using System.Collections.Generic;
using pwiz.Common.SystemUtil;
using pwiz.Skyline.Model.DocSettings;

namespace pwiz.Skyline.Model.Tools
{
    public class ToolReportCache
    {
        private readonly IDictionary<ReportSpec, string> _dictionary;
        private readonly LinkedList<ReportSpec> _list;
        private IDocumentContainer _documentContainer;
        private SrmDocument _document;
        private int CurrentSize { get; set; }
        private const int MAXIMUM_SIZE = 50 * 1024 * 1024;  // 50M

        private static ToolReportCache _reportCache;
        
        private ToolReportCache()
        {
            _dictionary = new Dictionary<ReportSpec, string>();
            _list = new LinkedList<ReportSpec>();
            CurrentSize = 0;
        }

        public static ToolReportCache Instance
        {
            get
            {
// ReSharper disable once ConvertIfStatementToNullCoalescingExpression
                if (_reportCache == null)
                    _reportCache = new ToolReportCache();
                return _reportCache;
            }
        }

        public void Register(IDocumentContainer container)
        {
            // Only one document container at a time
            if (_documentContainer != null)
                _documentContainer.Unlisten(OnDocumentChanged);
            _documentContainer = container;
            // A null container can be used to clear
            if (container == null)
            {
                _document = null;
            }
            else
            {
                _document = container.Document;
                container.Listen(OnDocumentChanged);
            }
            RefreshCache(_document);
        }

        protected void OnDocumentChanged(object sender, DocumentChangedEventArgs e)
        {
            RefreshCache(((IDocumentContainer) sender).Document);
        }

        /// <summary>
        /// The cache locking must be very fast to avoid blocking the UI
        /// </summary>
        private readonly object _cacheLock = new object();
        /// <summary>
        /// This lock is used to prevent multiple reports being generated
        /// at the same time.
        /// </summary>
        private readonly object _reportGenLock = new object();

        /// <summary>
        /// Returns the .csv file generated by exporting the given ReportSpec using data from the given Document.
        /// </summary>
        /// <param name="document">The current document open in Skyline</param>
        /// <param name="reportSpec">The export report format</param>
        /// <param name="progressMonitor"></param>
        public string GetReport(SrmDocument document, ReportSpec reportSpec, IProgressMonitor progressMonitor)
        {
            string report;
            if (TryGetReport(document, reportSpec, out report))
                return report;

            // Only on report at a time can be generated
            lock (_reportGenLock)
            {
                // Try again, in case we were waiting for the report we want to be generated
                if (TryGetReport(document, reportSpec, out report))
                    return report;

                report = TestReport ?? reportSpec.ReportToCsvString(document, progressMonitor);
                lock (_cacheLock)
                {
                    // if the document has changed while the report was exporting, don't add it to the cache
                    if (ReferenceEquals(document, _document))
                    {
                        CurrentSize += ReportSize(report);
                        while (CurrentSize > (TestMaximumSize ?? MAXIMUM_SIZE))
                        {
                            // remove the oldest reports
                            ReportSpec oldestSpec = _list.Last.Value;
                            string oldestReport;
                            if (_dictionary.TryGetValue(oldestSpec, out oldestReport))
                            {
                                _dictionary.Remove(oldestSpec);
                                CurrentSize -= ReportSize(oldestReport);
                            }
                            _list.RemoveLast();
                        }
                        _list.AddFirst(reportSpec);
                        _dictionary.Add(reportSpec, report);
                    }
                }
                return report;
            }
        }

        private bool TryGetReport(SrmDocument document, ReportSpec reportSpec, out string report)
        {
            lock (_cacheLock)
            {
                if (ReferenceEquals(document, _document) && _dictionary.TryGetValue(reportSpec, out report))
                {
                    _list.Remove(reportSpec);
                    _list.AddFirst(reportSpec);
                    return true;
                }
            }
            report = null;
            return false;
        }

        private void RefreshCache(SrmDocument document)
        {
            lock (_cacheLock)
            {
                _list.Clear();
                _dictionary.Clear();
                CurrentSize = 0;
                _document = document;
            }
        }

        public static int ReportSize(string report)
        {
            return report.Length*(sizeof (char));
        }

        #region Unit Test support

        public string TestReport { get; set; }
        public int? TestMaximumSize { get; set; }

        public bool ContainsKey(ReportSpec reportSpec)
        {
            return _dictionary.ContainsKey(reportSpec) && _list.Contains(reportSpec);
        }

        public bool ContainsValue(string value)
        {
            return _dictionary.Values.Contains(value);
        }

        public bool IsFirst(ReportSpec report)
        {
            return _list.First.Value.Equals(report);
        }

        public bool IsLast(ReportSpec report)
        {
            return _list.Last.Value.Equals(report);
        }

        public void ResetCache()
        {
            RefreshCache(null);
        }

        #endregion
    }

}

