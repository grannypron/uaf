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

public class LocalEngineLoader : IEngineLoader
{
    private string mRootPath = "";
    public LocalEngineLoader(string rootPath)
    {
        mRootPath = rootPath;
    }

    LocalEngineLoader() { }

    public IEnumerator loadEngine(XmlDocument configDoc, Engine engine, UnityUAFEventManager unityUAFEventManager, InitComplete initComplete)
    {

        String jsIndexUrl = configDoc.SelectNodes("//node()[local-name() = 'jsLibraryIndex']")[0].InnerText;
        LoadFiles(jsIndexUrl, engine);

        String setupFileContents = "";
        using (FileStream fs = new FileStream(@"C:\Users\Shadow\Desktop\uaf.git\uaf-unity\setup.js", FileMode.Open, FileAccess.Read))
        {
            setupFileContents = new StreamReader(fs).ReadToEnd();
        }
        engine.Execute(setupFileContents);

        initComplete.Invoke();
        return null;
    }


    private void LoadFiles(string path, Engine engine)
    {
        XmlDocument indexDoc = new XmlDocument();
        indexDoc.Load(path);
        XmlNodeList nodes = indexDoc.SelectNodes("//node()[local-name() = 'ItemGroup']/node()[local-name() = 'Content']/@Include");

        List<string[]> lib = new List<string[]>();
        foreach (XmlNode node in nodes)
        {
            if (node.InnerText != null && node.InnerText.StartsWith("js\\"))
            {
                String fileContents = "";
                using (FileStream fs = new FileStream(this.mRootPath + "\\" + node.InnerText, FileMode.Open, FileAccess.Read))
                {
                    fileContents = new StreamReader(fs).ReadToEnd();
                }
                lib.Add(new string[] { node.InnerText, fileContents });
            }
        }

        lib.Sort(new JSFileComparer());

        foreach (string[] fileData in lib)
        {
            engine.Execute(fileData[1]);
        }

    }

}
