#!/usr/bin/env python
# -*- coding: utf-8 -*-
# -*- Python -*-

"""
 @file ReceptionCommander.py
 @brief ModuleDescription
 @date $Date$


"""
import OpenRTM_aist
import RTC
import sys
import time
sys.path.append(".")

# Import RTM module


# Import Service implementation class
# <rtc-template block="service_impl">

# </rtc-template>

# Import Service stub modules
# <rtc-template block="consumer_import">
# </rtc-template>


# This module's spesification
# <rtc-template block="module_spec">
receptioncommander_spec = ["implementation_id", "ReceptionCommander",
                           "type_name",         "ReceptionCommander",
                           "description",       "ModuleDescription",
                           "version",           "1.0.0",
                           "vendor",            "tajimanina",
                           "category",          "Category",
                           "activity_type",     "STATIC",
                           "max_instance",      "1",
                           "language",          "Python",
                           "lang_type",         "SCRIPT",
                           ""]
# </rtc-template>

##
# @class ReceptionCommander
# @brief ModuleDescription
#
#


class ReceptionCommander(OpenRTM_aist.DataFlowComponentBase):

    ##
    # @brief constructor
    # @param manager Maneger Object
    #
    def __init__(self, manager):
        OpenRTM_aist.DataFlowComponentBase.__init__(self, manager)

        self._d_human_x = RTC.TimedLongSeq(RTC.Time(0, 0), [])
        """
				"""
        self._human_xIn = OpenRTM_aist.InPort("human_x", self._d_human_x)
        self._d_human_y = RTC.TimedLongSeq(RTC.Time(0, 0), [])
        """
				"""
        self._human_yIn = OpenRTM_aist.InPort("human_y", self._d_human_y)
        self._d_wait_people = RTC.TimedLong(RTC.Time(0, 0), 0)
        """
				"""
        self._wait_peopleIn = OpenRTM_aist.InPort(
            "wait_people", self._d_wait_people)
        self._d_human_edge1X = RTC.TimedLongSeq(RTC.Time(0, 0), [])
        """
				"""
        self._human_edge1XIn = OpenRTM_aist.InPort(
            "human_edge1X", self._d_human_edge1X)
        self._d_human_edge1Y = RTC.TimedLongSeq(RTC.Time(0, 0), [])
        """
				"""
        self._human_edge1YIn = OpenRTM_aist.InPort(
            "human_edge1Y", self._d_human_edge1Y)
        self._d_human_edge2X = RTC.TimedLongSeq(RTC.Time(0, 0), [])
        """
				"""
        self._human_edge2XIn = OpenRTM_aist.InPort(
            "human_edge2X", self._d_human_edge2X)
        self._d_human_edge2Y = RTC.TimedLongSeq(RTC.Time(0, 0), [])
        """
				"""
        self._human_edge2YIn = OpenRTM_aist.InPort(
            "human_edge2Y", self._d_human_edge2Y)
        self._d_logger_cmd = RTC.TimedString(RTC.Time(0, 0), "")
        """
				"""
        self._logger_cmdOut = OpenRTM_aist.OutPort(
            "logger_cmd", self._d_logger_cmd)
        self._d_gauss_cmd = RTC.TimedString(RTC.Time(0, 0), "")
        """
				"""
        self._gauss_cmdOut = OpenRTM_aist.OutPort(
            "gauss_cmd", self._d_gauss_cmd)
        self._d_human_num = RTC.TimedLong(RTC.Time(0, 0), 0)
        """
				"""
        self._human_numOut = OpenRTM_aist.OutPort(
            "human_num", self._d_human_num)
        self._d_poco_cmd = RTC.TimedString(RTC.Time(0, 0), "")
        """
				"""
        self._poco_cmdOut = OpenRTM_aist.OutPort("poco_cmd", self._d_poco_cmd)

    def onInitialize(self):
        # Bind variables and configuration variable

        # Set InPort buffers
        self.addInPort("human_x", self._human_xIn)
        self.addInPort("human_y", self._human_yIn)
        self.addInPort("human_edge1X", self._human_edge1XIn)
        self.addInPort("human_edge1Y", self._human_edge1YIn)
        self.addInPort("human_edge2X", self._human_edge2XIn)
        self.addInPort("human_edge2Y", self._human_edge2YIn)
        self.addInPort("wait_people", self._wait_peopleIn)

        # Set OutPort buffers
        self.addOutPort("logger_cmd", self._logger_cmdOut)
        self.addOutPort("gauss_cmd", self._gauss_cmdOut)
        self.addOutPort("human_num", self._human_numOut)
        self.addOutPort("poco_cmd", self._poco_cmdOut)

        # Set service provider to Ports

        # Set service consumers to Ports

        # Set CORBA Service Ports
        self._human_x = []
        self._human_y = []
        self._human_edge1X = []
        self._human_edge1Y = []
        self._human_edge2X = []
        self._human_edge2Y = []
        self._human_max = 0

        return RTC.RTC_OK

    def onActivated(self, ec_id):
        self._flag1 = "write_start"
        self._flag2 = "write_stop"
        self._flag3 = "g_start"
        self._flag4 = "g_stop"
        self._flag5 = "poco_speak"

        return RTC.RTC_OK

    def onDeactivated(self, ec_id):

        return RTC.RTC_OK

    def onExecute(self, ec_id):
        if (self._human_xIn.isNew() & self._human_yIn.isNew()):
            # ポート読み込み
            self._d_human_x = self._human_xIn.read()
            self._human_x = self._d_human_x.data
            self._d_human_y = self._human_yIn.read()
            self._human_y = self._d_human_y.data
            # print("humanX:", self._human_x)
            # print("humanY:", self._human_y)
            # edgeポート読み込み
            self._d_human_edge1X = self._human_edge1XIn.read()
            self._human_edge1X = self._d_human_edge1X.data
            self._d_human_edge1Y = self._human_edge1YIn.read()
            self._human_edge1Y = self._d_human_edge1Y.data
            # print("human_edge1X:", self._human_edge1X)
            # print("human_edge1Y:", self._human_edge1Y)
            self._d_human_edge2X = self._human_edge2XIn.read()
            self._human_edge2X = self._d_human_edge2X.data
            self._d_human_edge2Y = self._human_edge2YIn.read()
            self._human_edge2Y = self._d_human_edge2Y.data
            # print("human_edge2X:", self._human_edge2X)
            # print("human_edge2Y:", self._human_edge2Y)
            # 人数読み込み
            self._d_wait_people = self._wait_peopleIn.read()
            self._wait_people = self._d_wait_people.data
            # print("wait_people:", self._wait_people)

            if (any((x < 0 for x in self._human_x))):
                self._d_logger_cmd.data = self._flag1
                self._logger_cmdOut.write()
                print("logger_cmd", self._d_logger_cmd.data)
                if (self._human_max < self._wait_people):
                    self._human_max = self._wait_people
                self._d_human_num.data = self._human_max
                self._human_numOut.write()
                print("human_num", self._d_human_num.data)
            elif (all((x == 100000 for x in self._human_x)) & all((x == 100000 for x in self._human_y)) & all((x == 0 for x in self._human_edge1X)) & all((x == 0 for x in self._human_edge1Y)) & all((x == 0 for x in self._human_edge2X)) & all((x == 0 for x in self._human_edge2Y))):
                self._d_logger_cmd.data = self._flag2
                self._logger_cmdOut.write()
                print("logger_cmd", self._d_logger_cmd.data)

            if (self._d_logger_cmd.data == self._flag2):
                self._d_gauss_cmd.data = self._flag3
                self._gauss_cmdOut.write()
                print("gauss_cmd", self._d_gauss_cmd.data)
                self._d_human_num.data = self._human_max
                self._human_numOut.write()
                print("human_num", self._d_human_num.data)
                self._human_max = 0
            else:
                self._d_gauss_cmd.data = self._flag4
                self._gauss_cmdOut.write()
                print("gauss_cmd", self._d_gauss_cmd.data)
                self._d_poco_cmd.data = self._flag5
                self._poco_cmdOut.write()
                print("poco_cmd", self._d_poco_cmd.data)
                self._d_poco_cmd.data = "null"

        return RTC.RTC_OK


def ReceptionCommanderInit(manager):
    profile = OpenRTM_aist.Properties(defaults_str=receptioncommander_spec)
    manager.registerFactory(profile,
                            ReceptionCommander,
                            OpenRTM_aist.Delete)


def MyModuleInit(manager):
    ReceptionCommanderInit(manager)

    # Create a component
    comp = manager.createComponent("ReceptionCommander")


def main():
    mgr = OpenRTM_aist.Manager.init(sys.argv)
    mgr.setModuleInitProc(MyModuleInit)
    mgr.activateManager()
    mgr.runManager()


if __name__ == "__main__":
    main()
