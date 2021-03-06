﻿/*
 * Original author: Don Marsh <donmarsh .at. u.washington.edu>,
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
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Drawing;
using System.IO;
using System.Linq;
using System.Windows.Forms;
using TestRunnerLib;
using ZedGraph;
using Timer = System.Windows.Forms.Timer;

namespace SkylineTester
{
    partial class SkylineTesterWindow
    {
        private void Run(object sender, EventArgs e)
        {
            ShiftKeyPressed = (ModifierKeys == Keys.Shift);

            // Stop running task.
            if (_runningTab != null)
            {
                Stop(null, null);
                AcceptButton = DefaultButton;
                return;
            }

            commandShell.ClearLog();

            // Prepare to start task.
            _runningTab = _tabs[tabs.SelectedIndex];
            if (!_runningTab.Run())
                _runningTab = null;
            if (_runningTab == null)    // note: may be cleared by Run() (e.g., Cancel in DeleteWindow)
                return;

            foreach (var runButton in _runButtons)
                runButton.Text = "Stop";
            buttonStop.Enabled = true;
            AcceptButton = null;

            // Update elapsed time display.
            _runStartTime = DateTime.Now;
            if (_runTimer != null)
            {
                _runTimer.Stop();
                _runTimer.Dispose();
            }
            _runTimer = new Timer { Interval = 1000 };
            _runTimer.Tick += (s, a) =>
            {
                var elapsedTime = DateTime.Now - _runStartTime;
                statusRunTime.Text = "{0}:{1:D2}:{2:D2}".With(elapsedTime.Hours, elapsedTime.Minutes, elapsedTime.Seconds);
            };
            _runTimer.Start();
        }

        /// <summary>
        /// Allow the Tests tab to run the Quality tab without the overhead of loading the last log.
        /// </summary>
        public void RunQualityFromTestsTab()
        {
            LoadSummary();

            _runningTab = _tabQuality;
            _tabQuality.RunFromTestsTab();
            tabs.SelectTab(tabQuality);
        }

        public void ResetElapsedTime()
        {
            _runStartTime = DateTime.Now;
        }

        public void Stop()
        {
            RunUI(() => Stop(null, null));
        }

        private void Stop(object sender, EventArgs e)
        {
            _runningTab.Cancel();
        }

        public void Done()
        {
            _runningTab = null;

            foreach (var runButton in _runButtons)
                runButton.Text = "Run";
            buttonStop.Enabled = false;
            AcceptButton = DefaultButton;

            if (_runTimer != null)
            {
                _runTimer.Stop();
                _runTimer.Dispose();
                _runTimer = null;
            }

            SetStatus();

            if (_restart)
            {
                _restart = false;
                Run(null, null);
            }
        }

        public string GetBuildRoot()
        {
            var root = buildRoot.Text;
            if (!Path.IsPathRooted(root))
                root = Path.Combine(Environment.GetFolderPath(Environment.SpecialFolder.UserProfile), root);
            return root;
        }

        public string GetNightlyRoot()
        {
            var root = nightlyRoot.Text;
            if (!Path.IsPathRooted(root))
                root = Path.Combine(Environment.GetFolderPath(Environment.SpecialFolder.UserProfile), root);
            return root;
        }

        public string GetNightlyBuildRoot()
        {
            var nRoot = GetNightlyRoot();
            // Only use GetBuildRoot() if it's obviously been set by SkylineNightly.exe
            // - in which case it's going to be below GetNightlyRoot()
            if (!string.IsNullOrEmpty(buildRoot.Text) && GetBuildRoot().StartsWith(nRoot) && GetBuildRoot().Contains("SkylineTesterForNightly"))
                return GetBuildRoot();
            return nRoot;
        }

        public string GetLogsDir()
        {
            return Path.Combine(GetNightlyRoot(), "Logs");
        }

        public DirectoryInfo GetSkylineDirectory(string startDirectory)
        {
            string skylinePath = startDirectory;
            var skylineDirectory = skylinePath != null ? new DirectoryInfo(skylinePath) : null;
            while (skylineDirectory != null && skylineDirectory.Name != "Skyline")
                skylineDirectory = skylineDirectory.Parent;
            return skylineDirectory;
        }

        public void AddTestRunner(string args)
        {
            //MemoryChartWindow.Start("TestRunnerMemory.log");
            TestsRun = 0;
            if (Directory.Exists(_resultsDir))
                Try.Multi<Exception>(() => Directory.Delete(_resultsDir, true), 4, false);

            var testRunner = Path.Combine(GetSelectedBuildDir(), "TestRunner.exe");
            _testRunnerIndex = commandShell.Add(
                "{0} random=off status=on results={1} {2}{3}",
                testRunner.Quote(),
                _resultsDir.Quote(),
                AccessInternet.Checked ? "internet=on " : "",
                args);
        }

        public void ClearLog()
        {
            commandShell.ClearLog();
            _tabOutput.ClearErrors();
            _testRunnerIndex = int.MaxValue;
        }

        public void RunCommands()
        {
            RunningTestName = null;
            commandShell.FinishedOneCommand = () => { RunningTestName = null; };
            commandShell.Run(CommandsDone);
        }

        public int TestRunnerProcessId
        {
            get
            {
                return (commandShell.NextCommand == _testRunnerIndex + 1)
                    ? commandShell.ProcessId
                    : 0;
            }
        }

        private int _testRunnerIndex;

        public void CommandsDone(bool success)
        {
            commandShell.UpdateLog();

            if (commandShell.NextCommand > _testRunnerIndex)
            {
                _testRunnerIndex = int.MaxValue;

                // Report test results.
                var testRunner = Path.Combine(GetSelectedBuildDir(), "TestRunner.exe");
                commandShell.NextCommand = commandShell.Add("{0} report={1}", testRunner.Quote(), commandShell.LogFile.Quote());
                RunCommands();
                return;
            }

            commandShell.Done(success);

            if (_runningTab != null && _runningTab.Stop(success))
                _runningTab = null;
            if (_runningTab == null)
                Done();
        }

        private IEnumerable<string> GetLanguageNames()
        {
            foreach (var language in GetLanguages())
            {
                string name;
                if (_languageNames.TryGetValue(language, out name))
                    yield return name;
            }
        }

        /// <summary>
        /// Returns the set of languages which should always be present, regardless of the
        /// presence of resource DLLs.
        /// </summary>
        private IEnumerable<string> GetLanguages()
        {
            return (new FindLanguages(GetSelectedBuildDir(), "en", "fr", "tr").Enumerate());
        }

        public void InitLanguages(ComboBox comboBox)
        {
            string selectedItem = comboBox.SelectedItem != null ? comboBox.SelectedItem.ToString() : null;
            comboBox.Items.Clear();
            var languages = GetLanguageNames().ToList();
            foreach (var language in languages)
                comboBox.Items.Add(language);
            if (selectedItem != null)
                comboBox.SelectedItem = selectedItem;
            if (comboBox.SelectedIndex == -1)
                comboBox.SelectedIndex = 0;
        }

        public string GetCulture(ComboBox comboBox)
        {
            return _languageNames.First(x => x.Value == (string)comboBox.SelectedItem).Key;
        }

        public void RefreshLogs()
        {
            if (Tabs.SelectedTab == tabOutput)
                _tabOutput.Enter();
        }

        public void LoadSummary()
        {
            if (Summary == null)
            {
                var summaryLog = Path.Combine(GetLogsDir(), SummaryLog);
                Summary = new Summary(summaryLog);
            }
        }

        public void InitLogSelector(ComboBox combo, Button openButton = null)
        {
            combo.Items.Clear();
            
            LoadSummary();
            foreach (var run in Summary.Runs)
                AddRun(run, combo);

            if (openButton != null)
                openButton.Enabled = (combo.Items.Count > 0);
        }

        public void AddRun(Summary.Run run, ComboBox combo)
        {
            var text = run.Date.ToString("M/d  h:mm tt");
            if (run.Revision > 0)
                text += "    (rev. " + run.Revision + ")";
            combo.Items.Insert(0, text);
        }

        public string GetSelectedLog(ComboBox combo)
        {
            if (combo.SelectedIndex == -1)
                return null;

            int index = combo.SelectedIndex;
            var text = combo.Items[index].ToString();
            return (Char.IsDigit(text[0]))
                ? Summary.GetLogFile(Summary.Runs[combo.Items.Count - 1 - index])
                : DefaultLogFile;
        }

        public void OpenSelectedLog(ComboBox combo)
        {
            var file = GetSelectedLog(combo);
            if (File.Exists(file))
            {
                var editLogFile = new Process { StartInfo = { FileName = file } };
                editLogFile.Start();
            }
        }

        private ZedGraphControl CreateMemoryGraph()
        {
            var graph = InitGraph("Memory used");
            graph.IsShowPointValues = true;
            graph.PointValueEvent += GraphOnPointValueEvent;
            graph.MouseDownEvent += GraphOnMouseDownEvent;
            graph.MouseUpEvent += GraphOnMouseUpEvent;
            graph.MouseMoveEvent += GraphOnMouseMoveEvent;
            var pane = graph.GraphPane;
            pane.XAxis.Type = AxisType.Text;
            pane.XAxis.Scale.FontSpec.Family = "Courier New";
            pane.XAxis.Scale.FontSpec.Size = 12;
            pane.XAxis.Scale.Align = AlignP.Outside;
            pane.XAxis.MajorTic.IsAllTics = false;
            pane.XAxis.MinorTic.IsAllTics = false;
            pane.XAxis.Scale.IsVisible = true;
            pane.XAxis.Scale.Format = "#";
            pane.XAxis.Scale.Mag = 0;
            pane.YAxis.IsVisible = true;
            pane.YAxis.Scale.FontSpec.Family = "Courier New";
            pane.YAxis.Scale.FontSpec.Size = 12;
            pane.YAxis.Scale.Align = AlignP.Inside;
            pane.YAxis.MinorTic.IsAllTics = false;
            pane.YAxis.Title.Text = "MB";
            pane.YAxis.Scale.Format = "#";
            pane.YAxis.Scale.Mag = 0;
            pane.Legend.IsVisible = true;
            pane.YAxis.Scale.FontSpec.Angle = 90;
            return graph;
        }

        private bool GraphOnMouseMoveEvent(ZedGraphControl sender, MouseEventArgs e)
        {
            CurveItem nearestCurve;
            int index;
            if (sender.GraphPane.FindNearestPoint(new PointF(e.X, e.Y), out nearestCurve, out index))
                sender.Cursor = Cursors.Hand;
            return false;
        }

        private int _cursorIndex;

        private string GraphOnPointValueEvent(ZedGraphControl sender, GraphPane pane, CurveItem curve, int iPt)
        {
            _cursorIndex = iPt;
            return (string) curve[iPt].Tag;
        }

        private Point _mouseDownLocation;

        private bool GraphOnMouseUpEvent(ZedGraphControl sender, MouseEventArgs mouseEventArgs)
        {
            if (mouseEventArgs.Button == MouseButtons.Left && mouseEventArgs.Location == _mouseDownLocation)
                _tabs[_previousTab].MemoryGraphClick(_cursorIndex);
            return false;
        }

        private bool GraphOnMouseDownEvent(ZedGraphControl sender, MouseEventArgs mouseEventArgs)
        {
            if (mouseEventArgs.Button == MouseButtons.Left)
                _mouseDownLocation = mouseEventArgs.Location;
            return false;
        }

        private void InitQuality()
        {
            graphMemory = CreateMemoryGraph();
            panelMemoryGraph.Controls.Add(graphMemory);
        }

        public void InitNightly()
        {
            nightlyGraphMemory = CreateMemoryGraph();
            nightlyGraphPanel.Controls.Add(nightlyGraphMemory);

            graphTestsRun = InitGraph("Tests run");
            graphDuration = InitGraph("Duration");
            graphFailures = InitGraph("Failures");
            graphMemoryHistory = InitGraph("Memory used");

            nightlyTrendsTable.Controls.Clear();
            nightlyTrendsTable.Controls.Add(graphMemoryHistory, 0, 0);
            nightlyTrendsTable.Controls.Add(graphFailures, 0, 0);
            nightlyTrendsTable.Controls.Add(graphDuration, 0, 0);
            nightlyTrendsTable.Controls.Add(graphTestsRun, 0, 0);
        }

        private ZedGraphControl InitGraph(string title)
        {
            var graph = new ZedGraphControl
            {
                Dock = DockStyle.Fill,
                EditButtons = MouseButtons.Left,
                EditModifierKeys = Keys.None,
                IsEnableVPan = false,
                IsEnableVZoom = false,
                Margin = new Padding(5)
            };

            var pane = graph.GraphPane;
            pane.Title.Text = title;
            pane.Chart.Border.IsVisible = false;
            pane.YAxis.IsVisible = false;
            pane.X2Axis.IsVisible = false;
            pane.Y2Axis.IsVisible = false;
            pane.XAxis.MajorTic.IsOpposite = false;
            pane.YAxis.MajorTic.IsOpposite = false;
            pane.XAxis.MinorTic.IsOpposite = false;
            pane.YAxis.MinorTic.IsOpposite = false;
            pane.XAxis.MinorTic.IsAllTics = false;
            pane.XAxis.Title.IsVisible = false;
            pane.IsFontsScaled = false;
            pane.XAxis.Scale.MaxGrace = 0.0;
            pane.YAxis.Scale.MaxGrace = 0.05;
            pane.XAxis.Scale.FontSpec.Angle = 90;
            pane.Legend.IsVisible = false;

            return graph;
        }

        public bool HasBuildPrerequisites
        {
            get
            {
                GetBuildPrerequisites();

                if (Subversion == null)
                {
                    MessageBox.Show("Subversion is required to build Skyline.  You can install it from http://sourceforge.net/projects/win32svn/");
                    return false;
                }

                if (Devenv == null)
                {
                    MessageBox.Show("Visual Studio 12.0 is required to build Skyline.");
                    return false;
                }

                return true;
            }
        }

        public void GetBuildPrerequisites()
        {
            // Try to find where subversion is available.
            var programFiles = Environment.GetFolderPath(Environment.SpecialFolder.ProgramFilesX86);
            Subversion = Path.Combine(programFiles, @"Subversion\bin\svn.exe");
            if (!File.Exists(Subversion))
                Subversion = null;

            // Find Visual Studio, if available.
            Devenv = Path.Combine(programFiles, @"Microsoft Visual Studio 12.0\Common7\IDE\devenv.exe");
            if (!File.Exists(Devenv))
                Devenv = null;
        }

        public void RunUI(Action action, int delayMsec = 0)
        {
            if (delayMsec == 0)
            {
                if (!IsHandleCreated)
                    return;

                try
                {
                    Invoke(action);
                }
                catch (ObjectDisposedException)
                {
                }
            }
            else
            {
                var delayStart = new Timer { Interval = delayMsec };
                delayStart.Tick += (sender, args) =>
                {
                    ((Timer)sender).Stop();
                    RunUI(action);
                };
                delayStart.Start();
            }
        }

        private void RunUI(Action action)
        {
            if (!IsHandleCreated)
                return;

            try
            {
                Invoke(action);
            }
            catch (ObjectDisposedException)
            {
            }
        }
    }
}
