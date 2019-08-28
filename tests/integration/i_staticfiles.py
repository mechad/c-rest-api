import json
import unittest
import urllib.request as re

server = "http://localhost:8888"


class TestStaticFiles(unittest.TestCase):

    def test_static_html(self):
        req = re.Request(url=f"{server}/tests/html/index.html")
        with re.urlopen(req) as f:
            header = f.info()
            self.assertIn("text/html", str(header))
            data = f.read()
            self.assertNotEqual(data, None)
            self.assertEqual(len(data), 520)

    def test_static_css(self):
        req = re.Request(url=f"{server}/tests/html/style.css")
        with re.urlopen(req) as f:
            header = f.info()
            self.assertIn("text/css", str(header))
            data = f.read()
            self.assertNotEqual(data, None)
            self.assertEqual(len(data), 179)

    def test_static_js(self):
        req = re.Request(url=f"{server}/tests/html/javascript.js")
        with re.urlopen(req) as f:
            header = f.info()
            self.assertIn("text/javascript", str(header))
            data = f.read()
            self.assertNotEqual(data, None)
            self.assertEqual(len(data), 1085)

    def test_rest_html(self):
        req = re.Request(url=f"{server}/")
        with re.urlopen(req) as f:
            header = f.info()
            self.assertIn("text/html", str(header))
            data = f.read()
            self.assertNotEqual(data, None)
            self.assertEqual(len(data), 520)

    def test_rest_css(self):
        req = re.Request(url=f"{server}/css")
        with re.urlopen(req) as f:
            header = f.info()
            self.assertIn("text/css", str(header))
            data = f.read()
            self.assertNotEqual(data, None)
            self.assertEqual(len(data), 179)

    def test_rest_js(self):
        req = re.Request(url=f"{server}/javascript")
        with re.urlopen(req) as f:
            header = f.info()
            self.assertIn("text/javascript", str(header))
            data = f.read()
            self.assertNotEqual(data, None)
            self.assertEqual(len(data), 1085)


if __name__ == '__main__':
    unittest.main()
