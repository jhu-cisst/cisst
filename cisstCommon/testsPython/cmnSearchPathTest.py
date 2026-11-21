import pathlib
import tempfile
import unittest

from cisstCommonPython import cmnSearchPath


class SearchPathTest(unittest.TestCase):
    def TestFilesystemPaths(self):
        with tempfile.TemporaryDirectory() as directory:
            root = pathlib.Path(directory)
            first = root / "first"
            second = root / "second"
            first.mkdir()
            second.mkdir()
            (first / "input.json").write_text("first")
            (second / "input.json").write_text("second")

            search = cmnSearchPath(first)
            self.assertTrue(search.Has(first))
            self.assertEqual(search.Find(pathlib.Path("input.json")), first / "input.json")
            search.Add(second)
            self.assertEqual(search.Find("input.json"), second / "input.json")
            self.assertTrue(search.Remove(second))
            search.Add(second, cmnSearchPath.Position.TAIL)
            self.assertEqual(search.Find("input.json"), first / "input.json")
            search.Set(str(second))
            self.assertFalse(search.Has(first))
            self.assertEqual(search.Find("input.json"), second / "input.json")
