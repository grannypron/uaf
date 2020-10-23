using System;
using System.Collections.Generic;
using System.Linq;
using System.Xml;

namespace UAFLib
{
    class SpecabilityLoader
    {
        public Dictionary<String, object[]> load(string path)
        {

            Dictionary<String, object[]> returnData = new Dictionary<string, object[]>();

            XmlDocument doc = new XmlDocument();
            doc.Load(path);
            XmlNodeList items = doc.SelectNodes("//node()[local-name()='item']");

            foreach (XmlNode node in items)
            {
                XmlNode nameNode = node.ChildNodes[0];
                string name = null;
                if (nameNode != null)
                {
                    name = nameNode.InnerText;
                }
                if (name != null && !name.Equals("idk"))
                {
                    XmlNodeList asls = node.SelectNodes("asl//node()[local-name()='script']");
                    object[] aslValues = new object[asls.Count];
                    for (int idxAsl = 0; idxAsl < asls.Count; idxAsl++)
                    {
                        aslValues[idxAsl] = new string[] { asls[idxAsl].Attributes["name"].Value, asls[idxAsl].InnerText };
                    }
                    returnData.Add(name, aslValues);
                }
            }
            return returnData;
        }
    }
}
