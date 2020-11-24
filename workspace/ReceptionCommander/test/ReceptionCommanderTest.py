#!/usr/bin/env python
# -*- coding: utf-8 -*-
# -*- Python -*-

"""
 @file ReceptionCommanderTest.py
 @brief ModuleDescription
 @date $Date$


"""
import sys
import time
sys.path.append(".")

# Import RTM module
import RTC
import OpenRTM_aist


# Import Service implementation class
# <rtc-template block="service_impl">

# </rtc-template>

# Import Service stub modules
# <rtc-template block="consumer_import">
# </rtc-template>


# This module's spesification
# <rtc-template block="module_spec">
receptioncommandertest_spec = ["implementation_id", "ReceptionCommanderTest", 
		 "type_name",         "ReceptionCommanderTest", 
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
# @class ReceptionCommanderTest
# @brief ModuleDescription
# 
# 
class ReceptionCommanderTest(OpenRTM_aist.DataFlowComponentBase):
	
	##
	# @brief constructor
	# @param manager Maneger Object
	# 
	def __init__(self, manager):
		OpenRTM_aist.DataFlowComponentBase.__init__(self, manager)

		self._d_logger_cmd = OpenRTM_aist.instantiateDataType(RTC.TimedString)
		"""
		"""
		self._logger_cmdIn = OpenRTM_aist.InPort("logger_cmd", self._d_logger_cmd)
		self._d_gauss_cmd = OpenRTM_aist.instantiateDataType(RTC.TimedString)
		"""
		"""
		self._gauss_cmdIn = OpenRTM_aist.InPort("gauss_cmd", self._d_gauss_cmd)
		self._d_human_num = OpenRTM_aist.instantiateDataType(RTC.TimedLong)
		"""
		"""
		self._human_numIn = OpenRTM_aist.InPort("human_num", self._d_human_num)
		self._d_poco_cmd = OpenRTM_aist.instantiateDataType(RTC.TimedString)
		"""
		"""
		self._poco_cmdIn = OpenRTM_aist.InPort("poco_cmd", self._d_poco_cmd)
		self._d_human_x = OpenRTM_aist.instantiateDataType(RTC.TimedLongSeq)
		"""
		"""
		self._human_xOut = OpenRTM_aist.OutPort("human_x", self._d_human_x)
		self._d_human_y = OpenRTM_aist.instantiateDataType(RTC.TimedLongSeq)
		"""
		"""
		self._human_yOut = OpenRTM_aist.OutPort("human_y", self._d_human_y)
		self._d_wait_people = OpenRTM_aist.instantiateDataType(RTC.TimedLong)
		"""
		"""
		self._wait_peopleOut = OpenRTM_aist.OutPort("wait_people", self._d_wait_people)


		


		# initialize of configuration-data.
		# <rtc-template block="init_conf_param">
		
		# </rtc-template>


		 
	##
	#
	# The initialize action (on CREATED->ALIVE transition)
	# formaer rtc_init_entry() 
	# 
	# @return RTC::ReturnCode_t
	# 
	#
	def onInitialize(self):
		# Bind variables and configuration variable
		
		# Set InPort buffers
		self.addInPort("logger_cmd",self._logger_cmdIn)
		self.addInPort("gauss_cmd",self._gauss_cmdIn)
		self.addInPort("human_num",self._human_numIn)
		self.addInPort("poco_cmd",self._poco_cmdIn)
		
		# Set OutPort buffers
		self.addOutPort("human_x",self._human_xOut)
		self.addOutPort("human_y",self._human_yOut)
		self.addOutPort("wait_people",self._wait_peopleOut)
		
		# Set service provider to Ports
		
		# Set service consumers to Ports
		
		# Set CORBA Service Ports
		
		return RTC.RTC_OK
	
	#	##
	#	# 
	#	# The finalize action (on ALIVE->END transition)
	#	# formaer rtc_exiting_entry()
	#	# 
	#	# @return RTC::ReturnCode_t
	#
	#	# 
	#def onFinalize(self):
	#
	#	return RTC.RTC_OK
	
	#	##
	#	#
	#	# The startup action when ExecutionContext startup
	#	# former rtc_starting_entry()
	#	# 
	#	# @param ec_id target ExecutionContext Id
	#	#
	#	# @return RTC::ReturnCode_t
	#	#
	#	#
	#def onStartup(self, ec_id):
	#
	#	return RTC.RTC_OK
	
	#	##
	#	#
	#	# The shutdown action when ExecutionContext stop
	#	# former rtc_stopping_entry()
	#	#
	#	# @param ec_id target ExecutionContext Id
	#	#
	#	# @return RTC::ReturnCode_t
	#	#
	#	#
	#def onShutdown(self, ec_id):
	#
	#	return RTC.RTC_OK
	
		##
		#
		# The activated action (Active state entry action)
		# former rtc_active_entry()
		#
		# @param ec_id target ExecutionContext Id
		# 
		# @return RTC::ReturnCode_t
		#
		#
	def onActivated(self, ec_id):
	
		return RTC.RTC_OK
	
		##
		#
		# The deactivated action (Active state exit action)
		# former rtc_active_exit()
		#
		# @param ec_id target ExecutionContext Id
		#
		# @return RTC::ReturnCode_t
		#
		#
	def onDeactivated(self, ec_id):
	
		return RTC.RTC_OK
	
		##
		#
		# The execution action that is invoked periodically
		# former rtc_active_do()
		#
		# @param ec_id target ExecutionContext Id
		#
		# @return RTC::ReturnCode_t
		#
		#
	def onExecute(self, ec_id):
	
		return RTC.RTC_OK
	
	#	##
	#	#
	#	# The aborting action when main logic error occurred.
	#	# former rtc_aborting_entry()
	#	#
	#	# @param ec_id target ExecutionContext Id
	#	#
	#	# @return RTC::ReturnCode_t
	#	#
	#	#
	#def onAborting(self, ec_id):
	#
	#	return RTC.RTC_OK
	
	#	##
	#	#
	#	# The error action in ERROR state
	#	# former rtc_error_do()
	#	#
	#	# @param ec_id target ExecutionContext Id
	#	#
	#	# @return RTC::ReturnCode_t
	#	#
	#	#
	#def onError(self, ec_id):
	#
	#	return RTC.RTC_OK
	
	#	##
	#	#
	#	# The reset action that is invoked resetting
	#	# This is same but different the former rtc_init_entry()
	#	#
	#	# @param ec_id target ExecutionContext Id
	#	#
	#	# @return RTC::ReturnCode_t
	#	#
	#	#
	#def onReset(self, ec_id):
	#
	#	return RTC.RTC_OK
	
	#	##
	#	#
	#	# The state update action that is invoked after onExecute() action
	#	# no corresponding operation exists in OpenRTm-aist-0.2.0
	#	#
	#	# @param ec_id target ExecutionContext Id
	#	#
	#	# @return RTC::ReturnCode_t
	#	#

	#	#
	#def onStateUpdate(self, ec_id):
	#
	#	return RTC.RTC_OK
	
	#	##
	#	#
	#	# The action that is invoked when execution context's rate is changed
	#	# no corresponding operation exists in OpenRTm-aist-0.2.0
	#	#
	#	# @param ec_id target ExecutionContext Id
	#	#
	#	# @return RTC::ReturnCode_t
	#	#
	#	#
	#def onRateChanged(self, ec_id):
	#
	#	return RTC.RTC_OK
	



def ReceptionCommanderTestInit(manager):
    profile = OpenRTM_aist.Properties(defaults_str=receptioncommandertest_spec)
    manager.registerFactory(profile,
                            ReceptionCommanderTest,
                            OpenRTM_aist.Delete)

def MyModuleInit(manager):
    ReceptionCommanderTestInit(manager)

    # Create a component
    comp = manager.createComponent("ReceptionCommanderTest")

def main():
	mgr = OpenRTM_aist.Manager.init(sys.argv)
	mgr.setModuleInitProc(MyModuleInit)
	mgr.activateManager()
	mgr.runManager()

if __name__ == "__main__":
	main()

