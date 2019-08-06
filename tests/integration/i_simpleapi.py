import json
import unittest
import urllib.request as re

server = "http://localhost:8888"

class TestSimpleCallback(unittest.TestCase):

    def test_simple(self):
        req = re.Request(url=f"{server}/", method='GET')
        with re.urlopen(req) as f:
            data = f.read()
            self.assertNotEqual(data, None)
            j = json.loads(data)
            self.assertEqual(j['test'], 'callback')

    def test_parameter(self):
        req = re.Request(url=f"{server}/parameter/1")
        with re.urlopen(req) as f:
            data = f.read()
            self.assertNotEqual(data, None)
            self.assertNotIn('error', str(data))
            j = json.loads(data)
            self.assertEqual(j['result'], '1')
        req = re.Request(url=f"{server}/parameter/2")
        with re.urlopen(req) as f:
            data = f.read()
            self.assertNotEqual(data, None)
            self.assertNotIn('error', str(data))
            j = json.loads(data)
            self.assertEqual(j['result'], '2')
        req = re.Request(url=f"{server}/parameter/3")
        with re.urlopen(req) as f:
            data = f.read()
            self.assertNotEqual(data, None)
            self.assertNotIn('error', str(data))
            j = json.loads(data)
            self.assertEqual(j['result'], 'not found')

    def test_data(self):
        req = re.Request(url=f"{server}/api", method='POST', data=b'{"tdata": "test1"}')
        with re.urlopen(req) as f:
            data = f.read()
            self.assertNotEqual(data, None)
            self.assertNotIn('error', str(data))
            j = json.loads(data)
            self.assertEqual(j['result'], 'test1')
        req = re.Request(url=f"{server}/api", method='POST', data=b'{"tdata": "test2"}')
        with re.urlopen(req) as f:
            data = f.read()
            self.assertNotEqual(data, None)
            self.assertNotIn('error', str(data))
            j = json.loads(data)
            self.assertEqual(j['result'], 'test2')
        req = re.Request(url=f"{server}/api", method='POST', data=b'{"tdata": "test3"}')
        with re.urlopen(req) as f:
            data = f.read()
            self.assertNotEqual(data, None)
            self.assertNotIn('error', str(data))
            j = json.loads(data)
            self.assertEqual(j['result'], 'not found')



if __name__ == '__main__':
        unittest.main()
