using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace UAFLib
{
    class LibraryFile
    {
        public int mLines;
        public String mName;
        public LibraryFile(String name, int lines)
        {
            mName = name;
            mLines = lines;
        }
    }
}
