using Jint;
using System;
using System.Collections;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Xml;
using UAFLib;
using UnityEngine;
using UnityEngine.Networking;

public class GitHubEngineLoader : MonoBehaviour, IEngineLoader
{
    public IEnumerator loadEngine(XmlDocument configDoc, Engine engine, InitComplete initComplete)
    {

        String jsIndexUrl = configDoc.SelectNodes("//node()[local-name() = 'jsLibraryIndex']")[0].InnerText;

        UnityWebRequest jsIndexReq = UnityWebRequest.Get(jsIndexUrl);
        yield return jsIndexReq.SendWebRequest();

        XmlDocument indexDoc = new XmlDocument();
        String indexAsStr = jsIndexReq.downloadHandler.text;
        if (indexAsStr[0] > 1000)
        {
            indexAsStr = indexAsStr.Substring(1);
        }
        indexDoc.LoadXml(indexAsStr);
        XmlNodeList nodes = indexDoc.SelectNodes("//node()[local-name() = 'ItemGroup']/node()[local-name() = 'Content']/@Include");
        List<string[]> jsURLs = new List<string[]>();
        // Save in list to alphabetize
        foreach (XmlNode node in nodes)
        {
            if (node.InnerText != null && node.InnerText.StartsWith("js\\"))
            {
                string jaUrl = "https://raw.githubusercontent.com/grannypron/uaf/port/src/UAFLib/" + node.InnerText.Replace("\\", "/");
                jsURLs.Add(new string[] { jaUrl, jaUrl });
            }
        }

        jsURLs.Sort(new JSFileComparer());


        foreach (string[] jsUrls in jsURLs)
        {
            string jsUrl = jsUrls[0];
            Debug.Log("Loading " + jsUrl);
            UnityWebRequest jsReq = UnityWebRequest.Get(jsUrl);
            yield return jsReq.SendWebRequest();
            String fileContents = jsReq.downloadHandler.text;
            int lineCount = fileContents.Split('\n').Length;
            Debug.Log(jsUrl + " loaded.  Length: " + jsReq.downloadHandler.text.Length + ".  Lines: " + lineCount);
            engine.Execute(fileContents);
            Debug.Log(jsUrl + " executed.");
        }

        String setupFileContents = "";
        using (FileStream fs = new FileStream(@"C:\Users\Shadow\Desktop\uaf.git\uaf-unity\setup.js", FileMode.Open, FileAccess.Read))
        {
            setupFileContents = new StreamReader(fs).ReadToEnd();
        }

        int setupFileLineCount = setupFileContents.Split('\n').Length;
        ConsoleResults setupResults = new ConsoleResults();
        engine.SetValue("consoleResults", setupResults).Execute(setupFileContents);
        initComplete.Invoke(setupResults);
    }

}
