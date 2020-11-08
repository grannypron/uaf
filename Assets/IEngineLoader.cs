using System;
using System.Collections;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Xml;
using UAFLib;

public delegate void InitComplete(ConsoleResults results);
interface IEngineLoader
{
    IEnumerator loadEngine(XmlDocument config, Jint.Engine engine, InitComplete complete);
}

