using System;
using System.Collections.Generic;
using System.Text;
using System.Windows.Forms;
using System.Diagnostics;
using System.IO;
using System.Threading;
using System.ComponentModel;
using ManagedLibMSR;
using Extensions;

using Chromatogram = Extensions.Map<double, double>;
using FragmentToChromatogramMap = Extensions.Map<double, Extensions.Map<double, double>>;
using ParentToFragmentMap = Extensions.Map<double, Extensions.Map<double, Extensions.Map<double, double>>>;

namespace seems
{
	public class ProgressReportEventArgs : EventArgs
	{
		// Summary:
		//     Gets the percentage complete of a DataSource operation.
		//
		// Returns:
		//     An integer representing the current percentage complete. Range: [0,100]
		public int Percentage { get { return percentage; } }
		private int percentage;

		internal ProgressReportEventArgs( int percentage )
		{
			this.percentage = percentage;
		}

		internal ProgressReportEventArgs( double percentage )
		{
			this.percentage = Convert.ToInt32( Math.Floor( percentage * 100 ) );
		}
	}

	public class StatusReportEventArgs : EventArgs
	{
		// Summary:
		//     Gets a string describing the status of a DataSource operation.
		//
		// Returns:
		//     A string representing the current status of DataSource operation.
		public string Status { get { return status; } }
		private string status;

		internal StatusReportEventArgs( string status )
		{
			this.status = status;
		}
	}

	public class SetInputFileCompletedEventArgs : EventArgs
	{
		public DataSource DataSource { get { return dataSource; } }
		private DataSource dataSource;

		internal SetInputFileCompletedEventArgs( DataSource dataSource )
		{
			this.dataSource = dataSource;
		}
	}

	public delegate void ProgressReportEventHandler( object sender, ProgressReportEventArgs e );
	public delegate void StatusReportEventHandler( object sender, StatusReportEventArgs e );
	public delegate void SetInputFileCompletedEventHandler( object sender, SetInputFileCompletedEventArgs e );

	public class DataSource
	{
		private ManagedInstrumentInterface instrumentInterface;
		private string sourceFilepath;
		private List<SeemsScan> sourceScanHeaders = new List<SeemsScan>();

		public ManagedInstrumentInterface InstrumentInterface { get { return instrumentInterface; } }
		public string CurrentFilepath { get { return sourceFilepath; } }
		//public SeemsScan CurrentChromatogram { get { return sourceChromatogram; } }
		public List<SeemsScan> CurrentScanHeaders { get { return sourceScanHeaders; } }

		public event ProgressReportEventHandler ProgressReport;
		public event StatusReportEventHandler StatusReport;
		public event SetInputFileCompletedEventHandler SetInputFileCompleted;

		protected void OnProgressReport( int percentageComplete )
		{
			if( ProgressReport != null )
				ProgressReport( this, new ProgressReportEventArgs( percentageComplete ) );
		}

		protected void OnStatusReport( string status )
		{
			if( StatusReport != null )
				StatusReport( this, new StatusReportEventArgs( status ) );
		}

		protected void OnSetInputFileCompleted()
		{
			if( SetInputFileCompleted != null )
				SetInputFileCompleted( this, new SetInputFileCompletedEventArgs( this ) );
		}

		private EventWaitHandle setInputFileWaitHandle;
		public EventWaitHandle SetInputFileEventWaitHandle { get { return setInputFileWaitHandle; } }

		public DataSource()
		{
			// create empty data source
		}

		public DataSource( string filepath )
		{
			instrumentInterface = new ManagedInstrumentInterface();
			if( !instrumentInterface.initInterface() )
				throw new Exception( "failed to initialize data interface." );

			setInputFileWaitHandle = new EventWaitHandle( false, EventResetMode.ManualReset );
			setInputFileDelegate = new ParameterizedThreadStart( startSetInputFile );
			Thread setInputFileThread = new Thread( setInputFileDelegate );
			//setInputFileThread.IsBackground = true;
			setInputFileThread.Start( (object) filepath );
			if( !instrumentInterface.setInputFile( filepath ) )
				throw new Exception( "LibMSR could not open the file" );
		}

		private ParameterizedThreadStart setInputFileDelegate;
		private void startSetInputFile( object threadArg )
		{
			SetInputFile( (string) threadArg );
			setInputFileWaitHandle.Set();
			OnSetInputFileCompleted();
		}

		public bool SetInputFile( string filepath )
		{
			ManagedInstrumentInterface tempInterface = new ManagedInstrumentInterface();

			try
			{
				if( !tempInterface.initInterface() )
					throw new Exception( "failed to initialize temporary data interface." );

				if( !File.Exists( filepath ) )
					throw new ArgumentException( "file does not exist" );
				sourceFilepath = filepath;

				OnStatusReport( "Initializing interface for source file..." );

				if( !tempInterface.setInputFile( sourceFilepath ) )
					throw new Exception( "LibMSR could not open the file" );
				tempInterface.setCentroiding( false, false );

			} catch( Exception ex )
			{
				string message = "SeeMS encountered an error opening \"" + filepath + "\" (" + ex.Message + ")";
				if( ex.InnerException != null )
					message += "\n\nAdditional information: " + ex.InnerException.Message;
				MessageBox.Show( message,
								"Error opening source file",
								MessageBoxButtons.OK, MessageBoxIcon.Error, MessageBoxDefaultButton.Button1,
								0, false );
				return false;
			}

			try
			{
				OnStatusReport( "Loading metadata from source file..." );
				OnProgressReport( 0 );

				int totalScanCount = tempInterface.getTotalScanCount();
				int curScanCount = 0;

				if( totalScanCount > 1 )
				{
					ManagedScan chromatogram = tempInterface.getChromatogram();
					if( chromatogram == null )
						throw new Exception( "failed to generate chromatogram" );
					else
						sourceScanHeaders.Add( new SeemsScan( chromatogram, "Chromatogram" ) );
				}

				ParentToFragmentMap transitionMap = new ParentToFragmentMap();

				// get all scans by sequential access
				ManagedScan curScan = tempInterface.getScanHeader( instrumentInterface.getFirstScanNumber() );
				while( curScan != null )
				{
					++curScanCount;
					if( ( curScanCount % 100 ) == 0 || curScanCount == totalScanCount )
					{
						OnStatusReport( String.Format( "Loading metadata from source file ({0} of {1})...",
										curScanCount, totalScanCount ) );
						OnProgressReport( curScanCount * 100 / totalScanCount );
					}

					if( curScan.ScanType == ScanTypeEnum.SRM || curScan.ScanType == ScanTypeEnum.MRM )
					{
						ManagedScan curScanWithPeakData = tempInterface.getScan( curScan.ScanNumber );
						if( curScanWithPeakData.getTotalPeakCount() > 0 )
						{
							if( curScan.getPrecursorScanCount() > 0 )
							{
								FragmentToChromatogramMap fragmentMap = transitionMap[curScan.getPrecursorScanInfo( 0 ).Mz];
								for( int i = 0; i < curScanWithPeakData.getTotalPeakCount(); ++i )
									fragmentMap[curScanWithPeakData.getPeakMz( i )].Add( curScan.RetentionTime, curScanWithPeakData.getPeakIntensity( i ) );
							}
						}
					} else
						sourceScanHeaders.Add( new SeemsScan( curScan ) );
					curScan = tempInterface.getScanHeader();
				}
				OnProgressReport( 100 );

				if( transitionMap.Count > 0 )
				{
					OnStatusReport( "Generating chromatograms for SRM/MRM data..." );
					OnProgressReport( 0 );
					Map<double, RefPair<ManagedScan, Map<double, ManagedScan>>> transitionChromatograms = new Map<double, RefPair<ManagedScan, Map<double, ManagedScan>>>();
					foreach( ParentToFragmentMap.MapPair pfPair in transitionMap )
					{
						Map<double, List<double>> parentPeaks = new Map<double, List<double>>();
						foreach( FragmentToChromatogramMap.MapPair fcPair in pfPair.Value )
						{
							ManagedScan fragmentChromatogram = transitionChromatograms[pfPair.Key].second[fcPair.Key] = new ManagedScan();
							foreach( Chromatogram.MapPair tiPair in fcPair.Value )
							{
								fragmentChromatogram.addPeak( tiPair.Key, tiPair.Value );
								parentPeaks[tiPair.Key].Add(tiPair.Value);
							}
						}

						ManagedScan parentChromatogram = transitionChromatograms[pfPair.Key].first = new ManagedScan();
						ManagedScan productChromatogram = transitionChromatograms[pfPair.Key].second[pfPair.Key] = new ManagedScan();
						foreach( Map<double, List<double>>.MapPair itr in parentPeaks )
						{
							double totalIntensity = 0, productIntensity = 1;
							foreach( double intensity in itr.Value )
							{
								totalIntensity += intensity;
								productIntensity *= intensity;
							}
							parentChromatogram.addPeak( itr.Key, totalIntensity );
							productChromatogram.addPeak( itr.Key, productIntensity );
						}

						OnProgressReport( transitionChromatograms.Count * 100 / transitionMap.Count );
					}

					foreach( Map<double, RefPair<ManagedScan, Map<double, ManagedScan>>>.MapPair kvp1 in transitionChromatograms )
					{
						sourceScanHeaders.Add( new SeemsScan( kvp1.Value.first, String.Format( "{0} (Sum)", Math.Round( kvp1.Key, 2 ) ) ) );
						sourceScanHeaders.Add( new SeemsScan( kvp1.Value.second[kvp1.Key], String.Format( "{0} (Product)", Math.Round( kvp1.Key, 2 ) ) ) );
						foreach( Map<double, ManagedScan>.MapPair kvp2 in kvp1.Value.second )
							if( kvp1.Key != kvp2.Key )
								sourceScanHeaders.Add( new SeemsScan( kvp2.Value, String.Format( "{0} -> {1}", Math.Round( kvp1.Key, 2 ), Math.Round( kvp2.Key, 2 ) ) ) );
					}

					OnStatusReport( "Finished loading source metadata." );
					OnProgressReport( 100 );
				}
			} catch( Exception ex )
			{
				string message = "SeeMS encountered an error reading metadata from \"" + filepath + "\" (" + ex.Message + ")";
				if( ex.InnerException != null )
					message += "\n\nAdditional information: " + ex.InnerException.Message;
				MessageBox.Show( message,
								"Error reading source metadata",
								MessageBoxButtons.OK, MessageBoxIcon.Error, MessageBoxDefaultButton.Button1,
								0, false );
				OnStatusReport( "Failed to read source metadata." );
				return false;
			}

			return true;
		}
	}
}
