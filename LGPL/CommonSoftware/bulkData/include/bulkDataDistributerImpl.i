template<class TReceiverCallback, class TSenderCallback>
BulkDataDistributerImpl<TReceiverCallback, TSenderCallback>::BulkDataDistributerImpl(const ACE_CString& name,maci::ContainerServices* containerServices) : 
    CharacteristicComponentImpl(name,containerServices)
{
    ACS_TRACE("BulkDataDistributerImpl<>::BulkDataDistributerImpl");

    containerServices_p = containerServices;
}


template<class TReceiverCallback, class TSenderCallback>
BulkDataDistributerImpl<TReceiverCallback, TSenderCallback>::~BulkDataDistributerImpl()
{
    ACS_TRACE("BulkDataDistributerImpl<>::~BulkDataDistributerImpl");
}

template<class TReceiverCallback, class TSenderCallback>
void BulkDataDistributerImpl<TReceiverCallback, TSenderCallback>::initialize()
    throw (ACSErr::ACSbaseExImpl)
{
    ACS_TRACE("BulkDataDistributerImpl<>::initialize");

    dal_p = containerServices_p->getCDB();
    if (CORBA::is_nil(dal_p))
	{
	ACS_SHORT_LOG((LM_ERROR,"BulkDataDistributerImpl<>::initialize error getting CDB reference"));
	ACSBulkDataError::AVObjectNotFoundExImpl err = ACSBulkDataError::AVObjectNotFoundExImpl(__FILE__,__LINE__,"BulkDataDistributerImpl<>::initialize");
	err.log();
	throw err;
	}

    distributer.setContSvc(containerServices_p);
}


template<class TReceiverCallback, class TSenderCallback>
void BulkDataDistributerImpl<TReceiverCallback, TSenderCallback>::cleanUp()
{
    ACS_TRACE("BulkDataDistributerImpl<>::cleanUp");
}


/**************************** Sender part *****************************************/

template<class TReceiverCallback, class TSenderCallback>
void BulkDataDistributerImpl<TReceiverCallback, TSenderCallback>::connect(bulkdata::BulkDataReceiver_ptr receiverObj_p)
    throw (CORBA::SystemException, ACSBulkDataError::AVConnectErrorEx)
{
    ACS_TRACE("BulkDataDistributerImpl<>::connect");

    // single connect not allowed
    ACS_SHORT_LOG((LM_ERROR,"BulkDataDistributerImpl::connect ACSBulkDataError::AVFlowEndpointErrorExImpl single connection not allowed"));
    ACSBulkDataError::AVConnectErrorExImpl err = ACSBulkDataError::AVConnectErrorExImpl(__FILE__,__LINE__,"BulkDataDistributerImpl::connect");
    err.log(LM_DEBUG);
    throw err.getAVConnectErrorEx();
}




template<class TReceiverCallback, class TSenderCallback>
void BulkDataDistributerImpl<TReceiverCallback, TSenderCallback>::multiConnect(bulkdata::BulkDataReceiver_ptr receiverObj_p)
    throw (CORBA::SystemException, ACSBulkDataError::AVConnectErrorEx)
{
    ACS_TRACE("BulkDataDistributerImpl<>::multiConnect");

    if (CORBA::is_nil(receiverObj_p))
	{
	ACS_SHORT_LOG((LM_ERROR,"BulkDataDistributerImpl::multiConnect receiver reference null"));
	ACSBulkDataError::AVConnectErrorExImpl err = ACSBulkDataError::AVConnectErrorExImpl(__FILE__,__LINE__,"BulkDataDistributerImpl::multiConnect");
	throw err.getAVConnectErrorEx();
	}

    char buf[BUFSIZ];

    CORBA::ULong timeout = 0; 

    ACE_CString CDBpath="alma/";
    CDBpath += name();

    CDB::DAO_ptr dao_p = dal_p->get_DAO_Servant(CDBpath.c_str());
    if(CORBA::is_nil(dao_p))
	{
	ACS_SHORT_LOG((LM_ERROR,"BulkDataDistributerImpl::multiConnect error getting DAO reference"));
	ACSBulkDataError::AVConnectErrorExImpl err = ACSBulkDataError::AVConnectErrorExImpl(__FILE__,__LINE__,"BulkDataDistributerImpl::multiConnect");
	throw err.getAVConnectErrorEx();
	}

    ACE_OS::strcpy(buf,dao_p->get_string("sender_protocols"));

    // TBD; not used for now; waiting for requirements 
    try
	{
	timeout = (CORBA::ULong) dao_p->get_long("distr_timeout");
	}
    catch(...)
	{
	timeout = 0;
	}
    
    try
	{
	receiverObj_p->openReceiver();

	bulkdata::BulkDataReceiverConfig *receiverConfig = receiverObj_p->getReceiverConfig();

	ACE_CString recvName = receiverObj_p->name();

	distributer.multiConnect(receiverConfig,buf,recvName);

	distributer.setTimeout(timeout);
	}
    catch(ACSErr::ACSbaseExImpl &ex)
	{
	ACSBulkDataError::AVConnectErrorExImpl err = ACSBulkDataError::AVConnectErrorExImpl(ex,__FILE__,__LINE__,"BulkDataDistributerImpl::multiConnect");
	err.log(LM_DEBUG);
	throw err.getAVConnectErrorEx();
	}
    catch(ACSBulkDataError::AVOpenReceiverErrorEx &ex)
	{
	ACSBulkDataError::AVConnectErrorExImpl err = ACSBulkDataError::AVConnectErrorExImpl(ex,__FILE__,__LINE__,"BulkDataDistributerImpl::multiConnect");
	err.log(LM_DEBUG);
	throw err.getAVConnectErrorEx();
	}
    catch(ACSBulkDataError::AVReceiverConfigErrorEx &ex)
	{
	ACSBulkDataError::AVConnectErrorExImpl err = ACSBulkDataError::AVConnectErrorExImpl(ex,__FILE__,__LINE__,"BulkDataDistributerImpl::multiConnect");
	err.log(LM_DEBUG);
	throw err.getAVConnectErrorEx();
	}
   catch(...)
	{
	ACSErrTypeCommon::UnknownExImpl ex = ACSErrTypeCommon::UnknownExImpl(__FILE__,__LINE__,"BulkDataDistributerImpl::multiConnect");
	ACSBulkDataError::AVConnectErrorExImpl err = ACSBulkDataError::AVConnectErrorExImpl(ex,__FILE__,__LINE__,"BulkDataDistributerImpl::multiConnect");
	throw err.getAVConnectErrorEx();
	}
}


template<class TReceiverCallback, class TSenderCallback>
void BulkDataDistributerImpl<TReceiverCallback, TSenderCallback>::connectByName(const char *receiverName_p)
    throw (CORBA::SystemException, ACSBulkDataError::AVConnectErrorEx)
{
    ACS_TRACE("BulkDataDistributerImpl<>::connectByName");

    try
	{
	bulkdata::BulkDataReceiver_var receiver = containerServices_p->maci::ContainerServices::getComponentNonSticky<bulkdata::BulkDataReceiver>(receiverName_p);
	if(CORBA::is_nil(receiver.in()))
	    {
	    ACS_SHORT_LOG((LM_ERROR,"BulkDataDistributerImpl<>::connectByName receiver reference null"));
	    ACSBulkDataError::AVConnectErrorExImpl err = ACSBulkDataError::AVConnectErrorExImpl(__FILE__,__LINE__,"BulkDataDistributerImpl::connectByName");
	    throw err.getAVConnectErrorEx();
	    }
	else
	    {
	    multiConnect(receiver.in());
	    }
	}
    catch(ACSBulkDataError::AVConnectErrorEx &ex)
	{
	ACSBulkDataError::AVConnectErrorExImpl err = ACSBulkDataError::AVConnectErrorExImpl(ex,__FILE__,__LINE__,"BulkDataDistributerImpl::connectByName");
	err.log(LM_DEBUG);
	throw err.getAVConnectErrorEx();
	}
    catch(ACSErr::ACSbaseExImpl &ex)
	{
	ACSBulkDataError::AVConnectErrorExImpl err = ACSBulkDataError::AVConnectErrorExImpl(ex,__FILE__,__LINE__,"BulkDataDistributerImpl::connectByName");
	err.log(LM_DEBUG);
	throw err.getAVConnectErrorEx();
	}
    catch(...)
	{
	ACSErrTypeCommon::UnknownExImpl ex = ACSErrTypeCommon::UnknownExImpl(__FILE__,__LINE__,"BulkDataDistributerImpl::connectByName");
	ACSBulkDataError::AVConnectErrorExImpl err = ACSBulkDataError::AVConnectErrorExImpl(ex,__FILE__,__LINE__,"BulkDataDistributerImpl::connectByName");
	throw err.getAVConnectErrorEx();
	}

}


template<class TReceiverCallback, class TSenderCallback>
void BulkDataDistributerImpl<TReceiverCallback, TSenderCallback>::disconnect()
    throw (CORBA::SystemException, ACSBulkDataError::AVDisconnectErrorEx)
{
    ACS_TRACE("BulkDataDistributerImpl<>::disconnect");
}



template<class TReceiverCallback, class TSenderCallback>
void BulkDataDistributerImpl<TReceiverCallback, TSenderCallback>::multiDisconnect(bulkdata::BulkDataReceiver_ptr receiverObj_p)
    throw (CORBA::SystemException, ACSBulkDataError::AVDisconnectErrorEx)
{
    ACE_CString recvName = "";

    try
	{
	recvName = receiverObj_p->name();
	distributer.multiDisconnect(recvName);
	}
    catch(ACSErr::ACSbaseExImpl &ex)
	{
	rmEntryFromSenderMap(receiverObj_p);

	ACSBulkDataError::AVDisconnectErrorExImpl err = ACSBulkDataError::AVDisconnectErrorExImpl(ex,__FILE__,__LINE__,"BulkDataDistributerImpl::multiDisconnect");
	err.log(LM_DEBUG);
	throw err.getAVDisconnectErrorEx();
	}
    catch(CORBA::SystemException &ex)
	{
	rmEntryFromSenderMap(receiverObj_p);

	ACSErrTypeCommon::CORBAProblemExImpl err = ACSErrTypeCommon::CORBAProblemExImpl(__FILE__,__LINE__,"BulkDataDistributerImpl::multiDisconnect");
	err.setMinor(ex.minor());
	err.setCompletionStatus(ex.completed());
	err.setInfo(ex._info().c_str());
	ACSBulkDataError::AVDisconnectErrorExImpl err1 = ACSBulkDataError::AVDisconnectErrorExImpl(err,__FILE__,__LINE__,"BulkDataDistributerImpl::multiDisconnect");
	err1.log(LM_DEBUG);
	throw err1.getAVDisconnectErrorEx();
	}
    catch(...)
	{
	rmEntryFromSenderMap(receiverObj_p);

	ACSErrTypeCommon::UnknownExImpl ex = ACSErrTypeCommon::UnknownExImpl(__FILE__,__LINE__,"BulkDataDistributerImpl::multiDisconnect");
	ACSBulkDataError::AVDisconnectErrorExImpl err = ACSBulkDataError::AVDisconnectErrorExImpl(ex,__FILE__,__LINE__,"BulkDataDistributerImpl::multiDisconnect");
	err.log(LM_DEBUG);
	throw err.getAVDisconnectErrorEx();
	}
}


template<class TReceiverCallback, class TSenderCallback>
void BulkDataDistributerImpl<TReceiverCallback, TSenderCallback>::disconnectByName(const char *receiverName_p)
    throw (CORBA::SystemException, ACSBulkDataError::AVDisconnectErrorEx)
{
    ACS_TRACE("BulkDataDistributerImpl<>::disconnectByName");

    try
	{
	if(!distributer.isRecvConnected(receiverName_p))
	    {
	    ACS_SHORT_LOG((LM_ERROR,"BulkDataDistributerImpl::disconnectByName ACSBulkDataError::AVDisconnectErrorExImpl - receiver %s not connected",receiverName_p));
	    ACSBulkDataError::AVDisconnectErrorExImpl err = ACSBulkDataError::AVDisconnectErrorExImpl(__FILE__,__LINE__,"BulkDataDistributerImpl::disconnectByName");
	    throw err.getAVDisconnectErrorEx();
	    }

	bulkdata::BulkDataReceiver_var receiver = containerServices_p->maci::ContainerServices::getComponentNonSticky<bulkdata::BulkDataReceiver>(receiverName_p);
	if(CORBA::is_nil(receiver.in()))
	    {
	    ACS_SHORT_LOG((LM_ERROR,"BulkDataDistributerImpl::disconnectByName could not get receiver component reference"));
	    ACSBulkDataError::AVDisconnectErrorExImpl err = ACSBulkDataError::AVDisconnectErrorExImpl(__FILE__,__LINE__,"BulkDataDistributerImpl::disconnectByName");
	    throw err.getAVDisconnectErrorEx();
	    }
	else
	    {
	    multiDisconnect(receiver.in());
	    //containerServices_p->releaseComponent(receiverName_p);
	    }
	}
    catch(ACSBulkDataError::AVDisconnectErrorEx &ex)
	{
	rmEntryFromSenderMap(receiverName_p);

	ACSBulkDataError::AVDisconnectErrorExImpl err = ACSBulkDataError::AVDisconnectErrorExImpl(ex,__FILE__,__LINE__,"BulkDataDistributerImpl::disconnectByName");
	err.log(LM_DEBUG);
	throw err.getAVDisconnectErrorEx();
	}
    catch(ACSErr::ACSbaseExImpl &ex)
	{
	rmEntryFromSenderMap(receiverName_p);

	ACSBulkDataError::AVDisconnectErrorExImpl err = ACSBulkDataError::AVDisconnectErrorExImpl(ex,__FILE__,__LINE__,"BulkDataDistributerImpl::disconnectByName");
	err.log(LM_DEBUG);
	throw err.getAVDisconnectErrorEx();
	}
    catch(...)
	{
	rmEntryFromSenderMap(receiverName_p);

	ACSErrTypeCommon::UnknownExImpl ex = ACSErrTypeCommon::UnknownExImpl(__FILE__,__LINE__,"BulkDataDistributerImpl::disconnectByName");
	ACSBulkDataError::AVDisconnectErrorExImpl err = ACSBulkDataError::AVDisconnectErrorExImpl(ex,__FILE__,__LINE__,"BulkDataDistributerImpl::disconnectByName");
	throw err.getAVDisconnectErrorEx();
	}		
}


template<class TReceiverCallback, class TSenderCallback>
void BulkDataDistributerImpl<TReceiverCallback, TSenderCallback>::startSend()
    throw (CORBA::SystemException, ACSBulkDataError::AVStartSendErrorEx)
{
    ACS_TRACE("BulkDataDistributerImpl<>::startSend");
}


template<class TReceiverCallback, class TSenderCallback>
void BulkDataDistributerImpl<TReceiverCallback, TSenderCallback>::paceData()
    throw (CORBA::SystemException, ACSBulkDataError::AVPaceDataErrorEx)
{
    ACS_TRACE("BulkDataDistributerImpl<>::paceData");
}


template<class TReceiverCallback, class TSenderCallback>
void BulkDataDistributerImpl<TReceiverCallback, TSenderCallback>::stopSend()
    throw (CORBA::SystemException, ACSBulkDataError::AVStopSendErrorEx)
{
    ACS_TRACE("BulkDataDistributerImpl<>::stopSend");
}


template<class TReceiverCallback, class TSenderCallback>
void BulkDataDistributerImpl<TReceiverCallback, TSenderCallback>::openReceiver()
    throw (CORBA::SystemException, ACSBulkDataError::AVOpenReceiverErrorEx)
{
    ACS_TRACE("BulkDataDistributerImpl<>::openReceiver");
  
    char buf[BUFSIZ];

    ACE_CString CDBpath="alma/";
    CDBpath += name();

    CDB::DAO_ptr dao_p = dal_p->get_DAO_Servant(CDBpath.c_str());
    if (CORBA::is_nil(dao_p))
	{
	ACS_SHORT_LOG((LM_ERROR,"BulkDataDistributerImpl<>::openReceiver error getting DAO reference"));
	ACSBulkDataError::AVOpenReceiverErrorExImpl err = ACSBulkDataError::AVOpenReceiverErrorExImpl(__FILE__,__LINE__,"BulkDataDistributerImpl::openReceiver");
	throw err.getAVOpenReceiverErrorEx();	
	}
    
    ACE_OS::strcpy(buf,dao_p->get_string("recv_protocols"));

    AcsBulkdata::BulkDataReceiver<TReceiverCallback> *recv = distributer.getReceiver();
    if(recv == 0)
	{
	ACS_SHORT_LOG((LM_ERROR,"BulkDataDistributerImpl<>::openReceiver error getting receiver reference"));
	ACSBulkDataError::AVOpenReceiverErrorExImpl err = ACSBulkDataError::AVOpenReceiverErrorExImpl(__FILE__,__LINE__,"BulkDataDistributerImpl::openReceiver");
	throw err.getAVOpenReceiverErrorEx();	
	}

    try
	{
	recv->initialize();

	recv->createMultipleFlows(buf);
	}
    catch(ACSErr::ACSbaseExImpl &ex)
	{
	ACSBulkDataError::AVOpenReceiverErrorExImpl err = ACSBulkDataError::AVOpenReceiverErrorExImpl(ex,__FILE__,__LINE__,"BulkDataDistributerImpl::openReceiver");
	err.log(LM_DEBUG);
	throw err.getAVOpenReceiverErrorEx();
	}
    catch(...)
	{
	ACSErrTypeCommon::UnknownExImpl ex = ACSErrTypeCommon::UnknownExImpl(__FILE__,__LINE__,"BulkDataDistributerImpl::openReceiver");
	ACSBulkDataError::AVOpenReceiverErrorExImpl err = ACSBulkDataError::AVOpenReceiverErrorExImpl(ex,__FILE__,__LINE__,"BulkDataDistributerImpl::openReceiver");
	throw err.getAVOpenReceiverErrorEx();
	}
}


template<class TReceiverCallback, class TSenderCallback>
bulkdata::BulkDataReceiverConfig * BulkDataDistributerImpl<TReceiverCallback, TSenderCallback>::getReceiverConfig()
    throw (CORBA::SystemException, ACSBulkDataError::AVReceiverConfigErrorEx)
{
    ACS_TRACE("BulkDataDistributerImpl<>::getReceiverConfig");

    bulkdata::BulkDataReceiverConfig *receiverConfig = 0;
    try
	{
	receiverConfig = distributer.getReceiver()->getReceiverConfig();
	}
    catch(ACSBulkDataError::AVReceiverConfigErrorExImpl &ex)
	{
	ACSBulkDataError::AVReceiverConfigErrorExImpl err = ACSBulkDataError::AVReceiverConfigErrorExImpl(ex,__FILE__,__LINE__,"BulkDataDistributerImpl::getReceiverConfig");
	err.log(LM_DEBUG);
	throw err.getAVReceiverConfigErrorEx();
	}
    catch(...)
	{
	ACSErrTypeCommon::UnknownExImpl ex = ACSErrTypeCommon::UnknownExImpl(__FILE__,__LINE__,"BulkDataDistributerImpl::getReceiverConfig");
	ACSBulkDataError::AVReceiverConfigErrorExImpl err = ACSBulkDataError::AVReceiverConfigErrorExImpl(ex,__FILE__,__LINE__,"BulkDataDistributerImpl::getReceiverConfig");
	throw err.getAVReceiverConfigErrorEx();
	}

    return receiverConfig;
}


template<class TReceiverCallback, class TSenderCallback>
void BulkDataDistributerImpl<TReceiverCallback, TSenderCallback>::closeReceiver()
    throw (CORBA::SystemException, ACSBulkDataError::AVCloseReceiverErrorEx)
{
    ACS_TRACE("BulkDataDistributerImpl<>::closeReceiver");

    try
	{
	distributer.getReceiver()->closeReceiver();
	}
    catch(ACSBulkDataError::AVCloseReceiverErrorExImpl &ex)
	{
	ACSBulkDataError::AVCloseReceiverErrorExImpl err = ACSBulkDataError::AVCloseReceiverErrorExImpl(ex,__FILE__,__LINE__,"BulkDataDistributerImpl::closeReceiver");
	err.log(LM_DEBUG);
	throw err.getAVCloseReceiverErrorEx();
	}
    catch(...)
	{
	ACSErrTypeCommon::UnknownExImpl ex = ACSErrTypeCommon::UnknownExImpl(__FILE__,__LINE__,"BulkDataDistributerImpl::closeReceiver");
	ACSBulkDataError::AVCloseReceiverErrorExImpl err = ACSBulkDataError::AVCloseReceiverErrorExImpl(ex,__FILE__,__LINE__,"BulkDataDistributerImpl::closeReceiver");
	throw err.getAVCloseReceiverErrorEx();
	}
}


template<class TReceiverCallback, class TSenderCallback>
void BulkDataDistributerImpl<TReceiverCallback, TSenderCallback>::setReceiver(const bulkdata::BulkDataReceiverConfig &receiverConfig)
    throw (CORBA::SystemException, ACSBulkDataError::AVSetReceiverErrorEx)
{
    ACS_TRACE("BulkDataDistributerImpl<>::setReceiver");

    CORBA::ULong dim = receiverConfig.fepsInfo.length();
    
// loop on all flows
    for(CORBA::ULong i = 0; i < dim; i++)
	{ 
	ACE_CString flw = TAO_AV_Core::get_flowname(receiverConfig.fepsInfo[i]);

	TReceiverCallback *cb = 0;

	AcsBulkdata::BulkDataReceiver<TReceiverCallback> *recv = distributer.getReceiver();
	if(recv == 0)
	    {
	    ACS_SHORT_LOG((LM_ERROR,"BulkDataDistributerImpl<>::setReceiver error getting receiver reference"));
	    ACSBulkDataError::AVSetReceiverErrorExImpl err = ACSBulkDataError::AVSetReceiverErrorExImpl(__FILE__,__LINE__,"BulkDataBulkDataDistributerImpl::setReceiver");
	    throw err.getAVSetReceiverErrorEx();
	    }
	try
	    {
	    recv->getFlowCallback(flw, cb);
	    }
	catch(ACSErr::ACSbaseExImpl &ex)
	    {
	    ACSBulkDataError::AVSetReceiverErrorExImpl err = ACSBulkDataError::AVSetReceiverErrorExImpl(ex,__FILE__,__LINE__,"BulkDataBulkDataDistributerImpl::setReceiver");
	    err.log(LM_DEBUG);
	    throw err.getAVSetReceiverErrorEx();
	    }
	catch(...)
	    {
	    ACSErrTypeCommon::UnknownExImpl ex = ACSErrTypeCommon::UnknownExImpl(__FILE__,__LINE__,"BulkDataDistributerImpl::setReceiver");
	    ACSBulkDataError::AVSetReceiverErrorExImpl err = ACSBulkDataError::AVSetReceiverErrorExImpl(ex,__FILE__,__LINE__,"BulkDataBulkDataDistributerImpl::setReceiver");
	    throw err.getAVSetReceiverErrorEx();
	    }

	if(cb == 0)
	    {
	    ACS_SHORT_LOG((LM_ERROR,"BulkDataDistributerImpl<>::setReceiver distributor callback null"));
	    ACSBulkDataError::AVSetReceiverErrorExImpl err = ACSBulkDataError::AVSetReceiverErrorExImpl(__FILE__,__LINE__,"BulkDataBulkDataDistributerImpl::setReceiver");
	    throw err.getAVSetReceiverErrorEx();
	    } 
	else
	    {
	    cb->setDistributerImpl(this);
	    }	    
	}
}


template<class TReceiverCallback, class TSenderCallback>
ACSErr::Completion * BulkDataDistributerImpl<TReceiverCallback, TSenderCallback>::getCbStatus(CORBA::ULong flowNumber) 
    throw (CORBA::SystemException, ACSBulkDataError::AVInvalidFlowNumberEx, ACSBulkDataError::AVFlowEndpointErrorEx)
{
    ACS_TRACE("BulkDataDistributerImpl<>::getCbStatus");
    
    TReceiverCallback *cb = 0;

    try
	{
	getDistributer()->getReceiver()->getFlowCallback(flowNumber,cb);
	}
    catch(ACSBulkDataError::AVInvalidFlowNumberExImpl &ex)
	{
	ACSBulkDataError::AVInvalidFlowNumberExImpl err = ACSBulkDataError::AVInvalidFlowNumberExImpl(ex,__FILE__,__LINE__,"BulkDataDistributerImpl::getCbStatus");
	err.log(LM_DEBUG);
	throw err.getAVInvalidFlowNumberEx();
	}
    catch(ACSBulkDataError::AVFlowEndpointErrorExImpl &ex)
	{
	ACSBulkDataError::AVFlowEndpointErrorExImpl err = ACSBulkDataError::AVFlowEndpointErrorExImpl(ex,__FILE__,__LINE__,"BulkDataDistributerImpl::getCbStatus");
	err.log(LM_DEBUG);
	throw err.getAVFlowEndpointErrorEx();
	}

    if(cb->isTimeout() && cb->isWorking())
	{
	ACSBulkDataStatus::AVCbWorkingTimeoutCompletion *comp = new ACSBulkDataStatus::AVCbWorkingTimeoutCompletion();
	//comp->log();
	return comp->returnCompletion();
	}
    if(cb->isTimeout())
	{
	ACSBulkDataStatus::AVCbTimeoutCompletion *comp = new ACSBulkDataStatus::AVCbTimeoutCompletion();
	//comp->log();
	return comp->returnCompletion();
	}
    if(cb->isWorking())
	{
	ACSBulkDataStatus::AVCbWorkingCompletion *comp = new ACSBulkDataStatus::AVCbWorkingCompletion();
	//comp->log();
	return comp->returnCompletion();
	}	
    
    ACSBulkDataStatus::AVCbReadyCompletion *comp = new ACSBulkDataStatus::AVCbReadyCompletion();
    //comp->log();

    return comp->returnCompletion();
}


template<class TReceiverCallback, class TSenderCallback>
ACSErr::Completion * BulkDataDistributerImpl<TReceiverCallback, TSenderCallback>::getReceiverCbStatus(const char *recvName, CORBA::ULong flowNumber) 
    throw (CORBA::SystemException)
{
    ACS_TRACE("BulkDataDistributerImpl<>::getReceiverCbStatus");

    bulkdata::BulkDataReceiver_var receiver = containerServices_p->maci::ContainerServices::getComponent<bulkdata::BulkDataReceiver>(recvName);
    if(!CORBA::is_nil(receiver.in()))
	{
	return receiver->getCbStatus(flowNumber);
	}
    
    ACSBulkDataStatus::AVCbNotAvailableCompletion *comp = new ACSBulkDataStatus::AVCbNotAvailableCompletion();
    return comp->returnCompletion();
}


template<class TReceiverCallback, class TSenderCallback>
void BulkDataDistributerImpl<TReceiverCallback, TSenderCallback>::rmEntryFromSenderMap(bulkdata::BulkDataReceiver_ptr receiverObj_p)
{
    ACS_TRACE("BulkDataDistributerImpl<>::removeEntryFromMap");

    Sender_Map *map = getDistributer()->getSenderMap();

    Sender_Map_Iterator iterator(*map);
    Sender_Map_Entry *entry = 0;
    for (;iterator.next (entry) !=  0;iterator.advance ())
	{
	if( ((entry->int_id_).first())->_is_equivalent(receiverObj_p) )
	    {
	    CORBA::release((entry->int_id_).first()); 
	    map->unbind(entry);
	    }
	}
}


template<class TReceiverCallback, class TSenderCallback>
void BulkDataDistributerImpl<TReceiverCallback, TSenderCallback>::rmEntryFromSenderMap(const char *receiverName_p)
{
    ACS_TRACE("BulkDataDistributerImpl<>::removeEntryFromMap");

    Sender_Map *map = getDistributer()->getSenderMap();

    Sender_Map_Iterator iterator(*map);
    Sender_Map_Entry *entry = 0;
    for (;iterator.next (entry) !=  0;iterator.advance ())
	{
	if(strcmp((entry->ext_id_).c_str(),receiverName_p) == 0)
	    {
	    CORBA::release((entry->int_id_).first()); 
	    map->unbind(entry);
	    }
	}
}


template<class TReceiverCallback, class TSenderCallback>
void BulkDataDistributerImpl<TReceiverCallback, TSenderCallback>::subscribeNotification(ACS::CBvoid_ptr notifCb)
    throw (CORBA::SystemException, ACSBulkDataError::AVNotificationMechanismErrorEx)
{
    try
	{
	getDistributer()->subscribeNotification(notifCb);
	}
    catch(ACSErr::ACSbaseExImpl &ex)
	{
	ACSBulkDataError::AVNotificationMechanismErrorExImpl err = ACSBulkDataError::AVNotificationMechanismErrorExImpl(ex,__FILE__,__LINE__,"BulkDataDistributerImpl::subscribeNotification");
	err.log(LM_DEBUG);
	throw err.getAVNotificationMechanismErrorEx();
	}
   catch(...)
	{
	ACSErrTypeCommon::UnknownExImpl ex = ACSErrTypeCommon::UnknownExImpl(__FILE__,__LINE__,"BulkDataDistributerImpl::subscribeNotification");
	ACSBulkDataError::AVNotificationMechanismErrorExImpl err = ACSBulkDataError::AVNotificationMechanismErrorExImpl(ex,__FILE__,__LINE__,"BulkDataDistributerImpl::subscribeNotification");
	err.log(LM_DEBUG);
	throw err.getAVNotificationMechanismErrorEx();
	}
}
