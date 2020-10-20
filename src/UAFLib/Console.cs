using Jint;
using Jint.Runtime;
using System;
using System.Collections.Generic;
using System.IO;

namespace UAFLib
{
    public class Console
    {
        
        public void helloWorld()
        {
            try
            {
                //**TODO: go back through the C++ source and find all instances of "delete" and re-insert them back with calls to a MemoryManager() class.  Apparently jint responds to delete statements, although all engines might not
                runTest(@"..\..\..\UAFLib\Tests\TestUtilHex.js");
                runTest(@"..\..\..\UAFLib\Tests\TestStripFilenamePath.js");
                runTest(@"..\..\..\UAFLib\Tests\TestCharacterSerialize.js");
                runTest(@"..\..\..\UAFLib\Tests\TestTagList.js");
                runTest(@"..\..\..\UAFLib\Tests\TestSimpleCombat.js");
                // Next: make a test case where the monsters get added - see COMBAT_DATA.prototype.AddMonstersToCombatants and implement COMBAT_DATA.prototype.determineInitCombatPos
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

        private void runTest(string path)
        {
            Engine engine = new Engine(cfg => cfg.AllowClr(typeof(MFCSerializer).Assembly));
            LibraryInfo lib = LoadFiles(@"..\..\..\UAFLib\js\", engine);
            engine.Execute(LoadFileFromString(path));
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

    }
}
