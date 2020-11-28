using System;
using System.Collections.Generic;
using System.IO;
using System.Xml;
using UAFLib.serialization;

namespace UAFLib.dataLoaders
{
    public class ItemLoader : BaseLoader
    {

        // This method is primary used in testing
        public Dictionary<string, Dictionary<string, List<string>>> loadFromBinaryFile(string path)
        {
            using (FileStream fs = new FileStream(path, FileMode.Open, FileAccess.Read))
            {
                byte[] data = new byte[fs.Length];
                fs.Read(data, 0, (int)fs.Length);
                return loadFromBinary(data);
            }
        }

        public Dictionary<string, Dictionary<string, List<string>>> loadFromBinary(byte[] data)
        {
            MemoryStream source = new MemoryStream(data);
            Dictionary<string, Dictionary<string, List<string>>> items = ProtoBuf.Serializer.Deserialize<Dictionary<string, Dictionary<string, List<string>>>>(source);
            return items;

        }

        public override Dictionary<string, object> load(XmlDocument doc)
        {
            Dictionary<String, object> returnData = new Dictionary<string, object>();
            XmlNodeList items = doc.SelectNodes("//node()[local-name()='item']");

            foreach (XmlNode node in items)
            {
                Dictionary<string, List<String>> itemData = new Dictionary<string, List<String>>();
                foreach (XmlAttribute attribute in node.Attributes)
                {
                    itemData[attribute.Name] = new List<String>() {attribute.Value};
                }

                List<string> specAbs = new List<string>();
                foreach (XmlNode specAbNode in node.SelectNodes("node()[local-name() = 'SpecialAbility']"))
                {
                    specAbs.Add(specAbNode.InnerText);
                }
                itemData["SpecialAbilities"] = specAbs;

                List<string> baseClasses = new List<string>();
                foreach (XmlNode baseClassNode in node.SelectNodes("node()[local-name() = 'baseclass']"))
                {
                    baseClasses.Add(baseClassNode.InnerText);
                }
                itemData["BaseClasses"] = baseClasses;
                returnData[itemData["id_name"][0].ToString()] = itemData;
            }
            return returnData;
        }
    }
}
