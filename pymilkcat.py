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

import milkcat_capi
import sys

class ParserOptions:
    ''' The options for Parser '''

    def __init__(self):
        self._options = milkcat_capi.milkcat_parseroptions_t()
        milkcat_capi.milkcat_parseroptions_init(self._options)

    def UseMixedSegmenter(self):
        self._options.word_segmenter = milkcat_capi.MC_SEGMENTER_MIXED
    def UseCRFSegmenter(self):
        self._options.word_segmenter = milkcat_capi.MC_SEGMENTER_CRF
    def UseBigramSegmenter(self):
        self._options.word_segmenter = milkcat_capi.MC_SEGMENTER_BIGRAM

    def UseMixedPOSTagger(self):
        self._options.part_of_speech_tagger = milkcat_capi.MC_POSTAGGER_MIXED
    def UseHMMPOSTagger(self):
        self._options.part_of_speech_tagger = milkcat_capi.MC_POSTAGGER_HMM
    def UseCRFPOSTagger(self):
        self._options.part_of_speech_tagger = milkcat_capi.MC_POSTAGGER_CRF
    def NoPOSTagger(self):
        self._options.part_of_speech_tagger = milkcat_capi.MC_POSTAGGER_NONE

    def UseYamadaParser(self):
        self._options.dependency_parser = milkcat_capi.MC_DEPPARSER_YAMADA
    def UseBeamYamadaParser(self):
        self._options.dependency_parser = milkcat_capi.MC_DEPPARSER_BEAMYAMADA
    def NoDependencyParser(self):
        self._options.dependency_parser = milkcat_capi.MC_DEPPARSER_NONE

    def SetUserDictionary(self, userdict):
        self._options.user_dictionary_path = userdict
    def SetModelPath(self, model_path):
        self._options.model_path = model_path

class Item:
    def __init__(self, it):
        self.word = it.word
        self.part_of_speech_tag = it.part_of_speech_tag
        self.head = it.head
        self.dependency_label = it.dependency_label
        self.is_begin_of_sentence = it.is_begin_of_sentence

class Parser:
    def __init__(self, options = ParserOptions()):
        self._parser = milkcat_capi.milkcat_parser_new(options._options)
        if self._parser == None:
            raise Exception(milkcat_capi.milkcat_last_error())
        self._iterator = milkcat_capi.milkcat_parseriterator_new()

    def Predict(self, text):
        milkcat_capi.milkcat_parser_predict(
            self._parser,
            self._iterator,
            text)
        result = []
        while milkcat_capi.milkcat_parseriterator_next(self._iterator):
            result.append(Item(self._iterator))
        return result

    def Break(self, text):
        prediction = self.Predict(text)
        return [item.word for item in prediction]
