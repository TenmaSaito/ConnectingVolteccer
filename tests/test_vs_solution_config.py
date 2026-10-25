import sys
from pathlib import Path
import tempfile
import unittest
sys.path.insert(0,str(Path(__file__).resolve().parents[1]/'tools'))
from select_vs_solution_config import read_solution_combinations, select_configuration, SelectionError

SOLUTION = """Microsoft Visual Studio Solution File, Format Version 12.00
Global
 GlobalSection(SolutionConfigurationPlatforms) = preSolution
   Debug|x64 = Debug|x64
   Debug|x86 = Debug|x86
   Release|x64 = Release|x64
   Release|x86 = Release|x86
 EndGlobalSection
 GlobalSection(ProjectConfigurationPlatforms) = postSolution
   {123}.Release|x86.ActiveCfg = Release|Win32
 EndGlobalSection
EndGlobal
"""

class SolutionConfigurationTests(unittest.TestCase):
    def setUp(self):
        self.temp = tempfile.TemporaryDirectory()
        self.path = Path(self.temp.name) / 'c++TKG.sln'
        self.path.write_text(SOLUTION, encoding='utf-8')
    def tearDown(self):
        self.temp.cleanup()
    def test_reads_only_solution_platforms(self):
        self.assertEqual(read_solution_combinations(self.path), ['Debug|x64','Debug|x86','Release|x64','Release|x86'])
    def test_auto_selects_release_x86(self):
        self.assertEqual(select_configuration(read_solution_combinations(self.path)), ('Release','x86'))
    def test_explicit_x64(self):
        self.assertEqual(select_configuration(read_solution_combinations(self.path), 'Debug', 'x64'), ('Debug','x64'))
    def test_win32_provides_helpful_error(self):
        with self.assertRaisesRegex(SelectionError, r'Release\|Win32.*Release\|x86'):
            select_configuration(read_solution_combinations(self.path), 'Release', 'Win32')
    def test_missing_configuration(self):
        with self.assertRaises(SelectionError):
            select_configuration(read_solution_combinations(self.path), 'Production', '')
    def test_fallback_to_x64(self):
        self.assertEqual(select_configuration(['Release|x64']), ('Release','x64'))

if __name__ == '__main__':
    unittest.main()
