using System;
using System.IO;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace UAFLibTest
{
    public static class DatabaseDescriptionLoader
    {
        public static void go()
        {
            FileStream outputFS = new FileStream(@"C:\Users\Shadow\Downloads\Full_Release_191031\uaf_00014\TutorialDesign.dsn\Data\spells-descriptions.txt", FileMode.OpenOrCreate, FileAccess.Write);
            StreamWriter outputSW = new StreamWriter(outputFS);
            FileStream fs = new FileStream(@"C:\Users\Shadow\Downloads\Full_Release_191031\testnew\Data\spells.txt", FileMode.Open, FileAccess.Read);
            StreamReader sr = new StreamReader(fs);
            string line = sr.ReadLine();
            while (line != null) {
                if (line.StartsWith("name ="))
                {
                    string nextLine = sr.ReadLine();
                    if (nextLine.StartsWith("description ="))
                    {
                        // Already has a description, so skip it and move to the next
                        outputSW.WriteLine(line);
                        outputSW.WriteLine(nextLine);
                        line = sr.ReadLine();
                        continue;
                    }
                    else { 
                        string name = line.Substring(6).Trim();
                        if (name.Contains("|"))
                        {
                            name = name.Substring(0, name.IndexOf("|"));
                        }
                        string twoDAFile = @"C:\Users\Shadow\Downloads\priest\" + name + ".2da";
                        if (File.Exists(twoDAFile))
                        {
                            FileStream fs2da = new FileStream(twoDAFile, FileMode.Open, FileAccess.Read);
                            StreamReader sr2da = new StreamReader(fs2da);
                            string desc = sr2da.ReadToEnd();
                            string newDesc = "";
                            int crCount = 0;
                            foreach (char c in desc)
                            {
                                if (c == '\n') {
                                    crCount++;
                                    // Skip any CR's after the 10th line because this text description will be wrapped
                                    if (crCount <= 10)
                                    {
                                        newDesc += "/n";
                                    }
                                    else
                                    {
                                        newDesc += " ";
                                    }
                                } else
                                {
                                    newDesc += c;
                                }
                            }
                            sr2da.Close();
                            fs2da.Close();
                            outputSW.WriteLine(line + "\ndescription = " + newDesc);
                            outputSW.WriteLine(nextLine);
                        }
                        else
                        {
                            outputSW.WriteLine(line);
                            outputSW.WriteLine(nextLine);
                        }
                    } 
                } else
                {
                    outputSW.WriteLine(line);
                }
                line = sr.ReadLine();
            }
            outputSW.Flush();
            outputFS.Flush();
            sr.Close();
            fs.Close();
            outputFS.Close();
        }
    }
}
