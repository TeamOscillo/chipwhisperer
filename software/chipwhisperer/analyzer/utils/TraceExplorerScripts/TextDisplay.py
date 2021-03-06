#!/usr/bin/python
# -*- coding: utf-8 -*-
#
# Copyright (c) 2014, NewAE Technology Inc
# All rights reserved.
#
# Author: Colin O'Flynn
#
# Find this and more at newae.com - this file is part of the chipwhisperer
# project, http://www.assembla.com/spaces/chipwhisperer
#
#    This file is part of chipwhisperer.
#
#    chipwhisperer is free software: you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published by
#    the Free Software Foundation, either version 3 of the License, or
#    (at your option) any later version.
#
#    chipwhisperer is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU Lesser General Public License for more details.
#
#    You should have received a copy of the GNU General Public License
#    along with chipwhisperer.  If not, see <http://www.gnu.org/licenses/>.
#=================================================

from functools import partial
import numpy as np
import copy

from PySide.QtCore import *
from PySide.QtGui import *
import pyqtgraph as pg

from chipwhisperer.common.autoscript import AutoScript
from chipwhisperer.common.utils import list2hexstr

class TextDisplay(AutoScript, QObject):

    def __init__(self, parent):
        super(TextDisplay, self).__init__(parent)
        self.parent = parent
        self.defineName()
        self.addDock()
        self.dock.hide()

    def defineName(self):
        self.name = 'Text Display'

        self.params = [
             {'name':'Update/Display Table', 'type':'action', 'action':self.updateTable},
             ]

    def traceManager(self):
        return self.parent.traceManager()
    #    if self._tmanager is None and self.parent is not None:
    #        self._tmanager = self.parent.traceManager()
#        return self._tmanager

    def updateTable(self):
        self.dock.show()
        tend = self.traceManager().numTrace()
        self.tablewid.setRowCount(tend)
        for tnum in range(0,tend):
            tin = self.traceManager().getTextin(tnum)
            tout = self.traceManager().getTextout(tnum)
            k = self.traceManager().getKnownKey(tnum)

            self.tablewid.setItem(tnum, 0, QTableWidgetItem(list2hexstr(tin)))
            self.tablewid.setItem(tnum, 1, QTableWidgetItem(list2hexstr(tout)))
            self.tablewid.setItem(tnum, 2, QTableWidgetItem(list2hexstr(k)))





    def addDock(self):

        self.tablewid = QTableWidget()

        self.tablewid.setRowCount(0)
        self.tablewid.setColumnCount(4)
        self.tablewid.setHorizontalHeaderLabels(["Text In", "Text Out", "Key", "Trace Data"])
        #self.tablewid.verticalHeader().hide()
        #self.tablewid.setSizePolicy(QSizePolicy.MinimumExpanding, QSizePolicy.MinimumExpanding)

        self.dock = self.parent.addDock(self.tablewid, "Trace Data")

