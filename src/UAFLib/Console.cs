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
        public string mJSUrls = null;//"https://raw.githubusercontent.com/grannypron/uaf/port/src/UAFLib/UAFLib.csproj";//null;


        public void helloWorld()
        {
            
            try
            {
                runTest(new string[] { @"..\..\..\UAFLib\Tests\TestParseDice.js" });
                runTest(new string[] { @"..\..\..\UAFLib\Tests\TestLoadItems.js" });
                runTest(new string[] { @"..\..\..\UAFLib\Tests\TestLoadSAs.js" });
                runTest(new string[] { @"..\..\..\UAFLib\Tests\TestUtilHex.js"});
                runTest(new string[] { @"..\..\..\UAFLib\Tests\TestEntityType.js" }); 
                runTest(new string[] { @"..\..\..\UAFLib\Tests\TestStripFilenamePath.js"});
                runTest(new string[] { @"..\..\..\UAFLib\Tests\TestCharacterSerialize.js"});
                runTest(new string[] { @"..\..\..\UAFLib\Tests\TestTagList.js"});
                runTest(new string[] { @"..\..\..\UAFLib\Tests\TestPathManager.js"});
                runTest(new string[] { @"..\..\..\UAFLib\Tests\TestRollDice.js"});
                runTest(new string[] { @"..\..\..\UAFLib\Tests\TestGlobalStats.js"});
                runTest(new string[] { @"..\..\..\UAFLib\Tests\TestSetupCombat.js"});
                runTest(new string[] { @"..\..\..\UAFLib\Tests\TestSetupCombat.js", @"..\..\..\UAFLib\Tests\TestSimpleCombatMovement.js" });
                runTest(new string[] { @"..\..\..\UAFLib\Tests\TestFreeAttacks.js" });
                runTest(new string[] { @"..\..\..\UAFLib\Tests\TestSetupCombat.js", @"..\..\..\UAFLib\Tests\TestCombatMovementAttack.js" });
                runTest(new string[] { @"..\..\..\UAFLib\Tests\TestSetupCombat.js", @"..\..\..\UAFLib\Tests\TestCombatMovementAttack.js", @"..\..\..\UAFLib\Tests\TestSimpleUnarmedAttack.js" });
                runTest(new string[] { @"..\..\..\UAFLib\Tests\TestSetupCombat.js", @"..\..\..\UAFLib\Tests\TestCombatMovementAttack.js", @"..\..\..\UAFLib\Tests\TestSimpleUnarmedAttack.js", @"..\..\..\UAFLib\Tests\TestKill.js" });
                runTest(new string[] { @"..\..\..\UAFLib\Tests\TestLoadItems.js", @"..\..\..\UAFLib\Tests\TestInventory.js" });
                
                runTest(new string[] { @"..\..\..\UAFLib\Tests\TestSetupCombat.js", @"..\..\..\UAFLib\Tests\TestCombatMovementAttack.js", @"..\..\..\UAFLib\Tests\TestSimpleAttack.js" });
                runTest(new string[] { @"..\..\..\UAFLib\Tests\TestSetupCombat.js", @"..\..\..\UAFLib\Tests\TestMultiRoundCombat.js" });
                runTest(new string[] { @"..\..\..\UAFLib\Tests\TestSetupCombat.js", @"..\..\..\UAFLib\Tests\TestMonsterAttack.js" });
                runTest(new string[] { @"..\..\..\UAFLib\Tests\TestSetupCombat.js", @"..\..\..\UAFLib\Tests\TestWinCombat.js" });
                runTest(new string[] { @"..\..\..\UAFLib\Tests\TestExperience.js" });
                //runTest(new string[] { @"..\..\..\UAFLib\Tests\DemoCombat.js" });
                //runTest(new string[] { @"..\..\..\UAFLib\Tests\TestGPDL.js" });
                System.Console.WriteLine("Tests complete!  Press Any key.");
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

        public ConsoleResults runTest(string[] paths)
        {
            List<string> strs = new List<string>();
            for (int idx = 0; idx < paths.Length; idx++) {
                string path = paths[idx];
                strs.Add(LoadFileFromString(path));
            }
            return runTestFromStrings(strs.ToArray());
        }

        public ConsoleResults runTestFromUrl(string url)
        {
            using (var client = new WebClient())
            {
                return runTestFromStrings(new string[] { client.DownloadString(url) });
            }
        }

        public ConsoleResults runTestFromStrings(string[] testStrs)
        {
            ConsoleResults results = new ConsoleResults();
            Engine engine = new Engine(cfg => cfg.AllowClr(typeof(MFCSerializer).Assembly));
            if (mJSUrls != null)
            {
                LibraryInfo lib = LoadFilesFromGitHub(this.mJSUrls, engine);
            } else {
                LibraryInfo lib = LoadFiles(this.mJSPath, engine);
            }
            foreach (string testStr in testStrs) { 
                engine.SetValue("consoleResults", results).Execute(testStr);
            }
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
            foreach (string file in Directory.GetFiles(path, "*.js", SearchOption.AllDirectories))
            {
                String fileContents = LoadFileFromString(file);
                int lineCount = fileContents.Split('\n').Length;
                lib.Add(new LibraryFile(file, lineCount, fileContents));
            }

            lib.Sort(new JSLibComparer());

            foreach (LibraryFile file in lib)
            {
                engine.Execute(file.mData);
            }
            return lib;

        }

        private LibraryInfo LoadFilesFromGitHub(string url, Engine engine)
        {

            XmlDocument doc = new XmlDocument();
            WebRequest.DefaultCachePolicy = new System.Net.Cache.RequestCachePolicy(System.Net.Cache.RequestCacheLevel.Revalidate);
            doc.Load(mJSUrls + "?x=123");
            XmlNodeList nodes = doc.SelectNodes("//node()[local-name() = 'ItemGroup']/node()[local-name() = 'Content']/@Include");
            LibraryInfo lib = new LibraryInfo();
            List<String> ghUrls = new List<string>();
            // Save in list to alphabetize
            foreach (XmlNode node in nodes)
            {
                if (node.InnerText != null && node.InnerText.StartsWith("js\\")) {
                    string ghUrl = "https://raw.githubusercontent.com/grannypron/uaf/port/src/UAFLib/" + node.InnerText.Replace("\\", "/");
                        ghUrls.Add(ghUrl);
                }
            }

            ghUrls.Sort();
            using (var client = new WebClient())
            {
                foreach (string ghUrl in ghUrls)
                {
                    String fileContents = client.DownloadString(ghUrl);
                    int lineCount = fileContents.Split('\n').Length;
                    engine.Execute(fileContents);
                    lib.Add(new LibraryFile(ghUrl, lineCount, fileContents));
                }
            }


            return lib;
        }
    }


    public class JSLibComparer : IComparer<LibraryFile>
    {
        public int Compare(LibraryFile f1, LibraryFile f2)
        {
            string name1 = getFile(f1.mName);
            string name2 = getFile(f2.mName);
            return String.Compare(name1.ToLower(), name2.ToLower());
        }

        string getFile(string s)
        {
            if (s.IndexOf("/") > -1) { 
                s = s.Substring(s.LastIndexOf("/") + 1); 
            }
            if (s.IndexOf("\\") > -1) {
                s = s.Substring(s.LastIndexOf("\\") + 1);
            }
            return s;
        }
    }
}
