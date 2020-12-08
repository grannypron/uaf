using Jint;
using Jint.Runtime;
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Net;
using System.Xml;
using NUnit.Framework;
using UAFLib.serialization;

namespace UAFLib
{
    public class TestRunner
    {
        public string mJSPath = Path.GetFullPath(typeof(TestRunner).Assembly.Location.ToString() + "\\..\\..\\..\\js");
        public string mJSUrls = null;//"https://raw.githubusercontent.com/grannypron/uaf/port/src/UAFLib/UAFLib.csproj";//null;
        string startupPath = Path.GetFullPath(typeof(TestRunner).Assembly.Location.ToString() + "\\..\\..\\..");

        [TestCase]
        public void TestParseDice()
        {
            //throw new Exception(startupPath);
            //loadAndSerializeObjects();  // Used to make bin versions of lookup collections
            runTest(new string[] { startupPath + @"\Tests\TestParseDice.js" });
        }
        
        [TestCase]
        public void TestLoadItems()
        {
            runTest(new string[] { startupPath + @"\Tests\TestLoadItems.js" });
        }

        [TestCase]
        public void TestLoadSAs()
        {
            runTest(new string[] { startupPath + @"\Tests\TestLoadSAs.js" });
        }

        [TestCase]
        public void TestUtilHex()
        {
            runTest(new string[] { startupPath + @"\Tests\TestUtilHex.js" });
        }
        [TestCase]
        public void TestEntityType()
        {
            runTest(new string[] { startupPath + @"\Tests\TestEntityType.js" });
        }
        [TestCase]
        public void TestStripFilenamePath()
        {
            runTest(new string[] { startupPath + @"\Tests\TestStripFilenamePath.js" });
        }
        [TestCase]
        public void TestCharacterSerialize()
        {
            runTest(new string[] { startupPath + @"\Tests\TestCharacterSerialize.js" });
        }
        [TestCase]
        public void TestTagList()
        {
            runTest(new string[] { startupPath + @"\Tests\TestTagList.js" });
        }
        [TestCase]
        public void TestPathManager()
        {
            runTest(new string[] { startupPath + @"\Tests\TestPathManager.js" });
        }
        [TestCase]
        public void TestRollDice()
        {
            runTest(new string[] { startupPath + @"\Tests\TestRollDice.js" });
        }
        [TestCase]
        public void TestGlobalStats()
        {
            runTest(new string[] { startupPath + @"\Tests\TestGlobalStats.js" });
        }
        [TestCase]
        public void TestSetupCombat()
        {
            runTest(new string[] { startupPath + @"\Tests\TestSetupCombat.js" });
        }
        [TestCase]
        public void TestSimpleCombatMovement()
        {
            runTest(new string[] { startupPath + @"\Tests\TestSetupCombat.js", startupPath + @"\Tests\TestSimpleCombatMovement.js" });
        }
        [TestCase]
        public void TestFreeAttacks()
        {
            runTest(new string[] { startupPath + @"\Tests\TestFreeAttacks.js" });
        }
        [TestCase]
        public void TestCombatMovementAttack()
        {
            runTest(new string[] { startupPath + @"\Tests\TestSetupCombat.js", startupPath + @"\Tests\TestCombatMovementAttack.js" });
        }
        [TestCase]
        public void TestSimpleUnarmedAttack()
        {
            runTest(new string[] { startupPath + @"\Tests\TestSetupCombat.js", startupPath + @"\Tests\TestCombatMovementAttack.js", startupPath + @"\Tests\TestSimpleUnarmedAttack.js" });
        }
        [TestCase]
        public void TestKill()
        {
            runTest(new string[] { startupPath + @"\Tests\TestSetupCombat.js", startupPath + @"\Tests\TestCombatMovementAttack.js", startupPath + @"\Tests\TestSimpleUnarmedAttack.js", startupPath + @"\Tests\TestKill.js" });
        }
        [TestCase]
        public void TestSimpleAttack()
        {
            runTest(new string[] { startupPath + @"\Tests\TestSetupCombat.js", startupPath + @"\Tests\TestCombatMovementAttack.js", startupPath + @"\Tests\TestSimpleAttack.js" });
        }
        [TestCase]
        public void TestMultiRoundCombat()
        {
            runTest(new string[] { startupPath + @"\Tests\TestSetupCombat.js", startupPath + @"\Tests\TestMultiRoundCombat.js" });
        }
        [TestCase]
        public void TestMonsterAttack()
        {
            runTest(new string[] { startupPath + @"\Tests\TestSetupCombat.js", startupPath + @"\Tests\TestMonsterAttack.js" });
        }
        [TestCase]
        public void TestWinCombat()
        {
            runTest(new string[] { startupPath + @"\Tests\TestSetupCombat.js", startupPath + @"\Tests\TestWinCombat.js" });
        }
        [TestCase]
        public void TestRangedAttack()
        {
            runTest(new string[] { startupPath + @"\Tests\TestSetupCombat.js", startupPath + @"\Tests\TestRangedAttack.js" });
        }
        [TestCase]
        public void TestThrownWeaponAttack()
        {
            runTest(new string[] { startupPath + @"\Tests\TestSetupCombat.js", startupPath + @"\Tests\TestThrownWeaponAttack.js" });
        }
        [TestCase]
        public void TestExperience()
        {
            runTest(new string[] { startupPath + @"\Tests\TestExperience.js" });
        }

        [TestCase]
        public void TestSpecialAbilities()
        {
            runTest(new string[] { startupPath + @"\Tests\TestLoadSAs.js", startupPath + @"\Tests\TestSpecialAbilities.js" });
        }

        [TestCase]
        public void TestInventory()
        {
            runTest(new string[] { startupPath + @"\Tests\TestLoadItems.js", startupPath + @"\Tests\TestInventory.js" });
        }

        [TestCase]
        public void TestReadyBest()
        {
            runTest(new string[] { startupPath + @"\Tests\TestLoadItems.js", startupPath + @"\Tests\TestReadyBest.js" });
        }

        public ConsoleResults runTest(string[] paths)
        {
            List<string> strs = new List<string>();
            for (int idx = 0; idx < paths.Length; idx++) {
                string path = paths[idx];
                strs.Add(LoadFileContents(path));
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

        private string LoadFileContents(string path)
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
                String fileContents = LoadFileContents(file);
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


        private void loadAndSerializeObjects()
        {
            UAFLib.dataLoaders.ItemLoader loader = new UAFLib.dataLoaders.ItemLoader();
            Dictionary<string, object> loadedData = loader.load("C:\\Users\\Shadow\\Desktop\\uaf.git\\uaf-port\\src\\UAFLib\\data\\items.xml");
            Dictionary<string, Dictionary<string, List<string>>> castLoadedData = new Dictionary<string, Dictionary<string, List<string>>>();
            foreach (string key in loadedData.Keys)
            {
                castLoadedData[key] = (Dictionary<string, List<string>>)loadedData[key];
            }

            using (var file = File.Create("items.bin"))
            {
                ProtoBuf.Serializer.Serialize(file, castLoadedData);
            }
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
