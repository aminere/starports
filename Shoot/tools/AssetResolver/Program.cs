using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Xml;
using System.IO;

namespace AssetResolver
{
    class Program
    {
        class Resolver
        {
            private Dictionary<string, string> m_FileMap = new Dictionary<string, string>();
            private System.IO.StreamWriter m_LogFile = new System.IO.StreamWriter("AssetResolverLog.txt");

            public Resolver()
            {
            }

            public void Resolve()
            {
                ResolveDirectory("data");
                m_LogFile.Flush();
            }

            private void ResolveDirectory(string dirPath)
            {
                var subdirs = Directory.GetDirectories(dirPath);
                var files = Directory.GetFiles(dirPath);

                foreach(var file in files)
                    ResolveFile(file);

                foreach(var subdir in subdirs)
                    ResolveDirectory(subdir);                
            }

            private void ResolveFile(string path)
            {
                var ext = Path.GetExtension(path);
                if (ext != ".xml")
                    return;

                XmlDocument doc = new XmlDocument();
                doc.Load(path);
                if (ResolveNode(doc.FirstChild, path))
                {
                    string outpath = path.Replace("data", "data_resolved");
                    string outDir = Path.GetDirectoryName(outpath);
                    if (!Directory.Exists(outDir))
                        Directory.CreateDirectory(outDir);
                    doc.Save(outpath);
                }
            }

            private bool ResolveNode(XmlNode node, string currentFile)
            {
                var changed = false;
                var templatePathAttr = node.Attributes["TemplatePath"];
                if(templatePathAttr != null)
                {
                    var templatePath = "data/"+templatePathAttr.Value;
                    if (templatePath != "data/" && !File.Exists(templatePath))
                    {
                        var newFile = FindEquivalentFile(templatePath);
                        if (newFile != null)
                        {
                            templatePathAttr.Value = newFile;
                            changed = true;
                        }
                        else
                        {
                            m_LogFile.WriteLine("Couldn't resolve '" + templatePath + "' for '" + currentFile + "'");
                        }
                    }
                }
                else if(node.Name == "file")
                {
                    var pathValue = "data/" + node.Attributes["Value"].Value;
                    if (pathValue != "data/" && !File.Exists(pathValue))
                    {
                        var newFile = FindEquivalentFile(pathValue);
                        if (newFile != null)
                        {
                            node.Attributes["Value"].Value = newFile;
                            changed = true;
                        }
                        else
                        {
                            m_LogFile.WriteLine("Couldn't resolve '" + pathValue + "' for '" + currentFile + "'");
                        }
                    }
                }

                for (var i = 0; i < node.ChildNodes.Count; ++i)
                {
                    if (ResolveNode(node.ChildNodes[i], currentFile))
                        changed = true;
                }

                return changed;
            }

            private string FindEquivalentFile(string path)
            {
                var fileName = Path.GetFileName(path);
                var file = FindFile("data", fileName);
                return file;                
            }

            private string FindFile(string dir, string fileName)
            {
                if (m_FileMap.ContainsKey(fileName))
                    return m_FileMap[fileName];

                var files = Directory.GetFiles(dir);
                var file = files.FirstOrDefault(f => Path.GetFileName(f).Equals(fileName, StringComparison.InvariantCultureIgnoreCase));
                if(file != null)
                {
                    file = file.Replace('\\', '/');
                    file = file.Replace("data/", "");
                    m_FileMap[fileName] = file;
                    return file;
                }

                var dirs = Directory.GetDirectories(dir);
                foreach(var subdir in dirs)
                {
                    var subFile = FindFile(subdir, fileName);
                    if (subFile != null)
                        return subFile;
                }                

                return null;
            }
        }

        static void Main(string[] args)
        {
            var resolver = new Resolver();
            resolver.Resolve();
        }
    }
}
