using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Xml;
using System.IO;

namespace TextureRefactorHelper
{
    class Resolver
    {        
        private System.IO.StreamWriter m_LogFile = new System.IO.StreamWriter("TextureRefactorHelper.txt");

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

            foreach (var file in files)
                ResolveFile(file);

            foreach (var subdir in subdirs)
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
            // Resolve CreationInfo structure
            //var isStruct = node.Name == "struct";
            //if(isStruct)
            //{
            //    var nodeNameAttr = node.Attributes["Name"];
            //    var nodeName = nodeNameAttr != null ? nodeNameAttr.Value : "";
            //    if (nodeName == "CreationInfo")
            //    {
            //        for (var i = 0; i < node.ChildNodes.Count; ++i)
            //        {
            //            var subNode = node.ChildNodes[i] as XmlElement;
            //            var subNodeName = subNode.Attributes["Name"].Value;
            //            if (subNodeName == "Textures")
            //            {
            //                var newNodes = new List<XmlNode>();
            //                for (var j = 0; j < subNode.ChildNodes.Count; ++j)
            //                {
            //                    var structNode = subNode.ChildNodes[j];
            //                    var fileNode = structNode.ChildNodes[0];
            //                    XmlNode mipMapNode = structNode.ChildNodes.Count > 1 ? structNode.ChildNodes[1] : null;
            //                    XmlNode _32BitsNode = structNode.ChildNodes.Count > 2 ? structNode.ChildNodes[2] : null;

            //                    var newNode = node.OwnerDocument.CreateElement("ref");
            //                    newNodes.Add(newNode);

            //                    var path = fileNode.Attributes["Value"].Value;
            //                    if(path.Length > 0)
            //                    {
            //                        var mipMaps = mipMapNode != null ? mipMapNode.Attributes["Value"].Value == "1" : true;
            //                        var _32Bits = _32BitsNode != null ? _32BitsNode.Attributes["Value"].Value == "1" : false;
            //                        var xmlFileName = Path.GetFileNameWithoutExtension(path) + ".xml";
            //                        var xmlPath = Path.GetDirectoryName(path) + "\\" + xmlFileName;
            //                        newNode.SetAttribute("TemplatePath", xmlPath.Replace('\\', '/'));

            //                        if (!File.Exists("data\\"+xmlPath))
            //                        {
            //                            var newDoc = new XmlDocument();
            //                            var rootNode = newDoc.CreateElement("Texture");
            //                            var newMipMapNode = newDoc.CreateElement("bool");
            //                            var new32BitsNode = newDoc.CreateElement("bool");
            //                            newMipMapNode.SetAttribute("Name", "MipMaps");
            //                            newMipMapNode.SetAttribute("Value", mipMaps ? "1" : "0");
            //                            new32BitsNode.SetAttribute("Name", "32Bits");
            //                            new32BitsNode.SetAttribute("Value", _32Bits ? "1" : "0");
            //                            rootNode.AppendChild(newMipMapNode);
            //                            rootNode.AppendChild(new32BitsNode);
            //                            newDoc.AppendChild(rootNode);
            //                            newDoc.Save("data\\"+xmlPath);
            //                        }
            //                        else
            //                        {
            //                            // ? force mipmaps to true?
            //                        }
            //                    }                               
            //                }

            //                subNode.RemoveAll();
            //                subNode.SetAttribute("Name", "Textures");                           
            //                foreach (var newNode in newNodes)
            //                    subNode.AppendChild(newNode);                            
            //            }
            //        }
            //        return true;
            //    }
            //}           

            // Resolve references to a Texture
            //var changed = false;
            //var templateAttr = node.Attributes["TemplatePath"];
            //if(templateAttr != null)
            //{
            //    var templatePath = templateAttr.Value;
            //    if(templatePath.Contains("png"))
            //    {
            //        var mipMaps = false;
            //        var _32Bits = false;
            //        var xmlFileName = Path.GetFileNameWithoutExtension(templatePath) + ".xml";
            //        var xmlPath = Path.GetDirectoryName(templatePath) + "\\" + xmlFileName;
            //        (node as XmlElement).SetAttribute("TemplatePath", xmlPath.Replace('\\', '/'));

            //        if (!File.Exists("data\\" + xmlPath))
            //        {
            //            var newDoc = new XmlDocument();
            //            var rootNode = newDoc.CreateElement("Texture");
            //            var newMipMapNode = newDoc.CreateElement("bool");
            //            var new32BitsNode = newDoc.CreateElement("bool");
            //            newMipMapNode.SetAttribute("Name", "MipMaps");
            //            newMipMapNode.SetAttribute("Value", mipMaps ? "1" : "0");
            //            new32BitsNode.SetAttribute("Name", "32Bits");
            //            new32BitsNode.SetAttribute("Value", _32Bits ? "1" : "0");
            //            rootNode.AppendChild(newMipMapNode);
            //            rootNode.AppendChild(new32BitsNode);
            //            newDoc.AppendChild(rootNode);
            //            newDoc.Save("data\\" + xmlPath);
            //        }

            //        changed = true;
            //    }
            //}

            // Resolve paths to a png
            var changed = false;
            var nameAttr = node.Attributes["Name"];
            if(nameAttr != null)
            {
                var name = nameAttr.Value;
                if (name == "Icon" && node.Name == "file")
                {
                    var path = node.Attributes["Value"].Value;
                    if(path.Contains("png"))
                    {
                        var mipMaps = false;
                        var _32Bits = false;
                        var xmlFileName = Path.GetFileNameWithoutExtension(path) + ".xml";
                        var xmlPath = Path.GetDirectoryName(path) + "\\" + xmlFileName;
                        (node as XmlElement).SetAttribute("Value", xmlPath.Replace('\\', '/'));

                        if (!File.Exists("data\\" + xmlPath))
                        {
                            var newDoc = new XmlDocument();
                            var rootNode = newDoc.CreateElement("Texture");
                            var newMipMapNode = newDoc.CreateElement("bool");
                            var new32BitsNode = newDoc.CreateElement("bool");
                            newMipMapNode.SetAttribute("Name", "MipMaps");
                            newMipMapNode.SetAttribute("Value", mipMaps ? "1" : "0");
                            new32BitsNode.SetAttribute("Name", "32Bits");
                            new32BitsNode.SetAttribute("Value", _32Bits ? "1" : "0");
                            rootNode.AppendChild(newMipMapNode);
                            rootNode.AppendChild(new32BitsNode);
                            newDoc.AppendChild(rootNode);
                            newDoc.Save("data\\" + xmlPath);
                        }
                        
                        changed = true;
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
    }

    class Program
    {
        static void Main(string[] args)
        {
            var resolver = new Resolver();
            resolver.Resolve();
        }
    }
}
