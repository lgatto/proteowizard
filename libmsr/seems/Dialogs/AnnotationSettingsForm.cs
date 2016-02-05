using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using Extensions;

namespace seems
{
	using LabelToAliasAndColorPair = Map<string, Pair<string, Color>>.MapPair;

	public partial class AnnotationSettingsForm : Form
	{
		private List<string> chromatogramMatchToleranceUnitList = new List<string>();
		private List<string> scanMatchToleranceUnitList = new List<string>();
		private GraphForm ownerGraphForm;

		public AnnotationSettingsForm( GraphForm graphForm )
		{
			InitializeComponent();

			chromatogramMatchToleranceUnitList.Add( "sec." );
			//chromatogramMatchToleranceUnitList.Add( "min." );
			//chromatogramMatchToleranceUnitList.Add( "hours" );

			scanMatchToleranceUnitList.Add( "Da/z" );
			//scanMatchToleranceUnitList.Add( "ppm" );
			//scanMatchToleranceUnitList.Add( "resolving power" );

			ownerGraphForm = graphForm;

			if( ownerGraphForm.CurrentScan.IsMassSpectrum )
			{
				matchToleranceUnitsComboBox.Items.AddRange( (object[]) scanMatchToleranceUnitList.ToArray() );
				matchToleranceUnitsComboBox.SelectedIndex = (int) ownerGraphForm.AnnotationSettings.ScanMatchToleranceUnit;
				matchToleranceCheckbox.Checked = ownerGraphForm.AnnotationSettings.ScanMatchToleranceOverride;
				matchToleranceTextBox.Text = ownerGraphForm.AnnotationSettings.ScanMatchTolerance.ToString();
				showIndependentValueLabelCheckbox.Text = "Show m/z label";
				showDependentValueLabelCheckbox.Text = "Show intensity label";
				showIndependentValueLabelCheckbox.Checked = ownerGraphForm.AnnotationSettings.ShowScanPointMZs;
				showDependentValueLabelCheckbox.Checked = ownerGraphForm.AnnotationSettings.ShowScanPointIntensities;
			} else
			{
				matchToleranceUnitsComboBox.Items.AddRange( (object[]) chromatogramMatchToleranceUnitList.ToArray() );
				matchToleranceUnitsComboBox.SelectedIndex = (int) ownerGraphForm.AnnotationSettings.ChromatogramMatchToleranceUnit;
				matchToleranceCheckbox.Checked = ownerGraphForm.AnnotationSettings.ChromatogramMatchToleranceOverride;
				matchToleranceTextBox.Text = ownerGraphForm.AnnotationSettings.ChromatogramMatchTolerance.ToString();
				showIndependentValueLabelCheckbox.Text = "Show time label";
				showDependentValueLabelCheckbox.Text = "Show total intensity label";
				showIndependentValueLabelCheckbox.Checked = ownerGraphForm.AnnotationSettings.ShowChromatogramPointTimes;
				showDependentValueLabelCheckbox.Checked = ownerGraphForm.AnnotationSettings.ShowChromatogramPointIntensities;
			}

			if( !matchToleranceCheckbox.Checked )
			{
				matchToleranceTextBox.Enabled = false;
				matchToleranceUnitsComboBox.Enabled = false;
			}

			showMatchedAnnotationsCheckbox.Checked = ownerGraphForm.AnnotationSettings.ShowMatchedAnnotations;
			showUnmatchedAnnotationsCheckbox.Checked = ownerGraphForm.AnnotationSettings.ShowUnmatchedAnnotations;

			foreach( LabelToAliasAndColorPair itr in ownerGraphForm.AnnotationSettings.LabelToAliasAndColorMap )
			{
				aliasAndColorMappingListBox.Items.Add( new LabelToAliasAndColorListItem( itr ) );
			}
		}

		private void okButton_Click( object sender, EventArgs e )
		{
			this.DialogResult = DialogResult.OK;

			if( ownerGraphForm.CurrentScan.IsMassSpectrum )
			{
				ownerGraphForm.AnnotationSettings.ScanMatchToleranceUnit = (SeemsAnnotationSettings.ScanMatchToleranceUnits) matchToleranceUnitsComboBox.SelectedIndex;
				ownerGraphForm.AnnotationSettings.ScanMatchToleranceOverride = matchToleranceCheckbox.Checked;
				ownerGraphForm.AnnotationSettings.ScanMatchTolerance = Convert.ToDouble( matchToleranceTextBox.Text );
				ownerGraphForm.AnnotationSettings.ShowScanPointMZs = showIndependentValueLabelCheckbox.Checked;
				ownerGraphForm.AnnotationSettings.ShowScanPointIntensities = showDependentValueLabelCheckbox.Checked;
			} else
			{
				ownerGraphForm.AnnotationSettings.ChromatogramMatchToleranceUnit = (SeemsAnnotationSettings.ChromatogramMatchToleranceUnits) matchToleranceUnitsComboBox.SelectedIndex;
				ownerGraphForm.AnnotationSettings.ChromatogramMatchToleranceOverride = matchToleranceCheckbox.Checked;
				ownerGraphForm.AnnotationSettings.ChromatogramMatchTolerance = Convert.ToDouble( matchToleranceTextBox.Text );
				ownerGraphForm.AnnotationSettings.ShowChromatogramPointTimes = showIndependentValueLabelCheckbox.Checked;
				ownerGraphForm.AnnotationSettings.ShowChromatogramPointIntensities = showDependentValueLabelCheckbox.Checked;
			}

			ownerGraphForm.AnnotationSettings.ShowMatchedAnnotations = showMatchedAnnotationsCheckbox.Checked;
			ownerGraphForm.AnnotationSettings.ShowUnmatchedAnnotations = showUnmatchedAnnotationsCheckbox.Checked;

			ownerGraphForm.AnnotationSettings.LabelToAliasAndColorMap.Clear();
			foreach( object itr in aliasAndColorMappingListBox.Items )
			{
				LabelToAliasAndColorPair item = ( itr as LabelToAliasAndColorListItem ).mapPair;
				ownerGraphForm.AnnotationSettings.LabelToAliasAndColorMap[item.Key] = new Pair<string, Color>( item.Value.first, item.Value.second );
			}

			ownerGraphForm.updateGraph();
		}

		private void cancelButton_Click( object sender, EventArgs e )
		{
			this.DialogResult = DialogResult.Cancel;
		}

		private void aliasAndColorMappingListBox_DrawItem( object sender, DrawItemEventArgs e )
		{
			e.DrawBackground();
			LabelToAliasAndColorListItem item = (LabelToAliasAndColorListItem) aliasAndColorMappingListBox.Items[e.Index];
			e.Graphics.DrawString( item.ListBoxView,
				aliasAndColorMappingListBox.Font,
				new SolidBrush( aliasAndColorMappingListBox.ForeColor ),
				new PointF( (float) e.Bounds.X, (float) e.Bounds.Y ) );
			Rectangle colorSampleBox = new Rectangle( e.Bounds.Location, e.Bounds.Size );
			colorSampleBox.X = e.Bounds.Right - e.Bounds.Height * 2;
			colorSampleBox.Location.Offset( -5, 0 );
			e.Graphics.FillRectangle( new SolidBrush( ownerGraphForm.ZedGraphControl.GraphPane.Chart.Fill.Color ), colorSampleBox );
			int middle = colorSampleBox.Y + colorSampleBox.Height / 2;
			e.Graphics.DrawLine( new Pen( item.mapPair.Value.second, 2 ), colorSampleBox.Left, middle, colorSampleBox.Right, middle );
			e.DrawFocusRectangle();
		}

		private void addAliasAndColorMappingButton_Click( object sender, EventArgs e )
		{
			AnnotationSettingsAddEditDialog dialog = new AnnotationSettingsAddEditDialog( ownerGraphForm.ZedGraphControl.GraphPane.Chart.Fill.Color );
			if( dialog.ShowDialog() == DialogResult.OK )
			{
				aliasAndColorMappingListBox.Items.Add(
					new LabelToAliasAndColorListItem(
						new Map<string, Pair<string, Color>>.MapPair( dialog.label,
							new Pair<string, Color>( dialog.alias, dialog.color ) ) ) );
			}
		}

		private void editAliasAndColorMappingButton_Click( object sender, EventArgs e )
		{
			LabelToAliasAndColorListItem item = (LabelToAliasAndColorListItem) aliasAndColorMappingListBox.SelectedItem;
			AnnotationSettingsAddEditDialog dialog = new AnnotationSettingsAddEditDialog( ownerGraphForm.ZedGraphControl.GraphPane.Chart.Fill.Color, item.mapPair.Key, item.mapPair.Value.first, item.mapPair.Value.second );
			if( dialog.ShowDialog() == DialogResult.OK )
			{
				if( item.mapPair.Key == dialog.label )
				{
					item.mapPair.Value.first = dialog.alias;
					item.mapPair.Value.second = dialog.color;
				} else
				{
					aliasAndColorMappingListBox.Items.RemoveAt( aliasAndColorMappingListBox.SelectedIndex );
					aliasAndColorMappingListBox.Items.Add(
					new LabelToAliasAndColorListItem(
						new Map<string, Pair<string, Color>>.MapPair( dialog.label,
							new Pair<string, Color>( dialog.alias, dialog.color ) ) ) );
				}
				aliasAndColorMappingListBox.Refresh();
			}
		}

		private void removeAliasAndColorMappingButton_Click( object sender, EventArgs e )
		{
			if( aliasAndColorMappingListBox.SelectedItem != null )
				aliasAndColorMappingListBox.Items.RemoveAt( aliasAndColorMappingListBox.SelectedIndex );
		}

		private void clearAliasAndColorMappingButton_Click( object sender, EventArgs e )
		{
			aliasAndColorMappingListBox.Items.Clear();
		}

		private void matchToleranceTextBox_TextChanged( object sender, EventArgs e )
		{
			if( matchToleranceTextBox.Text == "." )
				matchToleranceTextBox.Text = "0.";
			else
			{
				double result;
				if( !Double.TryParse( matchToleranceTextBox.Text, out result ) )
				{
					if( ownerGraphForm.CurrentScan.IsMassSpectrum )
						matchToleranceTextBox.Text = ownerGraphForm.AnnotationSettings.ScanMatchTolerance.ToString();
					else
						matchToleranceTextBox.Text = ownerGraphForm.AnnotationSettings.ChromatogramMatchTolerance.ToString();
				}
			}
		}

		private void matchToleranceCheckbox_CheckedChanged( object sender, EventArgs e )
		{
			if( !matchToleranceCheckbox.Checked )
			{
				matchToleranceTextBox.Enabled = false;
				matchToleranceUnitsComboBox.Enabled = false;
				if( ownerGraphForm.CurrentScan.IsMassSpectrum )
				{
					matchToleranceUnitsComboBox.SelectedIndex = (int) ownerGraphForm.AnnotationSettings.ScanMatchToleranceUnit;
					matchToleranceTextBox.Text = ownerGraphForm.AnnotationSettings.ScanMatchTolerance.ToString();
				} else
				{
					matchToleranceUnitsComboBox.SelectedIndex = (int) ownerGraphForm.AnnotationSettings.ChromatogramMatchToleranceUnit;
					matchToleranceTextBox.Text = ownerGraphForm.AnnotationSettings.ChromatogramMatchTolerance.ToString();
				}
			} else
			{
				matchToleranceTextBox.Enabled = true;
				matchToleranceUnitsComboBox.Enabled = true;
			}
		}
	}

	internal class LabelToAliasAndColorListItem
	{
		internal LabelToAliasAndColorPair mapPair;
		internal LabelToAliasAndColorListItem( LabelToAliasAndColorPair mapPair )
		{
			this.mapPair = mapPair;
		}

		internal string ListBoxView
		{
			get
			{
				return String.Format( "{0,-13}{1,-9}", mapPair.Key.Substring( 0, Math.Min( 6, mapPair.Key.Length ) ), mapPair.Value.first.Substring( 0, Math.Min( 4, mapPair.Value.first.Length ) ) );
			}
		}
	}
}