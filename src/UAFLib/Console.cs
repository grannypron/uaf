using Jint;
using Jint.Runtime;
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Net;
using System.Xml;

namespace UAFLib
{
    public class Console
    {
        public string mJSPath = @"..\..\..\UAFLib\js\";
        public string mJSUrls = "https://raw.githubusercontent.com/grannypron/uaf/port/src/UAFLib/UAFLib.csproj";//null;

        public void helloWorld()
        {

            Engine engine = new Engine(cfg => cfg.AllowClr(typeof(MFCSerializer).Assembly));
            LoadFilesFromGitHub("https://raw.githubusercontent.com/grannypron/uaf/port/src/UAFLib/UAFLib.csproj", engine);

            try
            {
                //**TODO: go back through the C++ source and find all instances of "delete" and re-insert them back with calls to a MemoryManager() class.  Apparently jint responds to delete statements, although all engines might not
                runTest(@"..\..\..\UAFLib\Tests\TestUtilHex.js");
                runTest(@"..\..\..\UAFLib\Tests\TestStripFilenamePath.js");
                runTest(@"..\..\..\UAFLib\Tests\TestCharacterSerialize.js");
                runTest(@"..\..\..\UAFLib\Tests\TestTagList.js");
                runTest(@"..\..\..\UAFLib\Tests\TestPathManager.js");
                runTest(@"..\..\..\UAFLib\Tests\TestRollDice.js");
                runTest(@"..\..\..\UAFLib\Tests\TestGlobalStats.js");
                runTest(@"..\..\..\UAFLib\Tests\TestLoadSAs.js");
                ConsoleResults results = runTest(@"..\..\..\UAFLib\Tests\TestSimpleCombat.js");
                System.Console.WriteLine(results.payload);
                // Next: make a test case where the monsters get added - see COMBAT_DATA.prototype.AddMonstersToCombatants and implement COMBAT_DATA.prototype.determineInitCombatPos
                // Next: Get GenerateIndoorCombatMap() working
            }
            catch (JavaScriptException ex)
            {
                System.Console.WriteLine(ex.Message);
                //Object[] errData = lib.getErrorFileAndLine(ex.Location.Start.Line);
                //Console.WriteLine("File: " + errData[0] + "  Line: " + errData[1]);  // not working atm
                System.Console.WriteLine("  Line: " + ex.Location.Start.Line);
                System.Console.WriteLine(ex.Message);
                System.Console.WriteLine(ex.CallStack);
            }

        }

        public ConsoleResults runTest(string path)
        {
            ConsoleResults results = new ConsoleResults();
            Engine engine = new Engine(cfg => cfg.AllowClr(typeof(MFCSerializer).Assembly));
            if (mJSUrls != null)
            {
                LibraryInfo lib = LoadFiles(this.mJSUrls, engine);
            } else {
                LibraryInfo lib = LoadFiles(this.mJSPath, engine);
            }

            engine.SetValue("consoleResults", results).Execute(LoadFileFromString(path));
            return results;
        }

        private string LoadFileFromString(string path)
        {
            StreamReader sr = new StreamReader(path);
            string str = sr.ReadToEnd();
            sr.Close();
            return str;
        }

        private LibraryInfo LoadFiles(string path, Engine engine)
        {
            LibraryInfo lib = new LibraryInfo();
            foreach (string file in Directory.GetFiles(path, "*.js"))
            {
                String fileContents = LoadFileFromString(file);
                int lineCount = fileContents.Split('\n').Length;
                engine.Execute(fileContents);
                lib.Add(new LibraryFile(file, lineCount));
                //System.Console.WriteLine(file + ":" + lineCount);
            }
            return lib;
        }

        private LibraryInfo LoadFilesFromGitHub(string url, Engine engine)
        {

            XmlDocument doc = new XmlDocument();
            doc.Load(mJSUrls);
            XmlNodeList nodes = doc.SelectNodes("//node()[local-name() = 'ItemGroup']/node()[local-name() = 'Content']/@Include");
            LibraryInfo lib = new LibraryInfo();
            foreach (XmlNode node in nodes)
            {
                if (node.InnerText != null && node.InnerText.StartsWith("js\\")) {
                    string ghUrl = "https://raw.githubusercontent.com/grannypron/uaf/port/src/UAFLib/" + node.InnerText.Replace("\\", "/");
                    using (var client = new WebClient())
                    {
                        String fileContents = client.DownloadString(ghUrl);
                        int lineCount = fileContents.Split('\n').Length;
                        engine.Execute(fileContents);
                        lib.Add(new LibraryFile(ghUrl, lineCount));
                    }
                }
            }
            return lib;
        }
    }
}
