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
            Engine engine = new Engine(cfg => cfg.AllowClr(typeof(MFCSerializer).Assembly));
            LibraryInfo lib = LoadFiles(@"..\..\..\UAFLib\js\", engine);
            try
            {
                engine.Execute(LoadFileFromString(@"..\..\..\UAFLib\Tests\TestCharacterSerialize.js"));
                //engine.Execute(LoadFileFromString(@"..\..\..\UAFLib\Tests\TestUtilHex.js"));
                //engine.Execute(LoadFileFromString(@"..\..\..\UAFLib\Tests\TestStripFilenamePath.js"));
            } catch (JavaScriptException ex)
            {
                System.Console.WriteLine(ex.Message);
                //Object[] errData = lib.getErrorFileAndLine(ex.Location.Start.Line);
                //Console.WriteLine("File: " + errData[0] + "  Line: " + errData[1]);  // not working atm
                System.Console.WriteLine("  Line: " + ex.Location.Start.Line);
                System.Console.WriteLine(ex.Message);
                System.Console.WriteLine(ex.CallStack);
            }

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
                System.Console.WriteLine(file + ":" + lineCount);
            }
            return lib;
        }

    }
}
