using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.IO;

namespace AssetDuplicateChecker
{
    class DuplicateChecker
    {
        private Dictionary<string, string> m_FileMap = new Dictionary<string, string>();
        private System.IO.StreamWriter m_LogFile = new System.IO.StreamWriter("DuplicateCheckerLog.txt");

        public DuplicateChecker()
        {
        }

        public void Check()
        {
            CheckDirectory("data");
            m_LogFile.Flush();
        }

        private void CheckDirectory(string dir)
        {
            var files = Directory.GetFiles(dir);
            foreach(var file in files)
            {
                var fileName = Path.GetFileName(file).ToLower();
                if (m_FileMap.ContainsKey(fileName))
                    m_LogFile.WriteLine("File '" + file + "' duplicates '" + m_FileMap[fileName]);
                else
                    m_FileMap[fileName] = file;
            }

            var subDirs = Directory.GetDirectories(dir);
            foreach (var subDir in subDirs)
                CheckDirectory(subDir);
        }
    }

    class Program
    {
        static void Main(string[] args)
        {
            var checker = new DuplicateChecker();
            checker.Check();
        }
    }
}
