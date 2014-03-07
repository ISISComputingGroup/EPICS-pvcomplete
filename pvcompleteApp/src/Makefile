TOP=../..

include $(TOP)/configure/CONFIG
#----------------------------------------
#  ADD MACRO DEFINITIONS AFTER THIS LINE
#=============================

#==================================================
# build a support library

LIBRARY_IOC += pvcomplete

# xxxRecord.h will be created from xxxRecord.dbd
#DBDINC += xxxRecord
# install pvcomplete.dbd into <top>/dbd
DBD += pvcomplete.dbd

# specify all source files to be compiled and added to the library
pvcomplete_SRCS += pvcomplete.cpp

pvcomplete_LIBS += $(EPICS_BASE_IOC_LIBS)

#===========================

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE

