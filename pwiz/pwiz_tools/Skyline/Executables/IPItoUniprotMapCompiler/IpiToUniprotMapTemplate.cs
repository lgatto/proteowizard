/*
 * Original author: Brian Pratt <bspratt .at. u.washington.edu>,
 *                  MacCoss Lab, Department of Genome Sciences, UW
 *
 * Copyright 2014 University of Washington - Seattle, WA
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
using System.IO;
using System.Reflection;
using Ionic.Zip;

namespace pwiz.ProteomeDatabase.Fasta
{
    /// <summary>
    /// 
    /// IPI support is going away in most online protein servers,
    /// handle this mapping locally instead.
    /// 
    /// This file is generated by the IpiToUniprotMapCompiler project
    /// in the Skyline\Executables area.  Do not edit it directly.
    /// 
    /// It's based on  
    /// ftp://ftp.uniprot.org/pub/databases/uniprot/current_release/knowledgebase/idmapping/last-UniProtKB2IPI.map.gz.
    /// 
    /// </summary>
    public class IpiToUniprotMap
    {

        // Implementation notes 
        //
        // Parallel arrays for best packing efficiency in 64 bit, per nicksh
        //
        // You might think a dictionary is the way to go, but with nearly 100,000
        // entries it takes quite a while to construct, plus at that size it can
        // cause issues with memory use (ends up on the Large Object Heap).  
        // Using a presorted array and binary search gives much better overall performance.
        //
        // A simple index instead of this lookup scheme would require a table of 
        // 1,028,485 ints (the largest IPI number) for only  98,650 unique values.
        //
        // But even at 98,650 values we end up in the Large Object Heap if we don't
        // subdivide (we don't want to be in the LOH because it doesn't garbage 
        // collect as readily as we'd like).

        // DECLARE_IPI_COUNT - this gets replaced with table size and chunksize declaration
        private readonly List<int[]> _ipi;
        private readonly List<int> _chunkStarts;
        private readonly List<string[]> _accession;

        /// <summary>
        /// Map an IPI string to a Uniprot accession id, based on the table taken from
        /// ftp://ftp.uniprot.org/pub/databases/uniprot/current_release/knowledgebase/idmapping/last-UniProtKB2IPI.map.gz.
        /// </summary>
        /// <param name="strIPI">a string like IPI12345678 or ipi_12345678 etc (others just pass through without mapping)</param>
        /// <returns>a string with uniprot mapping, or the input string if no mapping exists</returns>
        public string MapToUniprot(string strIPI)
        {
            if (strIPI.ToUpperInvariant().StartsWith("IPI")) // ipi, IPI, IPI:IPI, iPi_ etc // Not L10N
            {
                strIPI = strIPI.Split('.')[0]; // Drop the version number if any
                for (int len = 3; len < strIPI.Length; )
                {
                    int code;
                    if (int.TryParse(strIPI.Substring(len++), out code))
                    {
                        // Which chunk is it in, if it exists?
                        int chunk = _chunkStarts.BinarySearch(code);
                        if (chunk < 0)
                            chunk = (~chunk - 1);
                        if ((chunk >= 0) && (chunk < SEGMENT_COUNT))
                        {
                            int index = Array.BinarySearch(_ipi[chunk], code);
                            if (index >= 0)  // We do expect an exact match
                                return _accession[chunk][index];
                        }
                        break; // It's a number, but not in the table
                    }
                }
            }
            return strIPI; // No mapping
        }


        public IpiToUniprotMap()
        {
            int added = 0;
            _ipi = new List<int[]>(SEGMENT_COUNT);
            _accession = new List<String[]>(SEGMENT_COUNT);
            _chunkStarts = new List<int>(SEGMENT_COUNT + 1);
            for (var iseg = SEGMENT_COUNT; iseg-- > 0; )
            {
                _ipi.Add(new int[SEGMENT_SIZE]);
                _accession.Add(new String[SEGMENT_SIZE]);
            }

            // Add the mappings from an embedded resouce file.  See notes above for why this is done dynamically.
            var streamInfo = Assembly.GetExecutingAssembly().GetManifestResourceStream("pwiz.ProteomeDatabase.Fasta.IpiToUniprotMap.zip"); // Not L10N
            using (var zip = ZipFile.Read(streamInfo))
            {
                var entry = zip["MapUniprotIPI.txt"]; // Not L10N
                using (var zstream = entry.OpenReader())
                {
                    using (var stream = new StreamReader(zstream))
                    {
                        string line = stream.ReadLine();
                        while (line != null)
                        {
                            var pair = line.Split(' ');
                            _ipi[added / SEGMENT_SIZE][added % SEGMENT_SIZE] = Convert.ToInt32(pair[0]);
                            _accession[added / SEGMENT_SIZE][added % SEGMENT_SIZE] = pair[1];
                            added++;
                            line = stream.ReadLine();
                        }
                    }
                }
            }
            while (added < SEGMENT_SIZE * SEGMENT_COUNT)
            {
                _ipi[added / SEGMENT_SIZE][added % SEGMENT_SIZE] = int.MaxValue; // Fill out the last chunk so binary search works properly
                added++;
            }

            for (int chunk = 0; chunk < SEGMENT_COUNT; chunk++)
                _chunkStarts.Add(_ipi[chunk][0]);
            _chunkStarts.Add(int.MaxValue);
        }
    }
}