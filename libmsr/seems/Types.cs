using System;
using System.Collections.Generic;
using System.Text;
using System.Drawing;
using ManagedLibMSR;
using Extensions;

public class Pair<T1, T2>
{
	public Pair()
	{
		this.first = default(T1);
		this.second = default(T2);
	}

	public Pair( T1 first, T2 second )
	{
		this.first = first;
		this.second = second;
	}

	public T1 first;
	public T2 second;
}

public class RefPair<T1, T2>
	where T1 : new()
	where T2 : new()
{
	public RefPair()
	{
		this.first = new T1();
		this.second = new T2();
	}

	public RefPair( T1 first, T2 second )
	{
		this.first = first;
		this.second = second;
	}

	public T1 first;
	public T2 second;
}

namespace seems
{
	public class PointDataMap<T> : Map< double, T >
		where T: new()
	{
		public PointDataMap()
		{
		}

		public MapPair FindNear( double x, double tolerance )
		{
			Enumerator cur, min, max;

			min = LowerBound( x - tolerance );
			max = LowerBound( x + tolerance );

			if( !min.IsValid || ( max.IsValid && min.Current == max.Current ) )
				return null; // no peaks

			MutableKeyValuePair<double, T> best = min.Current;

			// find the peak closest to the desired mz
			double minDiff = Math.Abs( x - best.Key );
			cur = min;
			while( true )
			{
				double curDiff = Math.Abs( x - cur.Current.Key );
				if( curDiff < minDiff )
				{
					minDiff = curDiff;
					best = cur.Current;
				}
				cur.MoveNext();
				if( ( !max.IsValid && !cur.IsValid ) || cur.Current == max.Current )
					break;
			}

			return new MapPair( best.Key, best.Value );
		}
	}

	public class SeemsScan
	{
		// constructor for a full scan
		public SeemsScan( ManagedScan scan )
		{
			this.scan = scan;
			id = scan.ScanNumber.ToString();
			pointList = new SeemsPointList(scan);
		}

		// constructor for an SRM/MRM scan
		public SeemsScan( ManagedScan chromatogram, string id )
		{
			this.scan = chromatogram;
			this.id = id;
			pointList = new SeemsPointList(chromatogram);
		}

		private ManagedScan scan;
		private string id;

		public ManagedScan Scan { get { return scan; } }
		public string Id { get { return id; } }
		public bool IsMassSpectrum { get { return scan.ScanNumber > 0; } }
		public bool IsChromatogram { get { return scan.ScanNumber == 0; } }

		private SeemsPointList pointList;
		public SeemsPointList PointList { get { return pointList; } }
		public double[] MzList
		{
			get
			{
				double[] mzList = new double[scan.getTotalPeakCount()];
				for( int i = 0; i < scan.getTotalPeakCount(); ++i )
					mzList[i] = scan.getPeakMz( i );
				return mzList;
			}
		}

		public double[] IntensityList
		{
			get
			{
				double[] intensityList = new double[scan.getTotalPeakCount()];
				for( int i = 0; i < scan.getTotalPeakCount(); ++i )
					intensityList[i] = scan.getPeakIntensity( i );
				return intensityList;
			}
		}

		public string ComboBoxView
		{
			get
			{
				if( scan.ScanNumber > 0 )
				{
					string precursorMz = "n/a";
					if( scan.getPrecursorScanCount() > 0 )
						precursorMz = Convert.ToString( Math.Round( scan.getPrecursorScanInfo( 0 ).Mz, 3 ) );
					return String.Format( "{0,-8}{1,-10}{2,-5}{3,-15:E}{4,15}",
							id, Math.Round( scan.RetentionTime, 3 ), scan.MsLevel, scan.TotalIntensity, precursorMz );
				} else
					return String.Format( "{0,-23}{1,-15:E}{2,15}", id, scan.TotalIntensity, "n/a" );
			}
		}
	}

	public class SeemsPointList : ZedGraph.IPointList
	{
		private ZedGraph.PointPairList fullPointList;
		private ZedGraph.PointPairList scaledPointList;
		private List<int> scaledMaxIndexList;
		private int scaledWidth;
		private double scaledMin;
		private double scaledMax;
		private double scaleRange;
		private double scaleFactor;
		private int scaledMinIndex;
		private int scaledMaxIndex;

		public SeemsPointList( ManagedScan scan )
		{
			fullPointList = new ZedGraph.PointPairList();
			fullPointList.Capacity = scan.getTotalPeakCount();
			Map<double, double> sortedFullPointList = new Map<double, double>();
			for( int i = 0; i < scan.getTotalPeakCount(); ++i )
				sortedFullPointList[scan.getPeakMz( i )] = scan.getPeakIntensity( i );
			foreach( Map<double, double>.MapPair itr in sortedFullPointList )
				fullPointList.Add( new ZedGraph.PointPair( itr.Key, itr.Value ) );
			scaledPointList = new ZedGraph.PointPairList();
			scaledMaxIndexList = new List<int>();
		}

		public void SetScale( int bins, double min, double max )
		{
			if( scaledWidth == bins && scaledMin == min && scaledMax == max )
				return;

			scaledWidth = bins;
			scaledMin = min;
			scaledMax = max;
			scaleRange = max - min;
			scaleFactor = bins / scaleRange;
			scaledPointList.Clear();
			scaledPointList.Capacity = bins * 4; // store 4 points for each bin (entry, min, max, exit)
			scaledMaxIndexList.Clear();
			scaledMaxIndexList.Capacity = bins; // store just the index of the max point for each bin
			int lastBin = -1;
			int curBinEntryIndex = -1;
			int curBinMinIndex = -1;
			int curBinMaxIndex = -1;
			int curBinExitIndex = -1;
			for( int i = 0; i < fullPointList.Count; ++i )
			{
				ZedGraph.PointPair point = fullPointList[i];

				if( point.X < min )
					continue;

				if( point.X > max )
					break;

				int curBin = (int) Math.Round( scaleFactor * ( point.X - min ) );
				if( curBin > lastBin ) // new bin, insert points of last bin
				{
					if( lastBin > -1 )
					{
						scaledMinIndex = curBinMinIndex;
						scaledPointList.Add( fullPointList[curBinEntryIndex] );
						if( curBinEntryIndex != curBinMinIndex )
							scaledPointList.Add( fullPointList[curBinMinIndex] );
						if( curBinEntryIndex != curBinMaxIndex &&
							curBinMinIndex != curBinMaxIndex )
							scaledPointList.Add( fullPointList[curBinMaxIndex] );
						if( curBinEntryIndex != curBinMaxIndex &&
							curBinMinIndex != curBinMaxIndex &&
							curBinMaxIndex != curBinExitIndex )
							scaledPointList.Add( fullPointList[curBinExitIndex] );
						if( fullPointList[curBinMaxIndex].Y != 0 )
							scaledMaxIndexList.Add( curBinMaxIndex );
					}
					lastBin = curBin;
					curBinEntryIndex = i;
					curBinMinIndex = i;
					curBinMaxIndex = i;
				} else // same bin, set exit point
				{
					curBinExitIndex = i;
					if( point.Y > fullPointList[curBinMaxIndex].Y )
						scaledMaxIndex = curBinMaxIndex = i;
					else if( point.Y < fullPointList[curBinMaxIndex].Y )
						curBinMinIndex = i;
				}
			}

			if( lastBin > -1 )
			{
				scaledMinIndex = curBinMinIndex;
				scaledPointList.Add( fullPointList[curBinEntryIndex] );
				if( curBinEntryIndex != curBinMinIndex )
					scaledPointList.Add( fullPointList[curBinMinIndex] );
				if( curBinEntryIndex != curBinMaxIndex &&
					curBinMinIndex != curBinMaxIndex )
					scaledPointList.Add( fullPointList[curBinMaxIndex] );
				if( curBinEntryIndex != curBinMaxIndex &&
					curBinMinIndex != curBinMaxIndex &&
					curBinMaxIndex != curBinExitIndex )
					scaledPointList.Add( fullPointList[curBinExitIndex] );
				if( fullPointList[curBinMaxIndex].Y != 0 )
					scaledMaxIndexList.Add( curBinMaxIndex );
			}
		}

		public int Count
		{
			get
			{
				if( ScaledCount > 0 )
					return ScaledCount;
				else
					return FullCount;
			}
		}

		public int FullCount
		{
			get { return fullPointList.Count; }
		}

		public int ScaledCount
		{
			get { return scaledPointList.Count; }
		}

		public int MaxCount
		{
			get { return scaledMaxIndexList.Count; }
		}

		public List<int> ScaledMaxIndexList
		{
			get { return scaledMaxIndexList; }
		}

		public ZedGraph.PointPair GetPointAtIndex( int index )
		{
			return fullPointList[index];
		}

		public ZedGraph.PointPair this[int index]
		{
			get
			{
				if( ScaledCount > 0 )
					return scaledPointList[index];
				else
					return fullPointList[index];
			}
		}

		public object Clone()
		{
			throw new Exception( "The method or operation is not implemented." );
		}
	}

	public class SeemsPointAnnotation
	{
		public SeemsPointAnnotation()
		{
			point = 0;
			label = String.Empty;
			color = Color.Gray;
			width = 1;
		}

		public SeemsPointAnnotation( double point, string label, Color color, int width )
		{
			this.point = point;
			this.label = label;
			this.color = color;
			this.width = width;
		}

		private double point;
		public double Point
		{
			get { return point; }
			set { point = value; }
		}

		private string label;
		public string Label
		{
			get { return label; }
			set { label = value; }
		}

		private Color color;
		public Color Color
		{
			get { return color; }
			set { color = value; }
		}

		private int width;
		public int Width
		{
			get { return width; }
			set { width = value; }
		}
	}

	public class SeemsAnnotationSettings
	{
		public enum ChromatogramMatchToleranceUnits
		{
			Seconds = 0,
			Minutes,
			Hours
		}

		public enum ScanMatchToleranceUnits
		{
			Daltons = 0,
			PPM,
			ResolvingPower
		}

		public SeemsAnnotationSettings()
		{
			// ms-product-label -> (label alias, known color)
			labelToAliasAndColorMap = new Map<string, Pair<string, Color>>();
			labelToAliasAndColorMap["y"] = new Pair<string, Color>( "y", Color.Blue );
			labelToAliasAndColorMap["b"] = new Pair<string, Color>( "b", Color.Red );
			labelToAliasAndColorMap["y-NH3"] = new Pair<string, Color>( "y^", Color.Green );
			labelToAliasAndColorMap["y-H2O"] = new Pair<string, Color>( "y*", Color.Cyan );
			labelToAliasAndColorMap["b-NH3"] = new Pair<string, Color>( "b^", Color.Orange );
			labelToAliasAndColorMap["b-H2O"] = new Pair<string, Color>( "b*", Color.Violet );

			scanPointAnnotations = new PointDataMap<SeemsPointAnnotation>();
			chromatogramPointAnnotations = new PointDataMap<SeemsPointAnnotation>();
		}

		public void setScanLabels( Map<string, List<Pair<double, int>>> ionLabelToMzAndChargeMap )
		{
			foreach( Map<string, List<Pair<double, int>>>.MapPair ionListItr in ionLabelToMzAndChargeMap )
			{
				string[] labelNameIndexPair = ionListItr.Key.Split( " ".ToCharArray() );
				foreach( Pair<double, int> ionMzChargePair in ionListItr.Value )
				{
					SeemsPointAnnotation annotation = scanPointAnnotations[ionMzChargePair.first];
					if( labelToAliasAndColorMap.Contains( labelNameIndexPair[0] ) )
					{
						Pair<string, Color> labelColorPair = labelToAliasAndColorMap[labelNameIndexPair[0]];
						if( ionMzChargePair.second > 1 )
							annotation.Label = String.Format( "{0}{1} (+{2})", labelColorPair.first, labelNameIndexPair[1], ionMzChargePair.second );
						else
							annotation.Label = String.Format( "{0}{1}", labelColorPair.first, labelNameIndexPair[1] );

						annotation.Color = labelColorPair.second;
					} else
					{
						annotation.Label = ionListItr.Key;
						annotation.Color = Color.Blue;
					}
					annotation.Point = ionMzChargePair.first;
					annotation.Width = 2;
				}
			}
		}

		private bool showScanPointMZs;
		public bool ShowScanPointMZs
		{
			get { return showScanPointMZs; }
			set { showScanPointMZs = value; }
		}

		private bool showScanPointIntensities;
		public bool ShowScanPointIntensities
		{
			get { return showScanPointIntensities; }
			set { showScanPointIntensities = value; }
		}

		private bool showChromatogramPointTimes;
		public bool ShowChromatogramPointTimes
		{
			get { return showChromatogramPointTimes; }
			set { showChromatogramPointTimes = value; }
		}

		private bool showChromatogramPointIntensities;
		public bool ShowChromatogramPointIntensities
		{
			get { return showChromatogramPointIntensities; }
			set { showChromatogramPointIntensities = value; }
		}

		private bool showMatchedAnnotations;
		public bool ShowMatchedAnnotations
		{
			get { return showMatchedAnnotations; }
			set { showMatchedAnnotations = value; }
		}

		private bool showUnmatchedAnnotations;
		public bool ShowUnmatchedAnnotations
		{
			get { return showUnmatchedAnnotations; }
			set { showUnmatchedAnnotations = value; }
		}

		private bool chromatogramMatchToleranceOverride;
		public bool ChromatogramMatchToleranceOverride
		{
			get { return chromatogramMatchToleranceOverride; }
			set { chromatogramMatchToleranceOverride = value; }
		}

		private double chromatogramMatchTolerance;
		public double ChromatogramMatchTolerance
		{
			get { return chromatogramMatchTolerance; }
			set { chromatogramMatchTolerance = value; }
		}

		private ChromatogramMatchToleranceUnits chromatogramMatchToleranceUnit;
		public ChromatogramMatchToleranceUnits ChromatogramMatchToleranceUnit
		{
			get { return chromatogramMatchToleranceUnit; }
			set { chromatogramMatchToleranceUnit = value; }
		}

		private bool scanMatchToleranceOverride;
		public bool ScanMatchToleranceOverride
		{
			get { return scanMatchToleranceOverride; }
			set { scanMatchToleranceOverride = value; }
		}

		private double scanMatchTolerance;
		public double ScanMatchTolerance
		{
			get { return scanMatchTolerance; }
			set { scanMatchTolerance = value; }
		}

		private ScanMatchToleranceUnits scanMatchToleranceUnit;
		public ScanMatchToleranceUnits ScanMatchToleranceUnit
		{
			get { return scanMatchToleranceUnit; }
			set { scanMatchToleranceUnit = value; }
		}

		private Map<string, Pair<string, Color>> labelToAliasAndColorMap;
		public Map<string, Pair<string, Color>> LabelToAliasAndColorMap
		{
			get { return labelToAliasAndColorMap; }
		}

		private PointDataMap<SeemsPointAnnotation> scanPointAnnotations;
		public PointDataMap<SeemsPointAnnotation> ScanPointAnnotations
		{
			get { return scanPointAnnotations; }
		}

		private PointDataMap<SeemsPointAnnotation> chromatogramPointAnnotations;
		public PointDataMap<SeemsPointAnnotation> ChromatogramPointAnnotations
		{
			get { return chromatogramPointAnnotations; }
		}
	}
}
