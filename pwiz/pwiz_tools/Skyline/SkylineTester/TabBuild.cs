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

using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Windows.Forms;
using pwiz.Common.SystemUtil;

namespace SkylineTester
{
    public class TabBuild : TabBase
    {
        public TabBuild()
        {
            MainWindow.LabelSpecifyPath.Text =
                "(Specify absolute path or relative path from {0} folder)".With(Path.GetFileName(MainWindow.RootDir));
        }

        public override void Enter()
        {
            var buildRoot = MainWindow.GetBuildRoot();
            MainWindow.ButtonDeleteBuild.Enabled = Directory.Exists(buildRoot);
            MainWindow.DefaultButton = MainWindow.RunBuild;
        }

        public override bool Run()
        {
            if (!MainWindow.HasBuildPrerequisites)
                return false;
            var architectures = GetArchitectures();
            if (architectures.Count == 0)
            {
                MessageBox.Show("Select 32 or 64 bit architecture (or both).");
                return false;
            }

            StartLog("Build", MainWindow.DefaultLogFile, true);
            CreateBuildCommands(
                GetBranchUrl(),
                MainWindow.GetBuildRoot(), 
                architectures, 
                MainWindow.NukeBuild.Checked, 
                MainWindow.UpdateBuild.Checked,
                MainWindow.RunBuildVerificationTests.Checked);
            MainWindow.RunCommands();
            return true;
        }

        public override bool Stop(bool success)
        {
            if (success && MainWindow.StartSln.Checked && MainWindow.Devenv != null)
            {
                var buildRoot = MainWindow.GetBuildRoot();
                var slnDirectory = Path.Combine(buildRoot, @"pwiz_tools\Skyline");
                var process = new Process
                {
                    StartInfo =
                    {
                        FileName = Path.Combine(slnDirectory, "Skyline.sln"),
                        WorkingDirectory = slnDirectory,
                        UseShellExecute = true,
                    }
                };
                process.Start();
            }

            return true;
        }

        public override int Find(string text, int position)
        {
            return VerifyFind(text, position, "Build");
        }

        public static List<int> GetArchitectures()
        {
            var architectures = new List<int>();
            if (MainWindow.Build32.Checked)
                architectures.Add(32);
            if (MainWindow.Build64.Checked)
                architectures.Add(64);
            return architectures;
        }

        public static string GetBranchUrl()
        {
            return MainWindow.BuildTrunk.Checked
                ? @"https://svn.code.sf.net/p/proteowizard/code/trunk/pwiz"
                : MainWindow.BranchUrl.Text;
        }

        public static void CreateBuildCommands(
            string branchUrl, 
            string buildRoot, 
            IList<int> architectures, 
            bool nukeBuild, 
            bool updateBuild,
            bool runBuildTests)
        {
            var commandShell = MainWindow.CommandShell;
            var branchParts = branchUrl.Split('/');
            var branchName = "Skyline ({0}/{1})".With(branchParts[branchParts.Length - 2], branchParts[branchParts.Length - 1]);
            var subversion = MainWindow.Subversion;

            var architectureList = string.Join("- and ", architectures);
            commandShell.Add("# Build {0} {1}-bit...", branchName, architectureList);

            if (Directory.Exists(buildRoot))
            {
                if (nukeBuild)
                {
                    commandShell.Add("#@ Deleting Build directory...\n");
                    commandShell.Add("# Deleting Build directory...");
                    commandShell.Add("rmdir /s {0}", buildRoot.Quote());
                }
                else if (updateBuild)
                {
                    commandShell.Add("#@ Updating Build directory...\n");
                    commandShell.Add("# Updating Build directory...");
                    commandShell.Add("{0} cleanup {1}", subversion.Quote(), buildRoot.Quote());
                    commandShell.Add("{0} update {1}", subversion.Quote(), buildRoot.Quote());
                }
            }

            if (nukeBuild || updateBuild)
            {
                string tutorialsFolder = Path.Combine(PathEx.GetDownloadsPath(), "Tutorials");
                commandShell.Add("#@ Deleting Tutorials directory...\n");
                commandShell.Add("# Deleting Tutorials directory...");
                commandShell.Add("rmdir /s {0}", tutorialsFolder);
            }

            if (nukeBuild)
            {
                commandShell.Add("#@ Checking out {0} source files...\n", branchName);
                commandShell.Add("# Checking out {0} source files...", branchName);
                commandShell.Add("{0} checkout {1} {2}", subversion.Quote(), branchUrl.Quote(), buildRoot.Quote());
            }

            commandShell.Add("# Building Skyline...");
            commandShell.Add("cd {0}", buildRoot.Quote());
            foreach (int architecture in architectures)
            {
                commandShell.Add("#@ Building Skyline {0} bit...\n", architecture);
                commandShell.Add("{0} {1} {2} --i-agree-to-the-vendor-licenses toolset=msvc-12.0 nolog",
                    Path.Combine(buildRoot, @"pwiz_tools\build-apps.bat").Quote(),
                    architecture,
                    runBuildTests ? "pwiz_tools/Skyline" : "pwiz_tools/Skyline//Skyline.exe");
            }

            commandShell.Add("# Build done.");
        }

        public void DeleteBuild()
        {
            var buildRoot = MainWindow.GetBuildRoot();
            if (!Directory.Exists(buildRoot) ||
                MessageBox.Show(MainWindow, "Delete \"" + buildRoot + "\" folder?", "Confirm delete",
                    MessageBoxButtons.OKCancel) != DialogResult.OK)
            {
                return;
            }

            using (var deleteWindow = new DeleteWindow(buildRoot))
            {
                deleteWindow.ShowDialog();
            }

            MainWindow.ButtonDeleteBuild.Enabled = Directory.Exists(buildRoot);
        }

        public void BrowseBuild()
        {
            using (var dlg = new FolderBrowserDialog
            {
                Description = "Select or create a root folder for build source files.",
                ShowNewFolderButton = true
            })
            {
                if (dlg.ShowDialog(MainWindow) == DialogResult.OK)
                    MainWindow.BuildRoot.Text = dlg.SelectedPath;
            }

            MainWindow.ButtonDeleteBuild.Enabled = Directory.Exists(MainWindow.GetBuildRoot()); // MainWindow.GetBuildRoot() is the directory that actually gets deleted in DeleteBuild()
        }
    }
}
