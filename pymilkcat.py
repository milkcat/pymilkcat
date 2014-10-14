# -*- coding: utf-8 -*-
#
# The MIT License (MIT)
#
# Copyright 2013-2014 The MilkCat Project Developers
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.
#
# pymilkcat.py --- Created at 2014-02-20
#

import _pymilkcat


CRF_SEGMENTER = 1
UNIGRAM_SEGMENTER = 2
BIGRAM_SEGMENTER = 3
MIXED_SEGMENTER = 4

HMM_POSTAGGER = 1
CRF_POSTAGGER = 2
MIXED_POSTAGGER = 3
NO_POSTAGGER = 0

class Parser:
    '''
    Chinese word segmenter, part-of-speech tagger and dependency parser.

    Example:
        import pymilkcat
        parser = pymilkcat.Parser()
        parser.pos_tag('今天天气不错')
    '''

    def __init__(self, 
                 segmenter = MIXED_SEGMENTER, 
                 tagger = MIXED_POSTAGGER,
                 userdict_path = None, 
                 model_dir = None):
        if model_dir != None and model_dir[-1] != '/':
            model_dir += '/'
        self._model = _pymilkcat.Model(model_dir)
        if userdict_path != None:
            r = self._model.set_userdict(userdict_path)
            if r == False:
                raise Exception('Unable to load user dictionary: ' + userdict_path)
        self._parser = _pymilkcat.Parser(self._model, segmenter, tagger)

    def seg(self, text):
        return list(map(lambda x: x[0], self._parser.parse(text)))

    def pos_tag(self, text):
        return list(map(lambda x: (x[0], x[1]), self._parser.parse(text)))


