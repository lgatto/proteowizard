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

using System;
using System.IO;
using System.Reflection;

namespace pwiz.SkylineTestUtil
{
    /// <summary>
    /// MemoryProfiler creates memory snapshots if the SciTech Memory Profiler is running.
    /// </summary>
    public static class MemoryProfiler
    {
        private const string PROFILER_DLL = @"C:\Program Files\SciTech\NetMemProfiler4\Redist\MemProfiler2.dll";    // Not L10N
        private const string PROFILER_TYPE = "SciTech.NetMemProfiler.MemProfiler";   // Not L10N

        private static readonly MethodInfo FULL_SNAP_SHOT;

        static MemoryProfiler()
        {
            if (File.Exists(PROFILER_DLL))
            {
                var profilerAssembly = Assembly.LoadFrom(PROFILER_DLL);
                var profiler = profilerAssembly.GetType(PROFILER_TYPE);
                if (profiler != null && (bool) profiler.GetMethod("get_IsProfiling").Invoke(null, null))
                {
                    FULL_SNAP_SHOT = profiler.GetMethod("FullSnapShot", Type.EmptyTypes);
                }
            }
        }

        /// <summary>
        /// Take a memory shapshot.
        /// </summary>
        public static void Snapshot()
        {
            if (FULL_SNAP_SHOT != null)
            {
                FULL_SNAP_SHOT.Invoke(null, null);
            }
        }
    }
}
