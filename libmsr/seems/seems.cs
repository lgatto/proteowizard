using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Diagnostics;
using System.Threading;
using System.IO;
using System.Collections.Specialized;
using System.Xml;
using System.Text.RegularExpressions;
using ManagedLibMSR;
using JWC;
using Microsoft.Win32;
using Extensions;

namespace seems
{
	using DataSourceMap = Map<string, DataSource>;

	public partial class seems : Form
	{
		private OpenFileDialog browseToFileDialog;
		private ToolStripControlHost DummyScanNumberComboBoxHost = new ToolStripControlHost( new HeaderComboBox.HeaderComboBoxControl() );

		private MruStripMenu recentFilesMenu;
		private string seemsRegistryLocation = "Software\\SeeMS";
		private RegistryKey seemsRegistryKey;

		private DataSourceMap dataSources;

		public ToolStrip ToolStrip1 { get { return toolStrip1; } }
		public ToolStripLabel ToolStripScanLabel { get { return toolStripLabel2; } }
		public StatusStrip StatusStrip1 { get { return statusStrip1; } }
		public ToolStripStatusLabel StatusLabel { get { return toolStripStatusLabel1; } }
		public ToolStripProgressBar StatusProgressBar { get { return toolStripProgressBar1; } }
		public GraphForm CurrentGraphForm { get { return ( (GraphForm) ActiveMdiChild ); } }
		public ToolStripMenuItem CentroidMenuItem { get { return centroidToolStripMenuItem; } }
		public ToolStripMenuItem DeisotopeMenuItem { get { return deisotopeToolStripMenuItem; } }
		public ToolStripMenuItem UseVendorCentroidMenuItem { get { return vendorToolStripMenuItem; } }
		public ToolStripDropDownButton PeakProcessingButton { get { return peakProcessingToolStripDropDownButton; } }
		public ToolStripDropDownButton AnnotateButton { get { return annotateToolStripDropDownButton; } }

		public seems( string[] args )
		{
			InitializeComponent();

			seemsRegistryKey = Registry.CurrentUser.OpenSubKey( seemsRegistryLocation );
			if( seemsRegistryKey != null )
				seemsRegistryKey.Close();

			recentFilesMenu = new MruStripMenu( recentFilesFileMenuItem, new MruStripMenu.ClickedHandler( recentFilesFileMenuItem_Click ), seemsRegistryLocation + "\\Recent File List", true );

			dataSources = new Map<string, DataSource>();

			setFileControls( true );
			setScanControls( false );

			DummyScanNumberComboBoxHost.Control.Anchor = System.Windows.Forms.AnchorStyles.Right;
			DummyScanNumberComboBoxHost.Control.AutoSize = false;
			DummyScanNumberComboBoxHost.Control.Enabled = false;
			DummyScanNumberComboBoxHost.Control.Location = new System.Drawing.Point( 0, 0 );
			DummyScanNumberComboBoxHost.Control.Name = "scanNumberComboBox";
			DummyScanNumberComboBoxHost.Control.Size = new Size( 100, 22 );
			DummyScanNumberComboBoxHost.Control.TabIndex = 8;
			ToolStrip1.Items.Insert( ToolStrip1.Items.IndexOf( toolStripLabel2 ), DummyScanNumberComboBoxHost );
			DummyScanNumberComboBoxHost.Enabled = false;
			DummyScanNumberComboBoxHost.Alignment = ToolStripItemAlignment.Right;
			DummyScanNumberComboBoxHost.Control.Dock = DockStyle.Fill;
			DummyScanNumberComboBoxHost.AutoSize = false;

			browseToFileDialog = new OpenFileDialog();
			browseToFileDialog.Filter =
				"Any spectra format (*.mzData;*.mzXML;*.xml;*.raw;*.wiff;*.mgf;*.dta;fid;*.baf;*.yep)|*.mzData;*.mzXML;*.xml;*.raw;*.wiff;*.mgf;*.dta;fid;*.baf;*.yep|" +
				"mzData (*.mzData;*.xml)|*.mzData;*.xml|" +
				"mzXML (*.mzXML;*.xml)|*.mzXML;*.xml|" +
				"RAW (*.RAW)|*.raw|" +
				"WIFF (*.WIFF)|*.wiff|" +
				"Bruker (fid;*.baf;*.yep)|fid;*.baf;*.yep|" +
				"MGF (*.mgf)|*.mgf|" +
				"DTA (*.dta)|*.dta";
			browseToFileDialog.FilterIndex = 0;
			browseToFileDialog.InitialDirectory = "C:\\";

			ManagedScan.SpectrumSVGTemplateFilepath = Path.Combine( Application.StartupPath, "SpectrumSVG-template.svg" );

			if( args.Length > 0 )
			{
				this.BringToFront();
				this.Focus();
				this.Activate();
				this.Show();
				Application.DoEvents();

				try
				{
					openFile( args[0] );

					if( args.Length > 1 )
					{
						try
						{
							//browserForm.ScanNumberComboBox.SelectedIndex = Convert.ToInt32( args[1] );
						} catch
						{
						}
					}
				} catch( Exception ex )
				{
					string message = ex.Message;
					if( ex.InnerException != null )
						message += "\n\nAdditional information: " + ex.InnerException.Message;
					MessageBox.Show( message,
									"Error recovering from crash",
									MessageBoxButtons.OK, MessageBoxIcon.Error, MessageBoxDefaultButton.Button1,
									0, false );
				}
			}
		}

		public void setFileControls( bool enabled )
		{
			openFileToolStripButton.Enabled = enabled;
			openFileMenuItem.Enabled = enabled;
			closeAllWindowMenuItem.Enabled = enabled;
		}

		public void setScanControls( bool enabled )
		{
			peakProcessingToolStripDropDownButton.Enabled = enabled;
			annotateToolStripDropDownButton.Enabled = enabled;
			if( CurrentGraphForm != null )
			{
				CurrentGraphForm.ScanNumberComboBox.Enabled = enabled;
				if( enabled == true )
				{
					SeemsScan selectedScan = (SeemsScan) CurrentGraphForm.ScanNumberComboBox.SelectedItem;

					if( selectedScan.Scan.IsCentroided )
					{
						CentroidMenuItem.Enabled = false;
						CentroidMenuItem.Checked = true;
					} else
						CentroidMenuItem.Enabled = true;

					if( selectedScan.IsMassSpectrum )
					{
						peakProcessingToolStripDropDownButton.Enabled = true;
						peptideFragmentationToolStripMenuItem.Enabled = true;
						peptideMassMappingToolStripMenuItem.Enabled = true;
					} else
					{
						peakProcessingToolStripDropDownButton.Enabled = false;
						peptideFragmentationToolStripMenuItem.Enabled = false;
						peptideMassMappingToolStripMenuItem.Enabled = false;
					}
				}
			}
		}

		private void openFile( string filepath )
		{
			try
			{
				DataSourceMap.InsertResult insertResult = dataSources.Insert( filepath, null );
				if( insertResult.WasInserted )
				{
					// file was not already open; create a new data source
					insertResult.Element.Value = new DataSource( filepath );
					insertResult.Element.Value.StatusReport += new StatusReportEventHandler( openFile_StatusReport );
					insertResult.Element.Value.ProgressReport += new ProgressReportEventHandler( openFile_ProgressReport );
					insertResult.Element.Value.SetInputFileCompleted += new SetInputFileCompletedEventHandler( openFile_SetInputFileCompleted );
				} else
					GraphForm.CreateNewWindow( this, insertResult.Element.Value, true );

				// update recent files list
				recentFilesMenu.AddFile( filepath, Path.GetFileName( filepath ) );
				recentFilesMenu.SaveToRegistry();

			} catch( Exception ex )
			{
				string message = ex.Message;
				if( ex.InnerException != null )
					message += "\n\nAdditional information: " + ex.InnerException.Message;
				MessageBox.Show( message,
								"Error opening source file",
								MessageBoxButtons.OK, MessageBoxIcon.Error, MessageBoxDefaultButton.Button1,
								0, false );
			}
		}

		private delegate void SetStatusLabelCallback( string status );
		private void SetStatusLabel( string status )
		{
			if( toolStrip1.InvokeRequired )
			{
				SetStatusLabelCallback d = new SetStatusLabelCallback( SetStatusLabel );
				Invoke( d, new object[] { status } );
			} else
			{
				if( status.Length > 0 )
				{
					toolStripStatusLabel1.Text = status;
					toolStripStatusLabel1.Visible = true;
				} else
					toolStripStatusLabel1.Visible = false;
				toolStrip1.Refresh();
			}
		}

		private delegate void SetProgressPercentageCallback( int percentage );
		private void SetProgressPercentage( int percentage )
		{
			if( toolStrip1.InvokeRequired )
			{
				SetProgressPercentageCallback d = new SetProgressPercentageCallback( SetProgressPercentage );
				Invoke( d, new object[] { percentage } );
			} else
			{
				switch( percentage )
				{
					case 0:
						toolStripProgressBar1.Visible = true;
						toolStripProgressBar1.Minimum = 0;
						toolStripProgressBar1.Maximum = 100;
						toolStripProgressBar1.Value = 0;
						break;
					case 100:
						toolStripProgressBar1.Visible = false;
						break;
					default:
						toolStripProgressBar1.Value = percentage;
						break;
				}
				toolStrip1.Refresh();
			}
		}

		public delegate GraphForm CreateNewWindowCallback( Form mdiParent, DataSource dataSource, bool giveFocus );
		private void openFile_SetInputFileCompleted( object sender, SetInputFileCompletedEventArgs e )
		{
			if( InvokeRequired )
			{
				CreateNewWindowCallback d = new CreateNewWindowCallback( GraphForm.CreateNewWindow );
				Invoke( d, new object[] { this, e.DataSource, true } );
			} else
			{
				GraphForm.CreateNewWindow( this, e.DataSource, true );
			}
		}

		private void openFile_StatusReport( object sender, StatusReportEventArgs e )
		{
			SetStatusLabel( e.Status );
		}

		private void openFile_ProgressReport( object sender, ProgressReportEventArgs e )
		{
			SetProgressPercentage( e.Percentage );
		}

		private void openFile_Click( object sender, EventArgs e )
		{
			if( browseToFileDialog.ShowDialog() == DialogResult.OK )
			{
				openFile( browseToFileDialog.FileName );
			}
		}

		private void centroidToolStripMenuItem_CheckedChanged( object sender, EventArgs e )
		{
			CurrentGraphForm.updateGraph();
		}

		private void peptideFragmentationToolStripMenuItem_Click( object sender, EventArgs e )
		{
			PeptideFragmentationForm peptideFragmentationForm = new PeptideFragmentationForm();
			peptideFragmentationForm.ShowDialog( this );
			if( peptideFragmentationForm.DialogResult == DialogResult.Cancel )
				return;

			if( peptideFragmentationForm.MsProductReportXml != null )
			{
				Map<string, List<Pair<double, int>>> pointAnnotations = new Map<string, List<Pair<double, int>>>();

				XmlTextReader reader = new XmlTextReader( new StringReader( peptideFragmentationForm.MsProductReportXml ) );

				string ionSeriesName = "immonium";
				string ionSeriesIndex = "";
				int previousLabelCount = CurrentGraphForm.CurrentPointAnnotations.Count;
				while( reader.Read() )
				{
					switch( reader.NodeType )
					{
						case XmlNodeType.Element:
							if( reader.Name == "mz" )
							{
								string[] ionMzChargeStr = reader.ReadElementContentAsString().Split( ",".ToCharArray() );
								double ionMz = Convert.ToDouble( ionMzChargeStr[0] );
								int ionCharge = Convert.ToInt32( ionMzChargeStr[1] );
								pointAnnotations[ionSeriesName + ionSeriesIndex].Add( new Pair<double, int>( ionMz, ionCharge ) );
							} else if( reader.Name[0] == 'i' )
							{
								Int32 test;
								if( Int32.TryParse( reader.Name.Substring( 1 ), out test ) )
								{
									ionSeriesIndex = " " + reader.Name.Substring( 1 );
								}
							} else if( reader.Name == "name" )
							{
								ionSeriesName = reader.ReadElementContentAsString();
							}
							break;
					}
				}

				if( CurrentGraphForm.CurrentScan.IsMassSpectrum )
				{
					CurrentGraphForm.AnnotationSettings.setScanLabels( pointAnnotations );
					if( CurrentGraphForm.CurrentPointAnnotations.Count > previousLabelCount )
						CurrentGraphForm.updateGraph();
				}
			}
		}


		private void peptideMassMapProteinDigestToolStripMenuItem_Click( object sender, EventArgs e )
		{
			PeptideMassMapProteinDigestForm peptideMassMapProteinDigestForm = new PeptideMassMapProteinDigestForm();
			peptideMassMapProteinDigestForm.ShowDialog( this );
			if( peptideMassMapProteinDigestForm.DialogResult == DialogResult.Cancel )
				return;

			if( peptideMassMapProteinDigestForm.MsDigestReportXml != null )
			{
				Map<string, List<Pair<double, int>>> pointAnnotations = new Map<string, List<Pair<double, int>>>();

				XmlTextReader reader = new XmlTextReader( new StringReader( peptideMassMapProteinDigestForm.MsDigestReportXml ) );

				double peptideMonoMz = 0, peptideAvgMz = 0;
				int peptideCharge = 0;
				string peptideSequence;
				int previousLabelCount = CurrentGraphForm.CurrentPointAnnotations.Count;
				while( reader.Read() )
				{
					switch( reader.NodeType )
					{
						case XmlNodeType.Element:
							if( reader.Name == "peptide" )
							{
								peptideMonoMz = peptideAvgMz = 0;
								peptideCharge = 0;
								peptideSequence = "";
							} else if( reader.Name == "mi_m_over_z" )
							{
								peptideMonoMz = Convert.ToDouble( reader.ReadElementContentAsString() );
							} else if( reader.Name == "av_m_over_z" )
							{
								peptideAvgMz = Convert.ToDouble( reader.ReadElementContentAsString() );
							} else if( reader.Name == "charge" )
							{
								peptideCharge = Convert.ToInt32( reader.ReadElementContentAsString() );
							} else if( reader.Name == "database_sequence" )
							{
								peptideSequence = reader.ReadElementContentAsString();
								if( peptideMonoMz > 0 ) pointAnnotations[peptideSequence].Add( new Pair<double, int>( peptideMonoMz, peptideCharge ) );
								if( peptideAvgMz > 0 ) pointAnnotations[peptideSequence].Add( new Pair<double, int>( peptideAvgMz, peptideCharge ) );
							}
							break;
					}
				}

				if( CurrentGraphForm.CurrentScan.IsMassSpectrum )
				{
					CurrentGraphForm.AnnotationSettings.setScanLabels( pointAnnotations );
					if( CurrentGraphForm.CurrentPointAnnotations.Count > previousLabelCount )
						CurrentGraphForm.updateGraph();
				}
			}
		}

		private void clearToolStripMenuItem_Click( object sender, EventArgs e )
		{
			if( CurrentGraphForm.CurrentPointAnnotations.Count == 0 )
				return;

			CurrentGraphForm.CurrentPointAnnotations.Clear();
			CurrentGraphForm.updateGraph();
		}

		private void settingsToolStripMenuItem_Click( object sender, EventArgs e )
		{
			AnnotationSettingsForm annotationSettingsForm = new AnnotationSettingsForm( CurrentGraphForm );
			annotationSettingsForm.ShowDialog( this );
		}

		private void manualEditToolStripMenuItem_Click( object sender, EventArgs e )
		{
			AnnotationEditForm annotationEditForm = new AnnotationEditForm( CurrentGraphForm );
			annotationEditForm.ShowDialog( this );
		}

		private void seems_FormClosing( object sender, FormClosingEventArgs e )
		{
			foreach( Map<string, DataSource>.MapPair sourceItr in dataSources )
				if( sourceItr.Value != null && sourceItr.Value.InstrumentInterface != null )
					sourceItr.Value.InstrumentInterface.freeInterface();
		}

		private void seems_Resize( object sender, EventArgs e )
		{
			//if( MouseButtons == MouseButtons.None && WindowState != FormWindowState.Minimized )
			//	webBrowser1.Refresh();
		}

		private void seems_MdiChildActivate( object sender, EventArgs e )
		{
			ToolStrip1.Items.Remove( DummyScanNumberComboBoxHost );
			foreach( Form f in MdiChildren )
				ToolStrip1.Items.Remove( ( (GraphForm) f ).ScanNumberComboBoxHost );

			if( CurrentGraphForm != null )
			{
				ToolStrip1.Items.Insert( ToolStrip1.Items.Count - 2, CurrentGraphForm.ScanNumberComboBoxHost );

				SeemsScan selectedScan = (SeemsScan) CurrentGraphForm.ScanNumberComboBox.SelectedItem;

				if( selectedScan != null && CurrentGraphForm.ScanNumberComboBox.Items.Count > 0 )
					setScanControls( true );
				else
					setScanControls( false );
			}

			if( ActiveMdiChild == null )
			{
				ToolStrip1.Items.Insert( ToolStrip1.Items.Count - 2, DummyScanNumberComboBoxHost );
				DummyScanNumberComboBoxHost.Enabled = false;
			}
		}

		private void cascadeWindowMenuItem_Click( object sender, EventArgs e )
		{
			LayoutMdi( MdiLayout.Cascade );
		}

		private void tileHorizontalWindowMenuItem_Click( object sender, EventArgs e )
		{
			LayoutMdi( MdiLayout.TileHorizontal );
		}

		private void tileVerticalWindowMenuItem_Click( object sender, EventArgs e )
		{
			LayoutMdi( MdiLayout.TileVertical );
		}

		private void arrangeIconsWindowMenuItem_Click( object sender, EventArgs e )
		{
			LayoutMdi( MdiLayout.ArrangeIcons );
		}

		private void closeAllWindowMenuItem_Click( object sender, EventArgs e )
		{
			foreach( Form f in MdiChildren )
				f.Close();
			ToolStrip1.Items.Insert( ToolStrip1.Items.Count - 2, DummyScanNumberComboBoxHost );
			DummyScanNumberComboBoxHost.Enabled = false;
		}

		private void recentFilesFileMenuItem_Click( int index, string filepath )
		{
			openFile( filepath );
		}

		private void exitFileMenuItem_Click( object sender, EventArgs e )
		{
			Application.Exit();
		}

		private void aboutHelpMenuItem_Click( object sender, EventArgs e )
		{
			MessageBox.Show( "© 2008 Matt Chambers, David L. Tabb", "About", MessageBoxButtons.OK, MessageBoxIcon.Information );
		}

		// workaround for MDI Window list bug
		private void windowToolStripMenuItem_DropDownOpening( object sender, EventArgs e )
		{
			if( ActiveMdiChild != null )
			{
				Form activeMdiChild = ActiveMdiChild;
				ActivateMdiChild( null );
				ActivateMdiChild( activeMdiChild );
			}
		}

		private void toolStripPanel1_Layout( object sender, LayoutEventArgs e )
		{
			
		}
	}
}