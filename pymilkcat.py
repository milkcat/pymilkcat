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

'''
A python interface for MilkCat - A Chinese morphological analyzer

Example:
    import pymilkcat
    print(pymilkcat.seg('今天天气不错'))
'''

import _pymilkcat

_seg_analyzer = None
_pos_tag_analyzer = None


DEFAULT_ANALYZER = 0
CRF_SEGMENTER = 1
CRF_ANALYZER = 2
DEFAULT_SEGMENTER = 3
BIGRAM_SEGMENTER = 4

def seg(text):
    '''
    Segment the text into a list of Chinese words. The first time call this
    function will be slower since it needs to load the model from disk
    '''

    global _seg_analyzer

    if _seg_analyzer == None:
        _seg_analyzer = MilkCat(DEFAULT_SEGMENTER)
    return _seg_analyzer.seg(text)

def pos_tag(text):
    '''
    Segment and get the part-of-speech tag of each word from text, return a list
    of (word, part_of_speech_tag) tuples. The first time call this function will
    be slower since it needs to load the model from disk
    '''

    global _pos_tag_analyzer

    if _pos_tag_analyzer == None:
        _pos_tag_analyzer = MilkCat(DEFAULT_ANALYZER)
    return _pos_tag_analyzer.pos_tag(text)

class MilkCat:
    '''
    MilkCat is a Chinese morphological analyzer, it can segment chinese text
    into words and get the part-of-speech tag of word or the word types.

    Example:
        import pymilkcat
        analyzer = pymilkcat.MilkCat()
        analyzer.pos_tag('今天天气不错')
    '''

    def __init__(self, 
                 analyzer_type = DEFAULT_ANALYZER, 
                 userdict_path = None, 
                 model_dir = None):
        '''
        analyzer_type: The type of analyzer would be create, it could be
            DEFAULT_ANALYZER   - The default word segmenter and POS tagger
            CRF_SEGMENTER      - The CRF model based word segmenter
            CRF_ANALYZER       - The CRF model based word segmenter and POS 
                                 tagger
            DEFAULT_SEGMENTER  - The default word segmenter
            BIGRAM_SEGMENTER   - The bigram model based word segmenter
        userdict_path: The path of user dictionary for word segmenter
        model_dir: The path of MilkCat model directory, set None to use the 
                   default model
        '''

        self._model = _pymilkcat.Model(model_dir)
        if userdict_path != None:
            self._model.set_userdict(userdict_path)
        self._analyzer = _pymilkcat.MilkCat(self._model, analyzer_type)

    def seg(self, text):
        '''
        Segment the text into a list of Chinese words
        '''
        return self._analyzer.analyze(text, pos_tag = False, word_type = False)

    def pos_tag(self, text):
        '''
        Get each word and corresponding Part-Of-Speech tag from text, one word 
        and its POS tag is a tuple (word, tag), and return a list of tuples.
        '''
        return self._analyzer.analyze(text, pos_tag = True, word_type = False)

    def analyze(self, text, word = True, pos_tag = True, word_type = True):
        '''
        Analyze the text and get the word or pos_tag or word_type specified by
        parameter.
        '''
        return self._analyzer.analyze(
            text, 
            word = word,
            pos_tag = pos_tag, 
            word_type = word_type)
