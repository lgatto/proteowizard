namespace seems
{
	partial class GraphForm
	{
		/// <summary>
		/// Required designer variable.
		/// </summary>
		private System.ComponentModel.IContainer components = null;

		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		/// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
		protected override void Dispose( bool disposing )
		{
			if( disposing && ( components != null ) )
			{
				components.Dispose();
			}
			base.Dispose( disposing );
		}

		#region Windows Form Designer generated code

		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		private void InitializeComponent()
		{
			this.components = new System.ComponentModel.Container();
			this.zedGraphControl1 = new ZedGraph.ZedGraphControl();
			this.scanNumberComboBoxContextMenuStrip = new System.Windows.Forms.ContextMenuStrip( this.components );
			this.openInActiveWindowToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
			this.openInNewWindowToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
			this.overlayOnActiveWindowToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
			this.scanNumberComboBoxContextMenuStrip.SuspendLayout();
			this.SuspendLayout();
			// 
			// zedGraphControl1
			// 
			this.zedGraphControl1.Dock = System.Windows.Forms.DockStyle.Fill;
			this.zedGraphControl1.Location = new System.Drawing.Point( 0, 0 );
			this.zedGraphControl1.Name = "zedGraphControl1";
			this.zedGraphControl1.ScrollGrace = 0;
			this.zedGraphControl1.ScrollMaxX = 0;
			this.zedGraphControl1.ScrollMaxY = 0;
			this.zedGraphControl1.ScrollMaxY2 = 0;
			this.zedGraphControl1.ScrollMinX = 0;
			this.zedGraphControl1.ScrollMinY = 0;
			this.zedGraphControl1.ScrollMinY2 = 0;
			this.zedGraphControl1.Size = new System.Drawing.Size( 464, 413 );
			this.zedGraphControl1.TabIndex = 0;
			this.zedGraphControl1.MouseClick += new System.Windows.Forms.MouseEventHandler( this.GraphForm_MouseClick );
			this.zedGraphControl1.PreviewKeyDown += new System.Windows.Forms.PreviewKeyDownEventHandler( this.GraphForm_PreviewKeyDown );
			this.zedGraphControl1.Resize += new System.EventHandler( this.zedGraphControl1_Resize );
			this.zedGraphControl1.ZoomEvent += new ZedGraph.ZedGraphControl.ZoomEventHandler( this.zedGraphControl1_ZoomEvent );
			// 
			// scanNumberComboBoxContextMenuStrip
			// 
			this.scanNumberComboBoxContextMenuStrip.Items.AddRange( new System.Windows.Forms.ToolStripItem[] {
            this.openInActiveWindowToolStripMenuItem,
            this.openInNewWindowToolStripMenuItem,
            this.overlayOnActiveWindowToolStripMenuItem} );
			this.scanNumberComboBoxContextMenuStrip.Name = "scanNumberComboBoxContextMenuStrip";
			this.scanNumberComboBoxContextMenuStrip.Size = new System.Drawing.Size( 211, 70 );
			// 
			// openInActiveWindowToolStripMenuItem
			// 
			this.openInActiveWindowToolStripMenuItem.Font = new System.Drawing.Font( "Tahoma", 8.25F, System.Drawing.FontStyle.Bold );
			this.openInActiveWindowToolStripMenuItem.Name = "openInActiveWindowToolStripMenuItem";
			this.openInActiveWindowToolStripMenuItem.Size = new System.Drawing.Size( 210, 22 );
			this.openInActiveWindowToolStripMenuItem.Text = "Open in active window";
			this.openInActiveWindowToolStripMenuItem.Click += new System.EventHandler( this.openInActiveWindowToolStripMenuItem_Click );
			// 
			// openInNewWindowToolStripMenuItem
			// 
			this.openInNewWindowToolStripMenuItem.Name = "openInNewWindowToolStripMenuItem";
			this.openInNewWindowToolStripMenuItem.Size = new System.Drawing.Size( 210, 22 );
			this.openInNewWindowToolStripMenuItem.Text = "Open in new window";
			this.openInNewWindowToolStripMenuItem.Click += new System.EventHandler( this.openInNewWindowToolStripMenuItem_Click );
			// 
			// overlayOnActiveWindowToolStripMenuItem
			// 
			this.overlayOnActiveWindowToolStripMenuItem.Name = "overlayOnActiveWindowToolStripMenuItem";
			this.overlayOnActiveWindowToolStripMenuItem.Size = new System.Drawing.Size( 210, 22 );
			this.overlayOnActiveWindowToolStripMenuItem.Text = "Overlay on active window";
			this.overlayOnActiveWindowToolStripMenuItem.Click += new System.EventHandler( this.overlayOnActiveWindowToolStripMenuItem_Click );
			// 
			// GraphForm
			// 
			this.AutoScaleDimensions = new System.Drawing.SizeF( 6F, 13F );
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.ClientSize = new System.Drawing.Size( 464, 413 );
			this.Controls.Add( this.zedGraphControl1 );
			this.MinimumSize = new System.Drawing.Size( 100, 50 );
			this.Name = "GraphForm";
			this.Text = "GraphForm";
			this.ResizeBegin += new System.EventHandler( this.GraphForm_ResizeBegin );
			this.MouseClick += new System.Windows.Forms.MouseEventHandler( this.GraphForm_MouseClick );
			this.ResizeEnd += new System.EventHandler( this.GraphForm_ResizeEnd );
			this.scanNumberComboBoxContextMenuStrip.ResumeLayout( false );
			this.ResumeLayout( false );

		}

		#endregion

		private ZedGraph.ZedGraphControl zedGraphControl1;
		private System.Windows.Forms.ContextMenuStrip scanNumberComboBoxContextMenuStrip;
		private System.Windows.Forms.ToolStripMenuItem openInActiveWindowToolStripMenuItem;
		private System.Windows.Forms.ToolStripMenuItem openInNewWindowToolStripMenuItem;
		private System.Windows.Forms.ToolStripMenuItem overlayOnActiveWindowToolStripMenuItem;
	}
}