using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Xml;

namespace UAFLib.dataLoaders
{
    public abstract class BaseLoader
    {
        public Dictionary<string, object> load(string path)
        {
            XmlDocument doc = new XmlDocument();
            doc.Load(path);
            return load(doc);
        }
        public abstract Dictionary<string, object> load(XmlDocument doc);
    }
}
