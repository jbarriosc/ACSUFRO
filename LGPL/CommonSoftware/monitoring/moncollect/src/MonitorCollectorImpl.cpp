/*******************************************************************************
* ALMA - Atacama Large Millimiter Array
* (c) European Southern Observatory, 2009
*
* This library is free software; you can redistribute it and/or
* modify it under the terms of the GNU Lesser General Public
* License as published by the Free Software Foundation; either
* version 2.1 of the License, or (at your option) any later version.
*
* This library is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
* Lesser General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public
* License along with this library; if not, write to the Free Software
* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307  USA
*
* "@(#) $Id: MonitorCollectorImpl.cpp,v 1.1 2011/01/19 14:38:01 tstaig Exp $"
*
* who       when      what
* --------  --------  ----------------------------------------------
* bjeram  2009-02-11  created
*/

#include "vltPort.h"

static char *rcsId="@(#) $Id: MonitorCollectorImpl.cpp,v 1.1 2011/01/19 14:38:01 tstaig Exp $";
static void *use_rcsId = ((void)&use_rcsId,(void *) &rcsId);

#include "MonitorCollectorImpl.h"
#include "MonitorCollectorErr.h"

using namespace TMCDB;
using namespace MonitorCollectorErr;


MonitorCollectorImpl::MonitorCollectorImpl(const ACE_CString& name,
			     maci::ContainerServices * containerServices)
    : acscomponent::ACSComponentImpl(name, containerServices),
    numOfComponents_m(0)
{
    AUTO_TRACE("MonitorCollectorImpl::MonitorCollectorImpl");
}//MonitorCollectorImpl

MonitorCollectorImpl::~MonitorCollectorImpl()
{
    AUTO_TRACE("MonitorCollectorImpl::~MonitorCollectorImpl");
}//~MonitorCollectorImpl

void MonitorCollectorImpl::initialize()
{
	AUTO_TRACE("MonitorCollectorImpl::initialize");


	try
	{
		contServ_m = getContainerServices();

		//TBD for time being we register to the blobber latter should be controller
		archiveMonitorController_m = contServ_m->getComponentNonSticky<MonitorArchiver::Controller>("ARCHIVE/TMCDB/MONITOR_CONTROL");
		archiveMonitorController_m->registerCollector(name());
	}
	catch(ACSErr::ACSbaseExImpl &_ex)
	{
		throw;
	}
	catch( CORBA::SystemException &ex )
	{
		ACSErrTypeCommon::CORBAProblemExImpl corbaProblemEx(__FILE__, __LINE__, "MonitorCollectorImpl::initialize");
		corbaProblemEx.setMinor(ex.minor());
		corbaProblemEx.setCompletionStatus(ex.completed());
		corbaProblemEx.setInfo(ex._info().c_str());

		throw corbaProblemEx;
	}
	catch(...)
	{
		ACSErrTypeCommon::UnexpectedExceptionExImpl uex(__FILE__, __LINE__, "MonitorCollectorImpl::initialize");
		throw uex;
	}//try-catch
}//initialize

void MonitorCollectorImpl::cleanUp()
{
	AUTO_TRACE("MonitorCollectorImpl::cleanUp");
	ACE_Hash_Map_Entry <ACE_CString, MonitorComponent*> *entry;
	ACE_Hash_Map_Iterator <ACE_CString, MonitorComponent*, ACE_Recursive_Thread_Mutex> iter(monitorComponents_m);

	archiveMonitorController_m->deregisterCollector(name());

	for( ;iter.next(entry)!=0; iter.advance() )
		delete entry->int_id_;

}//cleanUp

void MonitorCollectorImpl::registerMonitoredDevice (const char * componentName, const char* serialNumber)
{
	AUTO_TRACE("MonitorCollectorImpl::registerMonitoredDevice");

	MonitorComponent*mc = 0;
	try{
		mc = registerMonitoredComponent(componentName);
		if (!mc)
		{
			ACSErrTypeCommon::NullPointerExImpl nex(__FILE__, __LINE__, "MonitorCollectorImpl::registerMonitoredDevice");
			nex.setVariable("mc");
			throw nex;
		}

		mc->setDeviceSerialNumber(const_cast<char*>(serialNumber));
	}
	catch(ACSErr::ACSbaseExImpl &_ex)
	{
		if (mc)
		{
			monitorComponents_m.unbind(componentName);
			delete mc;
		}//if

		RegisteringDeviceProblemExImpl ex(_ex, __FILE__, __LINE__, "MonitorCollectorImpl::registerMonitoredDevice");
		ex.setDevice(componentName);

		throw ex.getRegisteringDeviceProblemEx();
	}
}//MonitorCollectorImpl::registerMonitoredDevice

void MonitorCollectorImpl::registerMonitoredDeviceWithMultipleSerial(const char*componentName, const TMCDB::propertySerialNumberSeq& serialNumbers)
{
	AUTO_TRACE("MonitorCollectorImpl::registerMonitoredDeviceWithMultipleSerial");
	MonitorComponent*mc=0;

	try{
		mc = registerMonitoredComponent(componentName);
		if (!mc)
		{
			ACSErrTypeCommon::NullPointerExImpl nex(__FILE__, __LINE__, "MonitorCollectorImpl::registerMonitoredDeviceWithMultipleSerial");
			nex.setVariable("mc");
			throw nex;
		}

		unsigned int len = serialNumbers.length();
		for(unsigned int i=0; i<len; i++)
		{
			mc->setPropertySerialNumber(const_cast<char*>(serialNumbers[i].propertyName.in()), serialNumbers[i].serialNumbers);
		}
	}
	catch(ACSErr::ACSbaseExImpl &_ex)
	{
		if (mc)
		{
			monitorComponents_m.unbind(componentName);
			delete mc;
		}//if

		RegisteringDeviceProblemExImpl ex(_ex, __FILE__, __LINE__, "MonitorCollectorImpl::registerMonitoredDeviceWithMultipleSerial");
		ex.setDevice(componentName);

		throw ex.getRegisteringDeviceProblemEx();
	}
}//registerMonitoredDeviceWithMultipleSerial

MonitorComponent* MonitorCollectorImpl::registerMonitoredComponent (const char * componentName)
{
    AUTO_TRACE("MonitorCollectorImpl::registerMonitoredComponent");
    MonitorComponent* mc = 0;

    ACE_GUARD_RETURN (ACE_Recursive_Thread_Mutex, proSect, mcMutex_m, 0); // protection

    if (!monitorComponents_m.find(componentName))
    {
    	DeviceAlreadyRegistredExImpl ex(__FILE__, __LINE__, "MonitorCollectorImpl::registerMonitoredDevice");
	    ex.setDevice(componentName);

	    throw ex;
    }//if

    try
	{
	ACS::CharacteristicComponent * comp = contServ_m->getComponentNonSticky<ACS::CharacteristicComponent>(componentName);
	//TBD: here we should check if it is also collocated
	if (!comp->_is_collocated())
	{
		NotCollocatedComponentExImpl ex(__FILE__, __LINE__, "MonitorCollectorImpl::registerMonitoredDevice");
		ex.setComponent(componentName);
		throw ex;
	}//if


	mc = new MonitorComponent(comp, contServ_m);
	mc->addAllProperties();
	monitorComponents_m.bind(componentName,  mc);
	ACS_LOG(LM_FULL_INFO ,"MonitorCollectorImpl::registerMonitoredDevice", (LM_DEBUG, "Device %s has been registered at pos: %d.",
						componentName, numOfComponents_m));
	numOfComponents_m++;

	return mc;
	}
    catch(ACSErr::ACSbaseExImpl &_ex)
	{
    	/// we did not manage to add a device
    	if (mc) delete mc;
    	throw;
	}
    catch(...)
    {
    	/// we did not manage to add a device
    	if (mc) delete mc;

    	ACSErrTypeCommon::UnexpectedExceptionExImpl uex(__FILE__, __LINE__,
							"MonitorCollectorImpl::registerMonitoredDevice");

    	throw uex;
	}//try-catch
}//registerMonitoredComponent


void MonitorCollectorImpl::deregisterMonitoredDevice (const char * componentName)
{
    AUTO_TRACE("MonitorCollectorImpl::deregisterMonitoredDevice");
    MonitorComponent* mc=0;

    ACE_GUARD (ACE_Recursive_Thread_Mutex, proSect, mcMutex_m); // protection

    if (monitorComponents_m.find(componentName, mc))
    {
    	DeviceNotRegistredExImpl ex(__FILE__, __LINE__, "MonitorCollectorImpl::deregisterMonitoredDevice");
    	ex.setDevice(componentName);

    	throw ex.getDeviceNotRegistredEx();
    }//if
    monitorComponents_m.unbind(componentName);
    numOfComponents_m--;
    if (mc)
    {
			mc->stopMonitoring();
			delete mc;
    }
    else
    {
    	ACSErrTypeCommon::NullPointerExImpl ex(__FILE__, __LINE__, "MonitorCollectorImpl::deregisterMonitoredDevice");
    	ex.setVariable("mc");
    	ex.log(LM_WARNING);
    }//if-else

}//deregisterMonitoredDevice

void MonitorCollectorImpl::startMonitoring (const char * componentName)
{
    AUTO_TRACE("MonitorCollectorImpl::startMonitoring");
    MonitorComponent* mc=0;

    ACE_GUARD (ACE_Recursive_Thread_Mutex, proSect, mcMutex_m); // protection
    if (monitorComponents_m.find(componentName, mc))
        {
    	StartMonitoringProblemExImpl ex(__FILE__, __LINE__, "MonitorCollectorImpl::startMonitoring");
    	ex.setDevice(componentName);

    	throw ex.getStartMonitoringProblemEx();
        }//if
    try
    {
    	mc->startMonitoring();
    }
    catch(ACSErr::ACSbaseExImpl &_ex)
    {

    	StartMonitoringProblemExImpl ex(_ex, __FILE__, __LINE__, "MonitorCollectorImpl::startMonitoring");
    	ex.setDevice(componentName);

    	throw ex.getStartMonitoringProblemEx();
    }
    catch(...)
    {
    	ACSErrTypeCommon::UnexpectedExceptionExImpl uex(__FILE__, __LINE__,
    			"MonitorCollectorImpl::startMonitoring");

    	StartMonitoringProblemExImpl ex(uex, __FILE__, __LINE__, "MonitorCollectorImpl::startMonitoring");
    	ex.setDevice(componentName);

    	throw ex.getStartMonitoringProblemEx();
    }//try-catch
 }//startMonitoring

void MonitorCollectorImpl::stopMonitoring (const char * componentName)
{
	AUTO_TRACE("MonitorCollectorImpl::stopMonitoring");
	MonitorComponent* mc=0;

	ACE_GUARD (ACE_Recursive_Thread_Mutex, proSect, mcMutex_m); // protection
	if (monitorComponents_m.find(componentName, mc))
	{
		StopMonitoringProblemExImpl ex(__FILE__, __LINE__, "MonitorCollectorImpl::stopMonitoring");
    	ex.setDevice(componentName);

		throw ex.getStopMonitoringProblemEx();
	}//if
	try
	{
		mc->stopMonitoring();
	}
	catch(...)
	{
		//TBD:: improved catch statement
		ACSErrTypeCommon::UnexpectedExceptionExImpl uex(__FILE__, __LINE__,
				"MonitorCollectorImpl::stopMonitoring");

		StopMonitoringProblemExImpl ex(uex, __FILE__, __LINE__, "MonitorCollectorImpl::stopMonitoring");
		ex.setDevice(componentName);

		throw ex.getStopMonitoringProblemEx();
	}//try-catch
}//stopMonitoring


TMCDB::MonitorDataBlocks * MonitorCollectorImpl::getMonitorData ()
{
    AUTO_TRACE("MonitorCollectorImpl::getMonitorData");

    ACE_Hash_Map_Entry <ACE_CString, MonitorComponent*> *entry;
    ACE_Hash_Map_Iterator <ACE_CString, MonitorComponent*, ACE_Recursive_Thread_Mutex> iter(monitorComponents_m);

    ACE_GUARD_RETURN (ACE_Recursive_Thread_Mutex, proSect, mcMutex_m, new TMCDB::MonitorDataBlocks()); // protection


    TMCDB::MonitorDataBlocks *monitorDataBlock = new TMCDB::MonitorDataBlocks(numOfComponents_m);
    monitorDataBlock->length(numOfComponents_m);

    // we ask all the components for MonitorDataBlock that contains blob data sequence
    for(int i=0; iter.next(entry)!=0; iter.advance() )
    {
        	entry->int_id_->fillSeq();
        	(*monitorDataBlock)[i] = entry->int_id_->getMonitorDataBlock();
        	 i++;

    }//for

    return monitorDataBlock;
}//getMonitorData


/* --------------- [ MACI DLL support functions ] -----------------*/
#include <maciACSComponentDefines.h>
MACI_DLL_SUPPORT_FUNCTIONS(MonitorCollectorImpl)
/* ----------------------------------------------------------------*/


/*___oOo___*/