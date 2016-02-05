using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Drawing.Drawing2D;
using System.Text;
using System.Windows.Forms;

namespace seems
{
	public partial class GraphForm : Form
	{
		private DataSource dataSource;
		public DataSource DataSource { get { return dataSource; } set { setDataSource( value ); } }

		private HeaderComboBox.HeaderComboBoxControl scanNumberComboBox = new HeaderComboBox.HeaderComboBoxControl();
		private ToolStripControlHost scanNumberComboBoxToolStripHost;

		private SeemsAnnotationSettings annotationSettings = new SeemsAnnotationSettings();
		public SeemsAnnotationSettings AnnotationSettings { get { return annotationSettings; } }

		private string scanNumberHeaderString = String.Format( "{0,-8}{1,-10}{2,-5}{3,-15:E}{4,15}",
			"Number", "Time (s)", "MSn", "TIC", "Parent M/Z" );
		private string navigatingQuery = "";

		public string CurrentSourceFilepath { get { return dataSource.CurrentFilepath; } set { dataSource.SetInputFile(value); } }
		public SeemsScan CurrentScan { get { return (SeemsScan) scanNumberComboBox.SelectedItem; } }
		public int CurrentScanIndex { get { return scanNumberComboBox.SelectedIndex; } }
		public string CurrentNavigatingQuery { get { return navigatingQuery; } }
		public PointDataMap<SeemsPointAnnotation> CurrentPointAnnotations
		{
			get
			{
				if( CurrentScan.IsMassSpectrum )
					return annotationSettings.ScanPointAnnotations;
				else
					return annotationSettings.ChromatogramPointAnnotations;
			}
		}
				
		public HeaderComboBox.HeaderComboBoxControl ScanNumberComboBox { get { return scanNumberComboBox; } }
		public ToolStripControlHost ScanNumberComboBoxHost { get { return scanNumberComboBoxToolStripHost; } }
		public ZedGraph.ZedGraphControl ZedGraphControl { get { return zedGraphControl1; } }

		private seems SeemsMdiParent { get { return (seems) MdiParent; } }

		public GraphForm()
		{
			InitializeComponent();
			InitializeGraph();
		}

		public GraphForm( DataSource dataSource )
		{
			InitializeComponent();
			InitializeGraph();

			DataSource = dataSource;
		}

		private void InitializeGraph()
		{
			scanNumberComboBox.AccessibleName = "scanNumberComboBox";
			scanNumberComboBox.Anchor = System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right;
			scanNumberComboBox.AutoSize = false;
			scanNumberComboBox.Enabled = false;
			scanNumberComboBox.IntegralHeight = true;
			scanNumberComboBox.ItemHeight = 13;
			scanNumberComboBox.ListDisplayMember = "ComboBoxView";
			scanNumberComboBox.Name = "scanNumberComboBox";
			scanNumberComboBox.SelectedIndex = -1;
			scanNumberComboBox.SelectedItem = null;
			scanNumberComboBox.SelectedValue = null;
			scanNumberComboBox.Size = new Size( 72, 22 );
			scanNumberComboBox.TabIndex = 8;
			scanNumberComboBox.TextDisplayMember = "Id";
			scanNumberComboBox.ValueMember = "Id";
			scanNumberComboBox.SelectedIndexChanged += new EventHandler( scanNumberComboBox_SelectedIndexChanged );
			scanNumberComboBox.ListHeaderText = scanNumberHeaderString;
			scanNumberComboBox.ListBox.ContextMenuStrip = scanNumberComboBoxContextMenuStrip;
			scanNumberComboBoxContextMenuStrip.Opened += new EventHandler( scanNumberComboBoxContextMenuStrip_Opened );
			scanNumberComboBox.Dock = DockStyle.Fill;
			scanNumberComboBoxToolStripHost = new ToolStripControlHost( scanNumberComboBox );
			scanNumberComboBoxToolStripHost.Alignment = ToolStripItemAlignment.Right;
			scanNumberComboBoxToolStripHost.AutoSize = false;

			if( SeemsMdiParent != null )
				SeemsMdiParent.ToolStrip1.Items.Insert( SeemsMdiParent.ToolStrip1.Items.IndexOf( SeemsMdiParent.ToolStripScanLabel ), scanNumberComboBoxToolStripHost );

			zedGraphControl1.GraphPane.Title.IsVisible = false;
			zedGraphControl1.GraphPane.Chart.Border.IsVisible = false;
			zedGraphControl1.GraphPane.Y2Axis.IsVisible = false;
			zedGraphControl1.GraphPane.X2Axis.IsVisible = false;
			zedGraphControl1.GraphPane.XAxis.MajorTic.IsOpposite = false;
			zedGraphControl1.GraphPane.YAxis.MajorTic.IsOpposite = false;
			zedGraphControl1.GraphPane.XAxis.MinorTic.IsOpposite = false;
			zedGraphControl1.GraphPane.YAxis.MinorTic.IsOpposite = false;
			zedGraphControl1.GraphPane.IsFontsScaled = false;
			zedGraphControl1.IsZoomOnMouseCenter = false;
			zedGraphControl1.GraphPane.YAxis.Scale.MaxGrace = 0.1;
			zedGraphControl1.IsEnableVZoom = false;
			zedGraphControl1.PanButtons = MouseButtons.Left;
			zedGraphControl1.PanModifierKeys = Keys.Control;
			zedGraphControl1.PanButtons2 = MouseButtons.None;
		}

		int overlaySelectedIndex;
		void scanNumberComboBoxContextMenuStrip_Opened( object sender, EventArgs e )
		{
			overlaySelectedIndex = ScanNumberComboBox.ListBox.SelectedIndex;
		}

		private void setDataSource( DataSource dataSource )
		{
			this.dataSource = dataSource;
			if( dataSource != null )
			{
				scanNumberComboBox.BeginUpdate();
				foreach( SeemsScan scan in dataSource.CurrentScanHeaders )
					scanNumberComboBox.Items.Add( scan );
				scanNumberComboBox.EndUpdate();
			}
		}

		public static GraphForm CreateNewWindow( Form mdiParent, DataSource dataSource, bool giveFocus )
		{
			// create a new window with this data source
			GraphForm graphForm = new GraphForm( dataSource );
			graphForm.MdiParent = mdiParent;
			graphForm.ShowData( 0 ); // show chromatogram
			graphForm.Show();
			if( giveFocus )
				graphForm.Activate();
			return graphForm;
		}

		private void scanNumberComboBox_SelectedIndexChanged( object sender, EventArgs e )
		{
			if( SeemsMdiParent != null )
				SeemsMdiParent.setScanControls( true );
			updateGraph();
			scanNumberComboBox.TextBox.Refresh();
			Application.DoEvents();
			zedGraphControl1.Focus();
		}

		public void ShowData( int dataIndex )
		{
			scanNumberComboBox.SelectedIndex = dataIndex;
		}

		public void OverlayData( int dataIndex )
		{
			showData( dataIndex, true );
		}

		private Color[] overlayColors = new Color[]
		{
			Color.Red, Color.Blue, Color.Green, Color.Purple, Color.Brown,
			Color.Magenta, Color.Cyan, Color.LightGreen, Color.Beige,
			Color.DarkRed, Color.DarkBlue, Color.DarkGreen, Color.DeepPink
		};

		private SeemsScan shownScan;
		private void showData( int dataIndex, bool isOverlay )
		{
			SeemsScan scan = dataSource.CurrentScanHeaders[dataIndex];
			ZedGraph.GraphPane pane = zedGraphControl1.GraphPane;

			if( isOverlay && pane.CurveList.Count > overlayColors.Length )
				MessageBox.Show( "SeeMS only supports up to " + overlayColors.Length + " simultaneous overlays.", "Too many overlays", MessageBoxButtons.OK, MessageBoxIcon.Stop );

			// set form title
			if( !isOverlay )
				Text = String.Format( "{0} - {1}", System.IO.Path.GetFileName( dataSource.CurrentFilepath ), scan.Id );
			else
				Text += "," + scan.Id;

			if( !isOverlay )
				pane.CurveList.Clear();

			if( shownScan != null && scan.IsMassSpectrum != shownScan.IsMassSpectrum )
			{
				zedGraphControl1.RestoreScale( pane );
				zedGraphControl1.ZoomOutAll( pane );
			}
			bool isScaleAuto = !pane.IsZoomed;

			//pane.GraphObjList.Clear();

			SeemsPointList pointList = scan.PointList;
			if( pointList.FullCount == 0 )
			{
				// the header does not have the data points, assume it is a mass spectrum
				pane.YAxis.Title.Text = "Intensity";
				pane.XAxis.Title.Text = "m/z";

				bool doCentroid = SeemsMdiParent.CentroidMenuItem.Enabled && SeemsMdiParent.CentroidMenuItem.Checked;
				dataSource.InstrumentInterface.setCentroiding( doCentroid, doCentroid, SeemsMdiParent.UseVendorCentroidMenuItem.Checked );

				SeemsScan scanWithData = new SeemsScan( dataSource.InstrumentInterface.getScan( scan.Scan.ScanNumber ) );
				pointList = scanWithData.PointList;
				int bins = (int) pane.CalcChartRect( zedGraphControl1.CreateGraphics() ).Width;
				if( isScaleAuto )
					pointList.SetScale( bins, pointList[0].X, pointList[pointList.Count - 1].X );
				else
					pointList.SetScale( bins, pane.XAxis.Scale.Min, pane.XAxis.Scale.Max );

				if( doCentroid || scanWithData.Scan.IsCentroided )
				{
					ZedGraph.StickItem stick = pane.AddStick( scan.Id, pointList, Color.Gray );
					stick.Symbol.IsVisible = false;
					stick.Line.Width = 1;
				} else
					pane.AddCurve( scan.Id, pointList, Color.Gray, ZedGraph.SymbolType.None );
			} else
			{
				// the header has the data points, assume it is a chromatogram
				int bins = (int) pane.CalcChartRect( zedGraphControl1.CreateGraphics() ).Width;
				if( isScaleAuto )
					pointList.SetScale( bins, pointList[0].X, pointList[pointList.Count - 1].X );
				else
					pointList.SetScale( bins, pane.XAxis.Scale.Min, pane.XAxis.Scale.Max );
				pane.YAxis.Title.Text = "Total Intensity";
				pane.XAxis.Title.Text = "Retention Time (in seconds)";
				pane.AddCurve( scan.Id, pointList, Color.Gray, ZedGraph.SymbolType.None );
			}
			pane.AxisChange();

			if( isOverlay )
			{
				pane.Legend.IsVisible = true;
				pane.Legend.Position = ZedGraph.LegendPos.TopCenter;
				for( int i = 0; i < pane.CurveList.Count; ++i )
					pane.CurveList[i].Color = overlayColors[i];
			} else
				pane.Legend.IsVisible = false;

			SetDataLabelsVisible( true );

			shownScan = scan;
			zedGraphControl1.Refresh();
			//zedGraphControl1.Focus();
		}

		// create a map of point X coordinates to point indexes in descending order by the Y coordinate
		// for each point, create a text label of the X and/or Y value (user configurable)
		// check that the label will not overlap any other labels (how to check against data lines as well?)
		// if no overlap, add the label and mask the 
		private ZedGraph.GraphObjList dataLabels = new ZedGraph.GraphObjList();
		public void SetDataLabelsVisible( bool visible )
		{
			//zedGraphControl1.GraphPane.GraphObjList.Clear();
			foreach( ZedGraph.GraphObj dataLabel in dataLabels )
				zedGraphControl1.GraphPane.GraphObjList.Remove( dataLabel );

			if( visible )
			{
				if( !( AnnotationSettings.ShowChromatogramPointTimes || AnnotationSettings.ShowScanPointMZs ||
					AnnotationSettings.ShowChromatogramPointIntensities || AnnotationSettings.ShowScanPointIntensities ||
					AnnotationSettings.ShowUnmatchedAnnotations || AnnotationSettings.ShowMatchedAnnotations ) )
					return;

				// set direct references
				ZedGraph.GraphPane pane = zedGraphControl1.GraphPane;
				ZedGraph.Axis xAxis = pane.XAxis;
				ZedGraph.Axis yAxis = pane.YAxis;
				yAxis.Scale.MinAuto = false;
				yAxis.Scale.Min = 0;

				// setup axes scales to enable the Transform method
				xAxis.Scale.SetupScaleData( pane, xAxis );
				yAxis.Scale.SetupScaleData( pane, yAxis );

				Graphics g = zedGraphControl1.CreateGraphics();
				//pane.Draw( g );
				pane.CurveList.Draw( g, pane, 1.0f );
				System.Drawing.Bitmap gmap = new Bitmap( Convert.ToInt32( pane.Rect.Width ), Convert.ToInt32( pane.Rect.Height ) );
				zedGraphControl1.DrawToBitmap( gmap, Rectangle.Round( pane.Rect ) );
				Region chartRegion = new Region( pane.Chart.Rect );
				Region clipRegion = new Region();
				clipRegion.MakeEmpty();
				g.SetClip( zedGraphControl1.MasterPane.Rect, CombineMode.Replace );
				g.SetClip( chartRegion, CombineMode.Exclude );
				/*Bitmap clipBmp = new Bitmap( Convert.ToInt32( pane.Rect.Width ), Convert.ToInt32( pane.Rect.Height ) );
				Graphics clipG = Graphics.FromImage( clipBmp );
				clipG.Clear( Color.White );
				clipG.FillRegion( new SolidBrush( Color.Black ), g.Clip );
				clipBmp.Save( "C:\\clip.bmp" );*/

				PointDataMap<SeemsPointAnnotation> matchedAnnotations = new PointDataMap<SeemsPointAnnotation>();

				// add automatic labels
				foreach( ZedGraph.CurveItem curve in pane.CurveList )
				{
					SeemsPointList pointList = (SeemsPointList) curve.Points;
					for( int i = 0; i < pointList.MaxCount; i++ )
					{
						ZedGraph.PointPair pt = pointList.GetPointAtIndex( pointList.ScaledMaxIndexList[i] );
						if( pt.X < xAxis.Scale.Min || pt.Y > yAxis.Scale.Max || pt.Y < yAxis.Scale.Min )
							continue;
						if( pt.X > xAxis.Scale.Max )
							break;

						StringBuilder pointLabel = new StringBuilder();
						Color pointColor = curve.Color;

						// Add annotation
						double annotationX = 0.0;
						SeemsPointAnnotation annotation = null;
						if( CurrentScan.IsMassSpectrum )
						{
							PointDataMap<SeemsPointAnnotation>.MapPair annotationPair = AnnotationSettings.ScanPointAnnotations.FindNear( pt.X, AnnotationSettings.ScanMatchTolerance );
							if( annotationPair != null )
							{
								annotationX = annotationPair.Key;
								annotation = annotationPair.Value;
								matchedAnnotations.Add( annotationPair );
							}
						} else
						{
							PointDataMap<SeemsPointAnnotation>.MapPair annotationPair = AnnotationSettings.ChromatogramPointAnnotations.FindNear( pt.X, AnnotationSettings.ChromatogramMatchTolerance );
							if( annotationPair != null )
							{
								annotationX = annotationPair.Key;
								annotation = annotationPair.Value;
								matchedAnnotations.Add( annotationPair );
							}
						}

						if( AnnotationSettings.ShowMatchedAnnotations && annotation != null )
						{
							pointLabel.AppendLine( annotation.Label );
							pointColor = annotation.Color;
							//if( curve is ZedGraph.StickItem )
							{
								ZedGraph.LineObj stickOverlay = new ZedGraph.LineObj( annotationX, 0, annotationX, pt.Y );
								//stickOverlay.IsClippedToChartRect = true;
								stickOverlay.Location.CoordinateFrame = ZedGraph.CoordType.AxisXYScale;
								stickOverlay.Line.Width = annotation.Width;
								stickOverlay.Line.Color = pointColor;
								zedGraphControl1.GraphPane.GraphObjList.Add( stickOverlay );
								dataLabels.Add(stickOverlay);
								//( (ZedGraph.StickItem) curve ).Color = pointColor;
								//( (ZedGraph.StickItem) curve ).Line.Width = annotation.Width;
							}
						}

						if( CurrentScan.IsMassSpectrum )
						{
							if( AnnotationSettings.ShowScanPointMZs )
								pointLabel.AppendLine( pt.X.ToString( "f2" ) );
							if( AnnotationSettings.ShowScanPointIntensities )
								pointLabel.AppendLine( pt.Y.ToString( "f2" ) );
						} else
						{
							if( AnnotationSettings.ShowChromatogramPointTimes )
								pointLabel.AppendLine( pt.X.ToString( "f2" ) );
							if( AnnotationSettings.ShowChromatogramPointIntensities )
								pointLabel.AppendLine( pt.Y.ToString( "f2" ) );
						}

						string pointLabelString = pointLabel.ToString();
						if( pointLabelString.Length == 0 )
							continue;

						// Create a text label from the X data value
						ZedGraph.TextObj text = new ZedGraph.TextObj( pointLabelString, pt.X, yAxis.Scale.ReverseTransform( yAxis.Scale.Transform( pt.Y ) - 5 ),
							ZedGraph.CoordType.AxisXYScale, ZedGraph.AlignH.Center, ZedGraph.AlignV.Bottom );
						text.ZOrder = ZedGraph.ZOrder.A_InFront;
						//text.IsClippedToChartRect = true;
						text.FontSpec.FontColor = pointColor;
						// Hide the border and the fill
						text.FontSpec.Border.IsVisible = false;
						text.FontSpec.Fill.IsVisible = false;
						//text.FontSpec.Fill = new Fill( Color.FromArgb( 100, Color.White ) );
						// Rotate the text to 90 degrees
						text.FontSpec.Angle = 0;

						string shape, coords;
						text.GetCoords( pane, g, 1.0f, out shape, out coords );
						if( shape != "poly" ) throw new InvalidOperationException( "shape must be 'poly'" );
						string[] textBoundsPointStrings = coords.Split( ",".ToCharArray() );
						if( textBoundsPointStrings.Length != 9 ) throw new InvalidOperationException( "coords length must be 8" );
						Point[] textBoundsPoints = new Point[]
						{
							new Point( Convert.ToInt32(textBoundsPointStrings[0]), Convert.ToInt32(textBoundsPointStrings[1])),
							new Point( Convert.ToInt32(textBoundsPointStrings[2]), Convert.ToInt32(textBoundsPointStrings[3])),
							new Point( Convert.ToInt32(textBoundsPointStrings[4]), Convert.ToInt32(textBoundsPointStrings[5])),
							new Point( Convert.ToInt32(textBoundsPointStrings[6]), Convert.ToInt32(textBoundsPointStrings[7]))
						};
						byte[] textBoundsPointTypes = new byte[]
						{
							(byte) PathPointType.Start,
							(byte) PathPointType.Line,
							(byte) PathPointType.Line,
							(byte) PathPointType.Line
						};
						GraphicsPath textBoundsPath = new GraphicsPath( textBoundsPoints, textBoundsPointTypes );
						textBoundsPath.CloseFigure();
						Region textBoundsRegion = new Region( textBoundsPath );
						textBoundsRegion.Intersect( pane.Chart.Rect );
						RectangleF[] textBoundsRectangles = textBoundsRegion.GetRegionScans( g.Transform );

						bool overlapDetected = false;
						for( int j = 0; j < textBoundsRectangles.Length && !overlapDetected; ++j )
						{
							if( g.Clip.IsVisible( textBoundsRectangles[j] ) )
								overlapDetected = true;

							for( float y = textBoundsRectangles[j].Top; y <= textBoundsRectangles[j].Bottom && !overlapDetected; ++y )
								for( float x = textBoundsRectangles[j].Left; x <= textBoundsRectangles[j].Right && !overlapDetected; ++x )
									if( gmap.GetPixel( Convert.ToInt32( x ), Convert.ToInt32( y ) ).ToArgb() != pane.Chart.Fill.Color.ToArgb() )
										overlapDetected = true;
						}
						if( !overlapDetected )
						{
							pane.GraphObjList.Add( text );
							clipRegion.Union( textBoundsRegion );
							//g.SetClip( chartRegion, CombineMode.Replace );
							g.SetClip( clipRegion, CombineMode.Replace );
							//clipG.Clear( Color.White );
							//clipG.FillRegion( new SolidBrush( Color.Black ), g.Clip );
							//clipBmp.Save( "C:\\clip.bmp" );
							dataLabels.Add( text );
							//text.Draw( g, pane, 1.0f );
							//zedGraphControl1.DrawToBitmap( gmap, Rectangle.Round( pane.Rect ) );
						}
					}
				}

				if( AnnotationSettings.ShowUnmatchedAnnotations )
				{
					PointDataMap<SeemsPointAnnotation> annotations;
					if( CurrentScan.IsMassSpectrum )
						annotations = AnnotationSettings.ScanPointAnnotations;
					else
						annotations = AnnotationSettings.ChromatogramPointAnnotations;

					foreach( PointDataMap<SeemsPointAnnotation>.MapPair annotationPair in annotations )
					{
						if( matchedAnnotations.Contains( annotationPair ) )
							continue;

						float stickLength = ( yAxis.MajorTic.Size * 2 ) / pane.Chart.Rect.Height;
						ZedGraph.LineObj stickOverlay = new ZedGraph.LineObj( annotationPair.Key, 1, annotationPair.Key, -stickLength );
						//stickOverlay.IsClippedToChartRect = true;
						stickOverlay.Location.CoordinateFrame = ZedGraph.CoordType.XScaleYChartFraction;
						stickOverlay.Line.Width = annotationPair.Value.Width;
						stickOverlay.Line.Color = annotationPair.Value.Color;
						zedGraphControl1.GraphPane.GraphObjList.Add( stickOverlay );
						dataLabels.Add( stickOverlay );
					}
				}
				g.Dispose();
			} else
			{
				// remove labels
			}
		}

		public void updateGraph()
		{
			long start = DateTime.Now.Ticks;
			showData( CurrentScanIndex, false );
			//SeemsMdiParent.StatusLabel.Text = "Scan graph generated in " + new TimeSpan( DateTime.Now.Ticks - start ).TotalSeconds + " seconds.";
		}

		private void GraphForm_PreviewKeyDown( object sender, PreviewKeyDownEventArgs e )
		{
			int key = (int) e.KeyCode;
			if( key == (int) Keys.Left && scanNumberComboBox.SelectedIndex > 0 )
			{
				scanNumberComboBox.SelectedIndex = scanNumberComboBox.SelectedIndex - 1;
				Application.DoEvents();
			} else if( key == (int) Keys.Right && scanNumberComboBox.SelectedIndex < scanNumberComboBox.Items.Count - 1 )
			{
				scanNumberComboBox.SelectedIndex = scanNumberComboBox.SelectedIndex + 1;
				Application.DoEvents();
			}
		}


		private void openInActiveWindowToolStripMenuItem_Click( object sender, EventArgs e )
		{
			scanNumberComboBox.SelectedIndex = scanNumberComboBox.ListBox.SelectedIndex;
			scanNumberComboBox.Button.PerformClick();
		}

		private void openInNewWindowToolStripMenuItem_Click( object sender, EventArgs e )
		{
			GraphForm oldForm = SeemsMdiParent.CurrentGraphForm;
			int oldSelectedIndex = oldForm.scanNumberComboBox.SelectedIndex;
			GraphForm graphForm = CreateNewWindow( SeemsMdiParent, dataSource, false );
			graphForm.ShowData( scanNumberComboBox.ListBox.SelectedIndex );
			oldForm.scanNumberComboBox.SelectedIndex = oldSelectedIndex;
			oldForm.Activate();
			oldForm.scanNumberComboBox.Button.PerformClick();
		}

		private void overlayOnActiveWindowToolStripMenuItem_Click( object sender, EventArgs e )
		{
			OverlayData( overlaySelectedIndex );
			//scanNumberComboBox.SelectedIndex = scanNumberComboBox.ListBox.SelectedIndex;
			scanNumberComboBox.Button.PerformClick();
		}

		private void zedGraphControl1_ZoomEvent( ZedGraph.ZedGraphControl sender, ZedGraph.ZoomState oldState, ZedGraph.ZoomState newState )
		{
			ZedGraph.GraphPane pane = zedGraphControl1.GraphPane;
			int bins = (int) pane.CalcChartRect( zedGraphControl1.CreateGraphics() ).Width;
			foreach( ZedGraph.CurveItem curve in pane.CurveList )
				( curve.Points as SeemsPointList ).SetScale( bins, pane.XAxis.Scale.Min, pane.XAxis.Scale.Max );
			pane.AxisChange();
			SetDataLabelsVisible( true );
			Refresh();
		}

		private void zedGraphControl1_Resize( object sender, EventArgs e )
		{
			ZedGraph.GraphPane pane = zedGraphControl1.GraphPane;
			int bins = (int) pane.CalcChartRect( zedGraphControl1.CreateGraphics() ).Width;
			foreach( ZedGraph.CurveItem curve in pane.CurveList )
				( curve.Points as SeemsPointList ).SetScale( bins, pane.XAxis.Scale.Min, pane.XAxis.Scale.Max );
			SetDataLabelsVisible( true );
		}

		private void GraphForm_MouseClick( object sender, MouseEventArgs e )
		{
			if( e.Button == MouseButtons.Middle )
				zedGraphControl1.ZoomOut( zedGraphControl1.GraphPane );
		}

		private void GraphForm_ResizeBegin( object sender, EventArgs e )
		{
			SuspendLayout();
			zedGraphControl1.Visible = false;
		}

		private void GraphForm_ResizeEnd( object sender, EventArgs e )
		{
			ResumeLayout();
			zedGraphControl1.Visible = true;
			Refresh();
		}
	}
}