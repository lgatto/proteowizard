﻿/*
 * Original author: Alana Killeen <killea .at. u.washington.edu>,
 *                  MacCoss Lab, Department of Genome Sciences, UW
 *
 * Copyright 2009 University of Washington - Seattle, WA
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
using System.IO;
using System.Collections.Generic;
using System.Linq;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using pwiz.ProteomeDatabase.API;
using pwiz.Skyline;
using pwiz.Skyline.Alerts;
using pwiz.Skyline.Controls;
using pwiz.Skyline.Controls.Graphs;
using pwiz.Skyline.Controls.SeqNode;
using pwiz.Skyline.EditUI;
using pwiz.Skyline.FileUI;
using pwiz.Skyline.Model;
using pwiz.Skyline.Model.Lib;
using pwiz.Skyline.Properties;
using pwiz.Skyline.SettingsUI;
using pwiz.Skyline.Util;
using pwiz.SkylineTestUtil;

namespace pwiz.SkylineTestTutorial
{
    /// <summary>
    /// Testing the tutorial for Skyline Targeted Method Editing
    /// </summary>
    [TestClass]
    public class MethodEditTutorialTest : AbstractFunctionalTest
    {
        private const string YEAST_ATLAS = "Yeast (Atlas)"; // Not L10N
        private const string YEAST_GPM = "Yeast (GPM)"; // Not L10N

        [TestMethod]
        public void TestMethodEditTutorial()
        {
            // Set true to look at tutorial screenshots.
            // IsPauseForScreenShots = true;

            LinkPdf = "https://skyline.gs.washington.edu/labkey/_webdav/home/software/Skyline/%40files/tutorials/MethodEdit-2_5.pdf";
            
            TestFilesZipPaths = new[]
            {
                @"https://skyline.gs.washington.edu/tutorials/MethodEdit.zip",
                @"TestTutorial\MethodEditCSVs.zip",
                @"TestTutorial\MethodEditViews.zip"
            };
            RunFunctionalTest();
        }

        protected override void DoTest()
        {
            // Creating a MS/MS Spectral Library, p. 1
            PeptideSettingsUI peptideSettingsUI = ShowDialog<PeptideSettingsUI>(SkylineWindow.ShowPeptideSettingsUI);
            RunDlg<BuildLibraryDlg>(peptideSettingsUI.ShowBuildLibraryDlg, buildLibraryDlg =>
            {
                buildLibraryDlg.LibraryPath = TestFilesDirs[0].GetTestPath(@"MethodEdit\Library\"); // Not L10N
                buildLibraryDlg.LibraryName = YEAST_ATLAS;
                buildLibraryDlg.LibraryCutoff = 0.95;
                buildLibraryDlg.LibraryAuthority = "peptideatlas.org"; // Not L10N
                buildLibraryDlg.OkWizardPage();
                IList<string> inputPaths = new List<string>
                 {
                     TestFilesDirs[0].GetTestPath(@"MethodEdit\Yeast_atlas\interact-prob.pep.xml") // Not L10N
                 };
                buildLibraryDlg.AddInputFiles(inputPaths);
                buildLibraryDlg.OkWizardPage();
            });

            PeptideSettingsUI peptideSettingsUI1 = peptideSettingsUI;
            RunUI(() =>
                {
                    peptideSettingsUI1.SelectedTab = PeptideSettingsUI.TABS.Library;
                    peptideSettingsUI1.PickedLibraries = new[] { YEAST_ATLAS };
                });
            WaitForOpenForm<PeptideSettingsUI>();   // To show Library tab for Forms testing
            PauseForScreenShot<PeptideSettingsUI.LibraryTab>("Peptide Settings - Library tab", 4); // Not L10N

            RunUI(() => peptideSettingsUI1.SelectedTab = PeptideSettingsUI.TABS.Digest);
            WaitForOpenForm<PeptideSettingsUI>();   // To show Digestion tab for Forms testing

            // Creating a Background Proteome File, p. 3
            FileEx.SafeDelete(TestFilesDirs[0].GetTestPath(@"MethodEdit\FASTA\Yeast" + ProteomeDb.EXT_PROTDB)); // Not L10N
            var buildBackgroundProteomeDlg =
                ShowDialog<BuildBackgroundProteomeDlg>(peptideSettingsUI.ShowBuildBackgroundProteomeDlg);
            RunUI(() =>
            {
                buildBackgroundProteomeDlg.BackgroundProteomePath =
                    TestFilesDirs[0].GetTestPath(@"MethodEdit\FASTA\Yeast"); // Not L10N
                buildBackgroundProteomeDlg.BackgroundProteomeName = "Yeast"; // Not L10N
                buildBackgroundProteomeDlg.AddFastaFile(
                    TestFilesDirs[0].GetTestPath(@"MethodEdit\FASTA\sgd_yeast.fasta")); // Not L10N
            });
            PauseForScreenShot<BuildBackgroundProteomeDlg>("Edit Background Proteome form", 5); // Not L10N

            OkDialog(buildBackgroundProteomeDlg, buildBackgroundProteomeDlg.OkDialog);

            PauseForScreenShot<PeptideSettingsUI.DigestionTab>("Peptide Settings - Digestion tab", 6); // Not L10N

            OkDialog(peptideSettingsUI, peptideSettingsUI.OkDialog);

            if (!TryWaitForCondition(() =>
                SkylineWindow.Document.Settings.PeptideSettings.Libraries.IsLoaded &&
                SkylineWindow.Document.Settings.PeptideSettings.Libraries.Libraries.Count > 0))
            {
                Assert.Fail("Timed out loading libraries: libCount={0}, NotLoadedExplained={1}",
                    SkylineWindow.Document.Settings.PeptideSettings.Libraries.Libraries.Count,
                    SkylineWindow.Document.Settings.PeptideSettings.Libraries.IsNotLoadedExplained ?? "<null>");
            }

            WaitForCondition(() =>
            {
                var peptideSettings = SkylineWindow.Document.Settings.PeptideSettings;
                var backgroundProteome = peptideSettings.BackgroundProteome;
                return (backgroundProteome.HasDigestion(peptideSettings));
            }, "backgroundProteome.HasDigestion");

            // Wait a bit in case web access is turned on and backgroundProteome is actually resolving protein metadata
            int millis = (AllowInternetAccess ? 300 : 60) * 1000;
            WaitForCondition(millis, () => !SkylineWindow.Document.Settings.PeptideSettings.BackgroundProteome.NeedsProteinMetadataSearch, "backgroundProteome.NeedsProteinMetadataSearch"); 

            // Pasting FASTA Sequences, p. 5
            RunUI(() => SetClipboardFileText(@"MethodEdit\FASTA\fasta.txt")); // Not L10N

            // New in v0.7 : Skyline asks about removing empty proteins.
            using (new CheckDocumentState(35, 25, 25, 75))
            {
                var emptyProteinsDlg = ShowDialog<EmptyProteinsDlg>(SkylineWindow.Paste);
                RunUI(() => emptyProteinsDlg.IsKeepEmptyProteins = true);
                OkDialog(emptyProteinsDlg, emptyProteinsDlg.OkDialog);
                WaitForCondition(millis, () => SkylineWindow.SequenceTree.Nodes.Count > 4);
            }

            RunUI(() =>
            {
                SkylineWindow.SequenceTree.SelectedNode = SkylineWindow.SequenceTree.Nodes[3].Nodes[0];
            });
            RestoreViewOnScreen(07);
            PauseForScreenShot("Main window", 7); // Not L10N

            RunUI(() =>
            {
                Settings.Default.ShowBIons = true;
                SkylineWindow.SequenceTree.SelectedNode.Expand();
                SkylineWindow.SequenceTree.SelectedNode =
                    SkylineWindow.SequenceTree.SelectedNode.Nodes[0].Nodes[1];
            });
            PauseForScreenShot("Main window showing effect of selection on Library Match graph", 8); // Not L10N

            CheckTransitionCount("VDIIANDQGNR", 3); // Not L10N

            using (new CheckDocumentState(35, 28, 31, 155))
            {
                var transitionSettingsUI = ShowDialog<TransitionSettingsUI>(SkylineWindow.ShowTransitionSettingsUI);
                RunUI(() =>
                    {
                        transitionSettingsUI.SelectedTab = TransitionSettingsUI.TABS.Filter;
                        transitionSettingsUI.PrecursorCharges = "2, 3"; // Not L10N
                        transitionSettingsUI.ProductCharges = "1"; // Not L10N
                        transitionSettingsUI.FragmentTypes = "y, b"; // Not L10N
                    });
                WaitForOpenForm<TransitionSettingsUI>();   // To show Filter tab for Forms testing
                PauseForScreenShot<TransitionSettingsUI.FilterTab>("Transition Settings - Filter tab", 9); // Not L10N
                RunUI(() =>
                    {
                        transitionSettingsUI.SelectedTab = TransitionSettingsUI.TABS.Library;
                        transitionSettingsUI.IonCount = 5;
                    });
                PauseForScreenShot<TransitionSettingsUI.LibraryTab>("Transition Settings - Library tab", 10); // Not L10N
                OkDialog(transitionSettingsUI, transitionSettingsUI.OkDialog);
            }
            PauseForScreenShot("Targets tree clipped from main window", 11); // Not L10N

            CheckTransitionCount("VDIIANDQGNR", 5); // Not L10N

            // Using a Public Spectral Library, p. 9
            peptideSettingsUI = ShowDialog<PeptideSettingsUI>(SkylineWindow.ShowPeptideSettingsUI);
            var editListUI =
                ShowDialog<EditListDlg<SettingsListBase<LibrarySpec>, LibrarySpec>>(peptideSettingsUI.EditLibraryList);
            var addLibUI = ShowDialog<EditLibraryDlg>(editListUI.AddItem);
            RunUI(() => addLibUI.LibrarySpec =
                new BiblioSpecLibSpec(YEAST_GPM, TestFilesDirs[0].GetTestPath(@"MethodEdit\Library\yeast_cmp_20.hlf"))); // Not L10N
            OkDialog(addLibUI, addLibUI.OkDialog);
            WaitForClosedForm(addLibUI);
            OkDialog(editListUI, editListUI.OkDialog);

            // Limiting Peptides per Protein, p. 11
            using (new CheckDocumentState(35, 182, 219, 1058))
            {
                RunUI(() =>
                    {
                        peptideSettingsUI.SelectedTab = PeptideSettingsUI.TABS.Library;
                        peptideSettingsUI.PickedLibraries = new[] {YEAST_ATLAS, YEAST_GPM};
                    });
                PauseForScreenShot<PeptideSettingsUI.LibraryTab>("Peptide Settings - Library tab", 12); // Not L10N
                OkDialog(peptideSettingsUI, peptideSettingsUI.OkDialog);
                Assert.IsTrue(WaitForCondition(
                    () =>
                        SkylineWindow.Document.Settings.PeptideSettings.Libraries.IsLoaded &&
                            SkylineWindow.Document.Settings.PeptideSettings.Libraries.Libraries.Count > 0));
            }

            using (new CheckDocumentState(35, 47, 47, 223, 2, true))    // Wait for change loaded, and expect 2 document revisions.
            {
                RunDlg<PeptideSettingsUI>(SkylineWindow.ShowPeptideSettingsUI, peptideSettingsUI2 =>
                    {
                        peptideSettingsUI2.PickedLibraries = new[] {YEAST_GPM};
                        peptideSettingsUI2.LimitPeptides = true;
                        peptideSettingsUI2.RankID = XHunterLibSpec.PEP_RANK_EXPECT;
                        peptideSettingsUI2.PeptidesPerProtein = 3;
                        peptideSettingsUI2.OkDialog();
                    });
            }

            using (new CheckDocumentState(19, 47, 47, 223))
            {
                RunUI(() =>
                    {
                        var refinementSettings = new RefinementSettings {MinPeptidesPerProtein = 1};
                        SkylineWindow.ModifyDocument("Remove empty proteins", refinementSettings.Refine); // Not L10N
                    });
            }

            // Inserting a Protein List, p. 11
            using (new CheckDocumentState(36, 58, 58, 278))
            {
                PasteDlg pasteProteinsDlg = ShowDialog<PasteDlg>(SkylineWindow.ShowPasteProteinsDlg);
                RunUI(() =>
                    {
                        var node = SkylineWindow.SequenceTree.Nodes[SkylineWindow.SequenceTree.Nodes.Count - 1];
                        SkylineWindow.SequenceTree.SelectedNode = node;
                        SetClipboardFileText(@"MethodEdit\FASTA\Protein list.txt"); // Not L10N
                        pasteProteinsDlg.SelectedPath = SkylineWindow.SequenceTree.SelectedPath;
                        pasteProteinsDlg.PasteProteins();
                    });
                PauseForScreenShot<PasteDlg.ProteinListTab>("Insert Protein List - For Screenshot, select last (empty) item in list", 14); // Not L10N
                OkDialog(pasteProteinsDlg, pasteProteinsDlg.OkDialog);
            }

            using (new CheckDocumentState(24, 58, 58, 278))
            {
                RunUI(() =>
                    {
                        var refinementSettings = new RefinementSettings {MinPeptidesPerProtein = 1};
                        SkylineWindow.ModifyDocument("Remove empty proteins", refinementSettings.Refine); // Not L10N
                    });
            }

            // Inserting a Peptide List, p. 13
            using (new CheckDocumentState(25, 70, 70, 338))
            {
                RunUI(() =>
                    {
                        SetClipboardFileText(@"MethodEdit\FASTA\Peptide list.txt"); // Not L10N
                        SkylineWindow.SequenceTree.SelectedNode = SkylineWindow.SequenceTree.Nodes[0];
                        SkylineWindow.Paste();
// ReSharper disable LocalizableElement
                        SkylineWindow.SequenceTree.Nodes[0].Text = "Primary Peptides"; // Not L10N
// ReSharper restore LocalizableElement
                    });
                FindNode("TLTAQSMQNSTQSAPNK"); // Not L10N
                PauseForScreenShot("Main window", 16); // Not L10N
            }

            using (new CheckDocumentState(35, 70, 70, 338))
            {
                RunUI(() => SkylineWindow.Undo());
                PasteDlg pastePeptidesDlg = ShowDialog<PasteDlg>(SkylineWindow.ShowPastePeptidesDlg);
                RunUI(pastePeptidesDlg.PastePeptides);
                PauseForScreenShot<PasteDlg.PeptideListTab>("Insert Peptide List -  For screenshot, select last (empty) line in list", 17); // Not L10N
                OkDialog(pastePeptidesDlg, pastePeptidesDlg.OkDialog);
            }

            // Simple Refinement, p. 16
            var findPeptideDlg = ShowDialog<FindNodeDlg>(SkylineWindow.ShowFindNodeDlg);
            RunUI(() => findPeptideDlg.SearchString = "IPEE"); // Not L10N
            OkDialog(findPeptideDlg, () =>
                                         {
                                             findPeptideDlg.FindNext();
                                             findPeptideDlg.Close();
                                         });
            PauseForScreenShot<GraphSpectrum>("Library Match graph metafile", 18); // Not L10N

            using (new CheckDocumentState(35, 64, 64, 320))
            {
                RefineDlg refineDlg = ShowDialog<RefineDlg>(SkylineWindow.ShowRefineDlg);
                RunUI(() => refineDlg.MinTransitions = 5);
                OkDialog(refineDlg, refineDlg.OkDialog);
                PauseForScreenShot("29/35 prot 50/64 pep 50/64 prec 246/320 tran", 18); // Not L10N
            }

            // Checking Peptide Uniqueness, p. 18
            RunUI(() =>
            {
                var node = SkylineWindow.SequenceTree.Nodes[SkylineWindow.SequenceTree.Nodes.Count - (TestSmallMolecules ? 3 : 2)];
                SkylineWindow.SequenceTree.SelectedNode = node;
            });

            using (new CheckDocumentState(34, 63, 63, 315))
            {
                var uniquePeptidesDlg = ShowDialog<UniquePeptidesDlg>(SkylineWindow.ShowUniquePeptidesDlg);
                WaitForConditionUI(() => uniquePeptidesDlg.GetDataGridView().RowCount == 1);
                RunUI(() =>
                    {
                        Assert.AreEqual(1, uniquePeptidesDlg.GetDataGridView().RowCount);
                        Assert.AreEqual(7, uniquePeptidesDlg.GetDataGridView().ColumnCount);
                    });
                PauseForScreenShot<UniquePeptidesDlg>("Unique Peptides form", 19); // Not L10N
                OkDialog(uniquePeptidesDlg, uniquePeptidesDlg.OkDialog);
                RunUI(() => SkylineWindow.EditDelete());
            }

            // Protein Name Auto-Completion
            PauseForScreenShot("(fig. 1): For screenshot, click at bottom of document tree, type 'ybl087' and see the autocomplete text.  Make sure to undo this new entry before proceeding.", 20); // Not L10N
            TestAutoComplete("ybl087", 0); // Not L10N
            var peptideGroups = new List<PeptideGroupDocNode>(Program.ActiveDocument.PeptideGroups);
            Assert.AreEqual("YBL087C", peptideGroups[peptideGroups.Count - 1].Name); // Not L10N

            // Protein Description Auto-Completion
            PauseForScreenShot("(fig. 2): For screenshot, click at bottom of document tree, type 'eft2' and see the autocomplete text, then down-arrow twice. Make sure to undo this new entry before proceeding.", 20); // Not L10N
            TestAutoComplete("eft2", 0); // Sorting logic puts this at the 0th entry in the list - Not L10N
            peptideGroups = new List<PeptideGroupDocNode>(Program.ActiveDocument.PeptideGroups);
            Assert.AreEqual("YDR385W", peptideGroups[peptideGroups.Count - 1].Name); // Not L10N

            // Peptide Sequence Auto-Completion, p. 21
            TestAutoComplete("IQGP", 0); // Not L10N
            var peptides = new List<PeptideDocNode>(Program.ActiveDocument.Peptides);
            Assert.AreEqual("K.AYLPVNESFGFTGELR.Q [769, 784]", peptides[peptides.Count - 1].Peptide.ToString()); // Not L10N
            PauseForScreenShot("(fig. 1) - For screenshot, click at the bottom of the document tree", 21); // Not L10N

            // Pop-up Pick-Lists, p. 21
            using (new CheckDocumentState(36, 71, 71, 355))
            {
                RunUI(() =>
                    {
                        var node = SkylineWindow.SequenceTree.Nodes[SkylineWindow.SequenceTree.Nodes.Count - (TestSmallMolecules ? 4 : 3)];
                        SkylineWindow.SequenceTree.SelectedNode = node;
                    });
                var pickList = ShowDialog<PopupPickList>(SkylineWindow.ShowPickChildrenInTest);
                RunUI(() =>
                    {
                        pickList.ApplyFilter(false);
                        pickList.SetItemChecked(8, true);
                        pickList.AutoManageChildren = false; // TODO: Because calling SetItemChecked does not do this
                    });
                PauseForScreenShot<PopupPickList>("(fig. 2) - YBL087C Peptides picklist", 21); // Not L10N
                RunUI(pickList.OnOk);
            }

            using (new CheckDocumentState(36, 71, 71, 355))
            {
                RunUI(() =>
                    {
                        SkylineWindow.SequenceTree.Nodes[34].ExpandAll();
                        var node =
                            SkylineWindow.SequenceTree.Nodes[34].Nodes[0].Nodes[0];
                        SkylineWindow.SequenceTree.SelectedNode = node;
                    });
                var pickList1 = ShowDialog<PopupPickList>(SkylineWindow.ShowPickChildrenInTest);
                RunUI(() =>
                    {
                        pickList1.SearchString = "y"; // Not L10N
                        pickList1.SetItemChecked(0, false);
                        pickList1.SetItemChecked(1, false);
                        pickList1.ApplyFilter(false);
                        pickList1.ToggleFind();
                        pickList1.SearchString = "b ++"; // Not L10N
                        pickList1.SetItemChecked(4, true);
                        pickList1.SetItemChecked(6, true);
                    });
                PauseForScreenShot<PopupPickList>("b ++ filtered picklist", 22); // Not L10N
                RunUI(pickList1.OnOk);
            }

            // Bigger Picture, p. 22. Drag and Drop, p. 23
            RunUI(() =>
            {
                ITipProvider nodeTip = SkylineWindow.SequenceTree.SelectedNode as ITipProvider;
                Assert.IsTrue(nodeTip != null && nodeTip.HasTip);
                var nodeName = SkylineWindow.SequenceTree.Nodes[1].Name;
                IdentityPath selectPath;
                SkylineWindow.ModifyDocument("Drag and drop", // Not L10N
                    doc => doc.MoveNode(SkylineWindow.Document.GetPathTo(0, 1), SkylineWindow.Document.GetPathTo(0, 0), out selectPath));
                Assert.IsTrue(SkylineWindow.SequenceTree.Nodes[0].Name == nodeName);
            });

            FindNode(string.Format("L [b5] - {0:F04}+", 484.3130)); // Not L10N - may be localized " (rank 3)"
            PauseForScreenShot("For Screenshots, First hover over YBL087C, then over 672.671+++", 23); // Not L10N

            // Preparing to Measure, p. 25
            RunDlg<TransitionSettingsUI>(() => SkylineWindow.ShowTransitionSettingsUI(TransitionSettingsUI.TABS.Prediction), transitionSettingsUI =>
            {
                transitionSettingsUI.RegressionCE = Settings.Default.GetCollisionEnergyByName("ABI 4000 QTrap"); // Not L10N
                transitionSettingsUI.RegressionDP = Settings.Default.GetDeclusterPotentialByName("ABI"); // Not L10N
                transitionSettingsUI.InstrumentMaxMz = 1800;
                transitionSettingsUI.OkDialog();
            });
            RunUI(() => SkylineWindow.SaveDocument(TestFilesDirs[0].GetTestPath("MethodEdit Tutorial.sky"))); // Not L10N
            var exportDialog = ShowDialog<ExportMethodDlg>(() =>
                SkylineWindow.ShowExportMethodDialog(ExportFileType.List));
            RunUI(() =>
            {
                exportDialog.ExportStrategy = ExportStrategy.Buckets;
                exportDialog.MethodType = ExportMethodType.Standard;
                exportDialog.OptimizeType = ExportOptimize.NONE;
                exportDialog.IgnoreProteins = true;
                exportDialog.MaxTransitions = 75;
            });
            PauseForScreenShot<ExportMethodDlg.TransitionListView>("Export Transition List form", 25); // Not L10N
            
            const string basename = "Yeast_list"; //  Not L10N
            OkDialog(exportDialog, () => exportDialog.OkDialog(TestFilesDirs[0].GetTestPath(basename)));  // write Yeast_list_000n.csv

            // check the output files
            for (int n = 0; n++ < 5;)
            {
                var csvname = String.Format("{0}_{1}.csv", basename, n.ToString("D4")); // Not L10N

                // AssertEx.FieldsEqual is hard-coded with CultureInfo.InvariantCulture, but so is transition list CSV export, so OK
                using (TextReader actual = new StreamReader(TestFilesDirs[0].GetTestPath(csvname)))
                using (TextReader target = new StreamReader(TestFilesDirs[1].GetTestPath(csvname)))
                {
                    AssertEx.FieldsEqual(target, actual, 6, null, true, TestSmallMolecules ? 3 : 0);
                }
            }
        }
        
        private void SetClipboardFileText(string filepath)
        {
            SetClipboardTextUI(File.ReadAllText(TestFilesDirs[0].GetTestPath(filepath)));
        }

        private static void TestAutoComplete(string text, int index)
        {
            var doc = WaitForDocumentLoaded();
            RunUI(() =>
            {
                var node = SkylineWindow.SequenceTree.Nodes[SkylineWindow.SequenceTree.Nodes.Count - 1];
                SkylineWindow.SequenceTree.SelectedNode = node;
                SkylineWindow.SequenceTree.BeginEdit(false);
                SkylineWindow.SequenceTree.StatementCompletionEditBox.TextBox.Text = text;
            });
            var statementCompletionForm = WaitForOpenForm<StatementCompletionForm>();
            Assert.IsNotNull(statementCompletionForm);
            RunUI(() => SkylineWindow.SequenceTree.StatementCompletionEditBox.OnSelectionMade(
                            (StatementCompletionItem)statementCompletionForm.ListView.Items[index].Tag));
            WaitForDocumentChangeLoaded(doc);
        }

        private static void CheckTransitionCount(string peptideSequence, int count)
        {
            var doc = SkylineWindow.Document;
            var nodePeptide = doc.Molecules.FirstOrDefault(nodePep =>
                Equals(peptideSequence, nodePep.Peptide.Sequence));
            Assert.IsNotNull(nodePeptide);
            Assert.IsTrue(nodePeptide.TransitionCount == count);
        }
    }
}
