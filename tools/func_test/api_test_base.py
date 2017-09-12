#!/bin/env/python
# -*- coding:utf-8 -*-
import unittest
import os
import sys
import httplib
import time
import json
import random
import md5

CUR_DIR = os.path.split(os.path.realpath(__file__))[0]

class ApiTestBase(unittest.TestCase):
    def setUp(self):
        self.ip = '127.0.0.1'
        self.port = 9876
        self.path = "/slots/friends"
        self.secret = "2x901as"

    def tearDown(self):
        #time.sleep(0.5)
        pass

    def _sign(self, params):
        # timestamp
        params["ts"] = "%d%4d" % (int(time.time() * 1000), random.randint(0, 9999))
        # sign
        sorted_params = sorted(params.items(),key = lambda e:e[0],reverse=False)
        str2sign = "&".join(["%s=%s" % (pair[0], pair[1]) for pair in sorted_params])\
                   + "&%s" % self.secret
        print "Str to sign:[%s]" % str2sign
        m1 = md5.new()
        m1.update(str2sign)
        sign = m1.hexdigest()
        params["sign"] = sign
        
    '''Return: code status body'''
    def _request(self, params = {}, path = None, sign = False):
        if sign:
            self._sign()
        # create uri
        dest = "%s:%d" % (self.ip, self.port)
        if path is None:
            path = self.path
        path += "?" + "&".join(["%s=%s" % (k, v) for k,v in params.items()])
        print "URI:[%s]" % path
        conn = httplib.HTTPConnection(dest)
        conn.request("POST", path)
        rsp = conn.getresponse()
        body = rsp.read()
        ob = None
        try:
            ob = json.loads(body)
        except Exception, e:
            ob = None
        self.assertTrue(ob is not None)
        return ob
