using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace UAFLib
{
    public class LibraryFile
    {
        public int mLines;
        public String mName;
        public String mData;
        public LibraryFile(String name, int lines, String data)
        {
            mName = name;
            mLines = lines;
            mData = data;
        }
    }
}
