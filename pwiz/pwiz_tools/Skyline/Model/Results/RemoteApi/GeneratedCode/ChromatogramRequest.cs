﻿//------------------------------------------------------------------------------
// <auto-generated>
//     This code was generated by a tool.
//     Runtime Version:4.0.30319.34014
//
//     Changes to this file may cause incorrect behavior and will be lost if
//     the code is regenerated.
// </auto-generated>
//------------------------------------------------------------------------------

// 
// This source code was auto-generated by xsd, Version=4.0.30319.33440.
// 
namespace pwiz.Skyline.Model.Results.RemoteApi.GeneratedCode {
    using System.Xml.Serialization;
    
    
    /// <remarks/>
    [System.CodeDom.Compiler.GeneratedCodeAttribute("xsd", "4.0.30319.33440")]
    [System.SerializableAttribute()]
    [System.Diagnostics.DebuggerStepThroughAttribute()]
    [System.ComponentModel.DesignerCategoryAttribute("code")]
    [System.Xml.Serialization.XmlTypeAttribute(AnonymousType=true)]
    [System.Xml.Serialization.XmlRootAttribute(Namespace="", IsNullable=false)]
    public partial class ChromatogramRequestDocument {
        
        private ChromatogramRequestDocumentIsolationScheme isolationSchemeField;
        
        private ChromatogramRequestDocumentChromatogramGroup[] chromatogramGroupField;
        
        private int minMzField;
        
        private int maxMzField;
        
        private double mzMatchToleranceField;
        
        private Ms2FullScanAcquisitionMethod ms2FullScanAcquisitionMethodField;
        
        private double minTimeField;
        
        private bool minTimeFieldSpecified;
        
        private double maxTimeField;
        
        private bool maxTimeFieldSpecified;
        
        public ChromatogramRequestDocument() {
            this.ms2FullScanAcquisitionMethodField = Ms2FullScanAcquisitionMethod.None;
        }
        
        /// <remarks/>
        public ChromatogramRequestDocumentIsolationScheme IsolationScheme {
            get {
                return this.isolationSchemeField;
            }
            set {
                this.isolationSchemeField = value;
            }
        }
        
        /// <remarks/>
        [System.Xml.Serialization.XmlElementAttribute("ChromatogramGroup")]
        public ChromatogramRequestDocumentChromatogramGroup[] ChromatogramGroup {
            get {
                return this.chromatogramGroupField;
            }
            set {
                this.chromatogramGroupField = value;
            }
        }
        
        /// <remarks/>
        [System.Xml.Serialization.XmlAttributeAttribute()]
        public int MinMz {
            get {
                return this.minMzField;
            }
            set {
                this.minMzField = value;
            }
        }
        
        /// <remarks/>
        [System.Xml.Serialization.XmlAttributeAttribute()]
        public int MaxMz {
            get {
                return this.maxMzField;
            }
            set {
                this.maxMzField = value;
            }
        }
        
        /// <remarks/>
        [System.Xml.Serialization.XmlAttributeAttribute()]
        public double MzMatchTolerance {
            get {
                return this.mzMatchToleranceField;
            }
            set {
                this.mzMatchToleranceField = value;
            }
        }
        
        /// <remarks/>
        [System.Xml.Serialization.XmlAttributeAttribute()]
        [System.ComponentModel.DefaultValueAttribute(Ms2FullScanAcquisitionMethod.None)]
        public Ms2FullScanAcquisitionMethod Ms2FullScanAcquisitionMethod {
            get {
                return this.ms2FullScanAcquisitionMethodField;
            }
            set {
                this.ms2FullScanAcquisitionMethodField = value;
            }
        }
        
        /// <remarks/>
        [System.Xml.Serialization.XmlAttributeAttribute()]
        public double MinTime {
            get {
                return this.minTimeField;
            }
            set {
                this.minTimeField = value;
            }
        }
        
        /// <remarks/>
        [System.Xml.Serialization.XmlIgnoreAttribute()]
        public bool MinTimeSpecified {
            get {
                return this.minTimeFieldSpecified;
            }
            set {
                this.minTimeFieldSpecified = value;
            }
        }
        
        /// <remarks/>
        [System.Xml.Serialization.XmlAttributeAttribute()]
        public double MaxTime {
            get {
                return this.maxTimeField;
            }
            set {
                this.maxTimeField = value;
            }
        }
        
        /// <remarks/>
        [System.Xml.Serialization.XmlIgnoreAttribute()]
        public bool MaxTimeSpecified {
            get {
                return this.maxTimeFieldSpecified;
            }
            set {
                this.maxTimeFieldSpecified = value;
            }
        }
    }
    
    /// <remarks/>
    [System.CodeDom.Compiler.GeneratedCodeAttribute("xsd", "4.0.30319.33440")]
    [System.SerializableAttribute()]
    [System.Diagnostics.DebuggerStepThroughAttribute()]
    [System.ComponentModel.DesignerCategoryAttribute("code")]
    [System.Xml.Serialization.XmlTypeAttribute(AnonymousType=true)]
    public partial class ChromatogramRequestDocumentIsolationScheme {
        
        private ChromatogramRequestDocumentIsolationSchemeIsolationWindow[] isolationWindowField;
        
        private double precursorFilterField;
        
        private bool precursorFilterFieldSpecified;
        
        private double precursorRightFilterField;
        
        private bool precursorRightFilterFieldSpecified;
        
        private string specialHandlingField;
        
        private int windowsPerScanField;
        
        private bool windowsPerScanFieldSpecified;
        
        /// <remarks/>
        [System.Xml.Serialization.XmlElementAttribute("IsolationWindow")]
        public ChromatogramRequestDocumentIsolationSchemeIsolationWindow[] IsolationWindow {
            get {
                return this.isolationWindowField;
            }
            set {
                this.isolationWindowField = value;
            }
        }
        
        /// <remarks/>
        [System.Xml.Serialization.XmlAttributeAttribute()]
        public double PrecursorFilter {
            get {
                return this.precursorFilterField;
            }
            set {
                this.precursorFilterField = value;
            }
        }
        
        /// <remarks/>
        [System.Xml.Serialization.XmlIgnoreAttribute()]
        public bool PrecursorFilterSpecified {
            get {
                return this.precursorFilterFieldSpecified;
            }
            set {
                this.precursorFilterFieldSpecified = value;
            }
        }
        
        /// <remarks/>
        [System.Xml.Serialization.XmlAttributeAttribute()]
        public double PrecursorRightFilter {
            get {
                return this.precursorRightFilterField;
            }
            set {
                this.precursorRightFilterField = value;
            }
        }
        
        /// <remarks/>
        [System.Xml.Serialization.XmlIgnoreAttribute()]
        public bool PrecursorRightFilterSpecified {
            get {
                return this.precursorRightFilterFieldSpecified;
            }
            set {
                this.precursorRightFilterFieldSpecified = value;
            }
        }
        
        /// <remarks/>
        [System.Xml.Serialization.XmlAttributeAttribute()]
        public string SpecialHandling {
            get {
                return this.specialHandlingField;
            }
            set {
                this.specialHandlingField = value;
            }
        }
        
        /// <remarks/>
        [System.Xml.Serialization.XmlAttributeAttribute()]
        public int WindowsPerScan {
            get {
                return this.windowsPerScanField;
            }
            set {
                this.windowsPerScanField = value;
            }
        }
        
        /// <remarks/>
        [System.Xml.Serialization.XmlIgnoreAttribute()]
        public bool WindowsPerScanSpecified {
            get {
                return this.windowsPerScanFieldSpecified;
            }
            set {
                this.windowsPerScanFieldSpecified = value;
            }
        }
    }
    
    /// <remarks/>
    [System.CodeDom.Compiler.GeneratedCodeAttribute("xsd", "4.0.30319.33440")]
    [System.SerializableAttribute()]
    [System.Diagnostics.DebuggerStepThroughAttribute()]
    [System.ComponentModel.DesignerCategoryAttribute("code")]
    [System.Xml.Serialization.XmlTypeAttribute(AnonymousType=true)]
    public partial class ChromatogramRequestDocumentIsolationSchemeIsolationWindow {
        
        private double startField;
        
        private double endField;
        
        private double targetField;
        
        private bool targetFieldSpecified;
        
        private double startMarginField;
        
        private bool startMarginFieldSpecified;
        
        private double endMarginField;
        
        private bool endMarginFieldSpecified;
        
        /// <remarks/>
        [System.Xml.Serialization.XmlAttributeAttribute()]
        public double Start {
            get {
                return this.startField;
            }
            set {
                this.startField = value;
            }
        }
        
        /// <remarks/>
        [System.Xml.Serialization.XmlAttributeAttribute()]
        public double End {
            get {
                return this.endField;
            }
            set {
                this.endField = value;
            }
        }
        
        /// <remarks/>
        [System.Xml.Serialization.XmlAttributeAttribute()]
        public double Target {
            get {
                return this.targetField;
            }
            set {
                this.targetField = value;
            }
        }
        
        /// <remarks/>
        [System.Xml.Serialization.XmlIgnoreAttribute()]
        public bool TargetSpecified {
            get {
                return this.targetFieldSpecified;
            }
            set {
                this.targetFieldSpecified = value;
            }
        }
        
        /// <remarks/>
        [System.Xml.Serialization.XmlAttributeAttribute()]
        public double StartMargin {
            get {
                return this.startMarginField;
            }
            set {
                this.startMarginField = value;
            }
        }
        
        /// <remarks/>
        [System.Xml.Serialization.XmlIgnoreAttribute()]
        public bool StartMarginSpecified {
            get {
                return this.startMarginFieldSpecified;
            }
            set {
                this.startMarginFieldSpecified = value;
            }
        }
        
        /// <remarks/>
        [System.Xml.Serialization.XmlAttributeAttribute()]
        public double EndMargin {
            get {
                return this.endMarginField;
            }
            set {
                this.endMarginField = value;
            }
        }
        
        /// <remarks/>
        [System.Xml.Serialization.XmlIgnoreAttribute()]
        public bool EndMarginSpecified {
            get {
                return this.endMarginFieldSpecified;
            }
            set {
                this.endMarginFieldSpecified = value;
            }
        }
    }
    
    /// <remarks/>
    [System.CodeDom.Compiler.GeneratedCodeAttribute("xsd", "4.0.30319.33440")]
    [System.SerializableAttribute()]
    [System.Diagnostics.DebuggerStepThroughAttribute()]
    [System.ComponentModel.DesignerCategoryAttribute("code")]
    [System.Xml.Serialization.XmlTypeAttribute(AnonymousType=true)]
    public partial class ChromatogramRequestDocumentChromatogramGroup {
        
        private ChromatogramRequestDocumentChromatogramGroupChromatogram[] chromatogramField;
        
        private double precursorMzField;
        
        private string modifiedSequenceField;
        
        private double minTimeField;
        
        private bool minTimeFieldSpecified;
        
        private double maxTimeField;
        
        private bool maxTimeFieldSpecified;
        
        private ChromExtractor extractorField;
        
        private ChromSource sourceField;
        
        private bool massErrorsField;
        
        private double driftTimeField;
        
        private bool driftTimeFieldSpecified;
        
        private double driftTimeWindowField;
        
        private bool driftTimeWindowFieldSpecified;
        
        public ChromatogramRequestDocumentChromatogramGroup() {
            this.precursorMzField = 0D;
            this.massErrorsField = false;
        }
        
        /// <remarks/>
        [System.Xml.Serialization.XmlElementAttribute("Chromatogram")]
        public ChromatogramRequestDocumentChromatogramGroupChromatogram[] Chromatogram {
            get {
                return this.chromatogramField;
            }
            set {
                this.chromatogramField = value;
            }
        }
        
        /// <remarks/>
        [System.Xml.Serialization.XmlAttributeAttribute()]
        [System.ComponentModel.DefaultValueAttribute(0D)]
        public double PrecursorMz {
            get {
                return this.precursorMzField;
            }
            set {
                this.precursorMzField = value;
            }
        }
        
        /// <remarks/>
        [System.Xml.Serialization.XmlAttributeAttribute()]
        public string ModifiedSequence {
            get {
                return this.modifiedSequenceField;
            }
            set {
                this.modifiedSequenceField = value;
            }
        }
        
        /// <remarks/>
        [System.Xml.Serialization.XmlAttributeAttribute()]
        public double MinTime {
            get {
                return this.minTimeField;
            }
            set {
                this.minTimeField = value;
            }
        }
        
        /// <remarks/>
        [System.Xml.Serialization.XmlIgnoreAttribute()]
        public bool MinTimeSpecified {
            get {
                return this.minTimeFieldSpecified;
            }
            set {
                this.minTimeFieldSpecified = value;
            }
        }
        
        /// <remarks/>
        [System.Xml.Serialization.XmlAttributeAttribute()]
        public double MaxTime {
            get {
                return this.maxTimeField;
            }
            set {
                this.maxTimeField = value;
            }
        }
        
        /// <remarks/>
        [System.Xml.Serialization.XmlIgnoreAttribute()]
        public bool MaxTimeSpecified {
            get {
                return this.maxTimeFieldSpecified;
            }
            set {
                this.maxTimeFieldSpecified = value;
            }
        }
        
        /// <remarks/>
        [System.Xml.Serialization.XmlAttributeAttribute()]
        public ChromExtractor Extractor {
            get {
                return this.extractorField;
            }
            set {
                this.extractorField = value;
            }
        }
        
        /// <remarks/>
        [System.Xml.Serialization.XmlAttributeAttribute()]
        public ChromSource Source {
            get {
                return this.sourceField;
            }
            set {
                this.sourceField = value;
            }
        }
        
        /// <remarks/>
        [System.Xml.Serialization.XmlAttributeAttribute()]
        [System.ComponentModel.DefaultValueAttribute(false)]
        public bool MassErrors {
            get {
                return this.massErrorsField;
            }
            set {
                this.massErrorsField = value;
            }
        }
        
        /// <remarks/>
        [System.Xml.Serialization.XmlAttributeAttribute()]
        public double DriftTime {
            get {
                return this.driftTimeField;
            }
            set {
                this.driftTimeField = value;
            }
        }
        
        /// <remarks/>
        [System.Xml.Serialization.XmlIgnoreAttribute()]
        public bool DriftTimeSpecified {
            get {
                return this.driftTimeFieldSpecified;
            }
            set {
                this.driftTimeFieldSpecified = value;
            }
        }
        
        /// <remarks/>
        [System.Xml.Serialization.XmlAttributeAttribute()]
        public double DriftTimeWindow {
            get {
                return this.driftTimeWindowField;
            }
            set {
                this.driftTimeWindowField = value;
            }
        }
        
        /// <remarks/>
        [System.Xml.Serialization.XmlIgnoreAttribute()]
        public bool DriftTimeWindowSpecified {
            get {
                return this.driftTimeWindowFieldSpecified;
            }
            set {
                this.driftTimeWindowFieldSpecified = value;
            }
        }
    }
    
    /// <remarks/>
    [System.CodeDom.Compiler.GeneratedCodeAttribute("xsd", "4.0.30319.33440")]
    [System.SerializableAttribute()]
    [System.Diagnostics.DebuggerStepThroughAttribute()]
    [System.ComponentModel.DesignerCategoryAttribute("code")]
    [System.Xml.Serialization.XmlTypeAttribute(AnonymousType=true)]
    public partial class ChromatogramRequestDocumentChromatogramGroupChromatogram {
        
        private double productMzField;
        
        private double mzWindowField;
        
        public ChromatogramRequestDocumentChromatogramGroupChromatogram() {
            this.productMzField = 0D;
            this.mzWindowField = 0D;
        }
        
        /// <remarks/>
        [System.Xml.Serialization.XmlAttributeAttribute()]
        [System.ComponentModel.DefaultValueAttribute(0D)]
        public double ProductMz {
            get {
                return this.productMzField;
            }
            set {
                this.productMzField = value;
            }
        }
        
        /// <remarks/>
        [System.Xml.Serialization.XmlAttributeAttribute()]
        [System.ComponentModel.DefaultValueAttribute(0D)]
        public double MzWindow {
            get {
                return this.mzWindowField;
            }
            set {
                this.mzWindowField = value;
            }
        }
    }
    
    /// <remarks/>
    [System.CodeDom.Compiler.GeneratedCodeAttribute("xsd", "4.0.30319.33440")]
    [System.SerializableAttribute()]
    public enum ChromExtractor {
        
        /// <remarks/>
        Summed,
        
        /// <remarks/>
        BasePeak,
    }
    
    /// <remarks/>
    [System.CodeDom.Compiler.GeneratedCodeAttribute("xsd", "4.0.30319.33440")]
    [System.SerializableAttribute()]
    public enum ChromSource {
        
        /// <remarks/>
        Ms1,
        
        /// <remarks/>
        Ms2,
        
        /// <remarks/>
        Sim,
    }
    
    /// <remarks/>
    [System.CodeDom.Compiler.GeneratedCodeAttribute("xsd", "4.0.30319.33440")]
    [System.SerializableAttribute()]
    public enum Ms2FullScanAcquisitionMethod {
        
        /// <remarks/>
        None,
        
        /// <remarks/>
        Targeted,
        
        /// <remarks/>
        DIA,
    }
}
