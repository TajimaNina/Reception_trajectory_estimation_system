#!/usr/bin/env python
# -*- coding: utf-8 -*-
# -*- Python -*-

"""
 @file logger.py
 @brief ModuleDescription
 @date $Date$


"""
import OpenRTM_aist
import RTC
import sys
import time
import csv
import datetime

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
logger_spec = ["implementation_id", "logger",
               "type_name",         "logger",
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
# @class logger
# @brief ModuleDescription
#
#


class logger(OpenRTM_aist.DataFlowComponentBase):

    ##
    # @brief constructor
    # @param manager Maneger Object
    #
    def __init__(self, manager):
        OpenRTM_aist.DataFlowComponentBase.__init__(self, manager)
        self._d_flag = RTC.TimedString(RTC.Time(0, 0), "")
        """
				"""
        self._d_flagIn = OpenRTM_aist.InPort("flag", self._d_flag)
        self._d_human_centerX = RTC.TimedLongSeq(RTC.Time(0, 0), [])
        """
				"""
        self._d_human_centerXIn = OpenRTM_aist.InPort(
            "human_centerX", self._d_human_centerX)
        self._d_human_centerY = RTC.TimedLongSeq(RTC.Time(0, 0), [])
        """
				"""
        self._d_human_centerYIn = OpenRTM_aist.InPort(
            "human_centerY", self._d_human_centerY)
        self._d_human_edge1X = RTC.TimedLongSeq(RTC.Time(0, 0), [])
        """
				"""
        self._d_human_edge1XIn = OpenRTM_aist.InPort(
            "human_edge1X", self._d_human_edge1X)
        self._d_human_edge1Y = RTC.TimedLongSeq(RTC.Time(0, 0), [])
        """
				"""
        self._d_human_edge1YIn = OpenRTM_aist.InPort(
            "human_edge1Y", self._d_human_edge1Y)
        self._d_human_edge2X = RTC.TimedLongSeq(RTC.Time(0, 0), [])
        """
				"""
        self._d_human_edge2XIn = OpenRTM_aist.InPort(
            "human_edge2X", self._d_human_edge2X)
        self._d_human_edge2Y = RTC.TimedLongSeq(RTC.Time(0, 0), [])
        """
				"""
        self._d_human_edge2YIn = OpenRTM_aist.InPort(
            "human_edge2Y", self._d_human_edge2Y)
        self._d_human_in = RTC.TimedLong(RTC.Time(0, 0), 0)
        """
				"""
        self._d_human_inIn = OpenRTM_aist.InPort(
            "human_in", self._d_human_in)
        # self._d_human_out = RTC.TimedLong(RTC.Time(0, 0), 0)
        """
		"""
        # self._d_human_outIn = OpenRTM_aist.InPort("human_out", self._d_human_out)
        self._d_filename = RTC.TimedString(RTC.Time(0, 0), "")
        """
				"""
        self._d_filenameOut = OpenRTM_aist.OutPort(
            "filename", self._d_filename)

        # initialize of configuration-data.
        # <rtc-template block="init_conf_param">

        # </rtc-template>

    def onInitialize(self):
        # Bind variables and configuration variable

        # Set InPort buffers
        self.addInPort("flag", self._d_flagIn)
        self.addInPort("human_centerX", self._d_human_centerXIn)
        self.addInPort("human_centerY", self._d_human_centerYIn)
        self.addInPort("human_edge1X", self._d_human_edge1XIn)
        self.addInPort("human_edge1Y", self._d_human_edge1YIn)
        self.addInPort("human_edge2X", self._d_human_edge2XIn)
        self.addInPort("human_edge2Y", self._d_human_edge2YIn)
        self.addInPort("human_in", self._d_human_inIn)
        self.addOutPort("filename", self._d_filenameOut)
        # self.addInPort("human_out", self._d_human_outIn)

        # Set OutPort buffers

        # Set service provider to Ports

        # Set service consumers to Ports

        # Set CORBA Service Ports

        self._human_centerX = []
        self._human_centerY = []
        self._human_edge1X = []
        self._human_edge1Y = []
        self._human_edge2X = []
        self._human_edge2Y = []
        self._k = []
        self._people_num = 0
        self._pre_people_num = 0
        self._time = datetime.datetime.now()

        return RTC.RTC_OK

    def onActivated(self, ec_id):
        self._k = [1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1]
        return RTC.RTC_OK

    def onDeactivated(self, ec_id):

        return RTC.RTC_OK

    def onExecute(self, ec_id):
        if (self._d_flagIn.isNew()):
            self._d_flag = self._d_flagIn.read()
            self._flag = self._d_flag.data
            print("flag:", self._flag)
            if (self._flag == "write_start"):
                if (self._d_human_centerXIn.isNew() & self._d_human_centerYIn.isNew() & self._d_human_edge1XIn.isNew() & self._d_human_edge1YIn.isNew() & self._d_human_edge2XIn.isNew() & self._d_human_edge2YIn.isNew() & self._d_human_inIn.isNew()):
                    # centerポート読み込み
                    self._d_human_centerX = self._d_human_centerXIn.read()
                    self._human_centerX = self._d_human_centerX.data
                    self._d_human_centerY = self._d_human_centerYIn.read()
                    self._human_centerY = self._d_human_centerY.data
                    print("human_centerX:", self._human_centerX)
                    print("human_centerY:", self._human_centerY)
                    # edgeポート読み込み
                    self._d_human_edge1X = self._d_human_edge1XIn.read()
                    self._human_edge1X = self._d_human_edge1X.data
                    self._d_human_edge1Y = self._d_human_edge1YIn.read()
                    self._human_edge1Y = self._d_human_edge1Y.data
                    print("human_edge1X:", self._human_edge1X)
                    print("human_edge1Y:", self._human_edge1Y)
                    self._d_human_edge2X = self._d_human_edge2XIn.read()
                    self._human_edge2X = self._d_human_edge2X.data
                    self._d_human_edge2Y = self._d_human_edge2YIn.read()
                    self._human_edge2Y = self._d_human_edge2Y.data
                    print("human_edge2X:", self._human_edge2X)
                    print("human_edge2Y:", self._human_edge2Y)

                    self._d_human_in = self._d_human_inIn.read()
                    self._human_in = self._d_human_in.data
                    print("human_in:", self._human_in)
                    # self._d_human_out = self._d_human_outIn.read()
                    # self._human_out = self._d_human_out.data
                    # print("human_out:", self._human_out)

                    self._people_num = int(self._human_in)
                    # if self._people_num != self._pre_people_num:
                    #    self._k = 1

                    log_list = [self._k[self._people_num]]
                    del self._human_centerX[int(self._people_num):]
                    del self._human_centerY[int(self._people_num):]
                    del self._human_edge1X[int(self._people_num):]
                    del self._human_edge1Y[int(self._people_num):]
                    del self._human_edge2X[int(self._people_num):]
                    del self._human_edge2Y[int(self._people_num):]
                    print("people_num:", self._people_num)
                    print("delete_human_centerX:", self._human_centerX)
                    print("delete_human_centerY:", self._human_centerY)
                    print("delete_human_edge1X:", self._human_edge1X)
                    print("delete_human_edge1Y:", self._human_edge1Y)
                    print("delete_human_edge2X:", self._human_edge2X)
                    print("delete_human_edge2Y:", self._human_edge2Y)

                    for i in range(len(self._human_centerX)):
                        log_list.append(self._human_centerX[i])
                        log_list.append(self._human_centerY[i])
                    for i in range(len(self._human_edge1X)):
                        log_list.append(self._human_edge1X[i])
                        log_list.append(self._human_edge1Y[i])
                        log_list.append(self._human_edge2X[i])
                        log_list.append(self._human_edge2Y[i])

                    self._file_name = '..\gaussian_process_multi\data\input\input_human' + \
                        str(self._people_num) + '_' + \
                        self._time.strftime('%Y%m%d_%H%M%S') + '.csv'
                    # self._file_name = 'C:\workspace\gaussian_process_multi\data\input\input_human' + str(self._people_num) + '_'+str(self._time)+'.csv'
                    with open(self._file_name, 'a') as f:
                        writer = csv.writer(f, lineterminator='\n')
                        writer.writerow(log_list)
                    self._d_filename.data = self._file_name
                    # self._pre_people_num = self._people_num
                    for i in range(self._people_num+1):
                        if self._people_num == int(i):
                            self._k[i] += 1
                            print('############update##############')
                            print(self._k[i])
            elif (self._flag == "write_stop"):
                print("write stop")
                self._time = datetime.datetime.now()
                self._k = [1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1]
            else:
                print("?????error?????")
            self._d_filenameOut.write()

        return RTC.RTC_OK


def loggerInit(manager):
    profile = OpenRTM_aist.Properties(defaults_str=logger_spec)
    manager.registerFactory(profile,
                            logger,
                            OpenRTM_aist.Delete)


def MyModuleInit(manager):
    loggerInit(manager)

    # Create a component
    comp = manager.createComponent("logger")


def main():
    mgr = OpenRTM_aist.Manager.init(sys.argv)
    mgr.setModuleInitProc(MyModuleInit)
    mgr.activateManager()
    mgr.runManager()


if __name__ == "__main__":
    main()
