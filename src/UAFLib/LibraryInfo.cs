using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace UAFLib
{
    class LibraryInfo : List<LibraryFile>
    {
        int totalNumLines = 0;
        public Object[] getErrorFileAndLine(int overallLine)
        {
            if (overallLine > totalNumLines)
            {
                return new object[] { null, overallLine};
            }

            //LibraryFile lastFile = this[0];
            int runningTotal = 0;
            foreach (LibraryFile file in this)
            {
                runningTotal += file.mLines;
                if (runningTotal > overallLine)
                {
                    return new Object[] { file, runningTotal - overallLine };
                }
            }
            return new Object[] { this[this.Count - 1], overallLine };
        }

        public new void Add(LibraryFile item)
        {
            base.Add(item);
            totalNumLines += item.mLines;
        }
    }
}
