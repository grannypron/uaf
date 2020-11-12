using System;
using System.Collections.Generic;
using System.Linq;
using System.Xml;
using UAFLib.dataLoaders;

namespace UAFLib.dataLoaders
{
    class ItemLoader : BaseLoader
    {

        public override Dictionary<string, object> load(XmlDocument doc)
        {
            Dictionary<String, object> returnData = new Dictionary<string, object>();
            XmlNodeList items = doc.SelectNodes("//node()[local-name()='item']");

            foreach (XmlNode node in items)
            {
                Dictionary<string, object> itemData = new Dictionary<string, object>();
                foreach (XmlAttribute attribute in node.Attributes)
                {
                    itemData[attribute.Name] = attribute.Value;
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
                returnData[itemData["id_name"].ToString()] = itemData;
            }
            return returnData;
        }
    }
}
