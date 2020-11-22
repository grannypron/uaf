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

public class ResourceEngineLoader : IEngineLoader
{
    string mJSFolder;
    string mSetupPath;
    ResourceEngineLoader() 
    {
    }

    public ResourceEngineLoader(string jsFolder, string setupPath)
    {
        this.mJSFolder = jsFolder;
        this.mSetupPath = setupPath;
    }

    public IEnumerator loadEngine(XmlDocument configDoc, Engine engine, UnityUAFEventManager unityUAFEventManager, InitComplete initComplete)
    {
        LoadFiles(this.mJSFolder, engine);
        TextAsset setupAsset = (TextAsset)Resources.Load(this.mSetupPath);
        engine.Execute(setupAsset.text);
        initComplete.Invoke();
        return null;
    }

    private void LoadFiles(string path, Engine engine)
    {

        // Get list of all js files in the resource directory
        UnityEngine.Object[] jsAssets = Resources.LoadAll(path, typeof(TextAsset));

        List<string[]> lib = new List<string[]>();
        foreach (UnityEngine.Object jsAsset in jsAssets)
        {
            if (((TextAsset)jsAsset).name.IndexOf("Tests/") < 0) { 
                String fileContents = ((TextAsset)jsAsset).text;
                lib.Add(new string[] { ((TextAsset)jsAsset).name, fileContents });
            }
        }

        lib.Sort(new JSFileComparer());

        foreach (string[] fileData in lib)
        {
            engine.Execute(fileData[1]);
        }

    }

}
