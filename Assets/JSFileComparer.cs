using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;


public class JSFileComparer : IComparer<string[]>
{
    public int Compare(string[] f1, string[] f2)
    {
        string name1 = getFile(f1[0]);
        string name2 = getFile(f2[0]);
        return String.Compare(name1.ToLower(), name2.ToLower());
    }

    string getFile(string s)
    {
        if (s.IndexOf("/") > -1)
        {
            s = s.Substring(s.LastIndexOf("/") + 1);
        }
        if (s.IndexOf("\\") > -1)
        {
            s = s.Substring(s.LastIndexOf("\\") + 1);
        }
        return s;
    }
}
