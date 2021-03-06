#ifndef _ERROR_COMPONENT_H
#define _ERROR_COMPONENT_H
/*******************************************************************************
*    ALMA - Atacama Large Millimiter Array
*    (c) Associated Universities Inc., 2002 
*    (c) European Southern Observatory, 2002
*    Copyright by ESO (in the framework of the ALMA collaboration)
*    and Cosylab 2002, All rights reserved
*
*    This library is free software; you can redistribute it and/or
*    modify it under the terms of the GNU Lesser General Public
*    License as published by the Free Software Foundation; either
*    version 2.1 of the License, or (at your option) any later version.
*
*    This library is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
*    Lesser General Public License for more details.
*
*    You should have received a copy of the GNU Lesser General Public
*    License along with this library; if not, write to the Free Software
*    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307  USA
*
* "@(#) $Id: acsexmplErrorComponentImpl.h,v 1.13 2010/11/10 16:58:09 rbourtem Exp $"
*
* who       when      what
* --------  --------  ----------------------------------------------
* david 2002-09-26 added more comments
* david  25/09/02  created
*/

#ifndef __cplusplus
#error This is a C++ include file and cannot be used from plain C
#endif

///This example is unique because it is derived from CharacteristicComponent's
///superclass, ACSComponent.
#include <acscomponentImpl.h>

///This is the CORBA stub client header for ACSErrTypeCommon.idl where the 
///definition of the CORBA exception is found.
#include <ACSErrTypeCommon.h>

/**
 *  The empty CORBA servant interface, POA_acsexmplErrorComponent::ErrorComponent,
 *  is obtained from this header file and is automatically generated from 
 *  ErrorComponent's Interface Definition File (i.e., acsexmplErrorComponent.idl) 
 *  by CORBA.
 */
#include <acsexmplErrorComponentS.h>
 
/** @file acsexmplErrorComponentImpl.h
 */

/** @addtogroup ACSEXMPLTOC
*/
/*@{
*/

/** @addtogroup ACSEXMPLTOCCOMPONENTS
*/
/*@{
*/

/** @defgroup ACSEXMPLERRORCOMPDOC Error Component
 *  @{
 * @htmlonly
<hr size="2" width="100%">
<div align="left">
<h2>Description</h2>
Simple Component that throws few exceptions and completions to show the use.
It shows how to the members can be set, when the exception/completion has one.
Also shows the use of the wrappers of exception/completions and how transform
them to corba structures to be catch by the client.
<br>
<br>
<h2>What can I gain from this example?</h2>
<ul>
  <li>an example derived from the ACS::ACSComponent IDL interface.</li>
  <li>use of ACS Exceptions</li>
  <li>use of ACS Completions</li>
  <li>raising CORBA exceptions.</li>
</ul>
<br>
<br>
<h2>Links</h2>
<ul>
  <li><a href="classErrorComponent.html">Error Component Class Reference</a></li>
  <li><a href="interfaceacsexmplErrorComponent_1_1ErrorComponent.html">Error Component IDL Documentation</a></li>
</ul>
</div>
   @endhtmlonly
 * @}
 */

/**
 * This class shows how to deal with errors in components.
 *
 * All components should inherit from CharacteristicComponentImpl or it's 
 * superclass, ACSComponentImpl, to remain  compatiable with ACS tools such 
 * as objexp (i.e., a GUI used to manipulate components).  This class also 
 * derives from POA_acsexmplErrorComponent::ErrorComponent which is a class automatically 
 * generated by CORBA from ErrorComponent's IDL file.
 * @version "@(#) $Id: acsexmplErrorComponentImpl.h,v 1.13 2010/11/10 16:58:09 rbourtem Exp $"
 */
class ErrorComponent: public virtual acscomponent::ACSComponentImpl,     //Component superclass
		  public POA_acsexmplErrorComponent::ErrorComponent    //CORBA servant stub
{    
  public:
    /**
     * Constructor
     * @param poa Poa which will activate this and also all other components. Developers need
     * not be concerned with what a PortableServer does...just pass it to the superclass's
     * constructor.
     * @param name component's name. All components have a name associated with them so other 
     * components and clients can access them.
     */
    ErrorComponent(
	       const ACE_CString& name,
	       maci::ContainerServices * containerServices);
    
    /**
     * Destructor
     */
    virtual ~ErrorComponent();
    
    /* --------------------- [ CORBA interface ] ----------------------*/    
    /**
     * Displays "Hello World" to the console.
     * Implementation of IDL displayMessage().
     * @htmlonly
       <br><hr>
       @endhtmlonly
     */     
    virtual void 
    displayMessage ();

    /**
     * Simple method raises a remote exception within the calling client.
     * Implementation of IDL badMethod().
     * @param depth depth of the error trace
     * @throw ACSErrTypeCommon::GenericErrorEx
     * @throw ACSErrTypeCommon::UnexpectedExceptionEx
     * @htmlonly
       <br><hr>
       @endhtmlonly
     */     
    virtual void
    badMethod(CORBA::Short depth); 

   /**
     * Simple method raises a remote exception within the calling client.
     * The error trace in the exception is added from a completion.
     * Implementation of IDL method.
     * @param depth depth of the error trace
     * @throw ACSErrTypeCommon::GenericErrorEx
     * @htmlonly
       <br><hr>
       @endhtmlonly
     */     
    virtual void
    exceptionFromCompletion(CORBA::Short depth);

    /**
     * This method throws a REMOTE (CORBA) type exception if depth>0.
     * If we want to throw a type exception we have to specfy this in the method signature.
     * In this example the method can throw: 
     *     ACSErrTypeCommon::GenericErrorEx or
     *     ACSErrTypeCommon::ACSErrTypeCommonEx exceptions. 
     * Although GenericErrorEx derives from ACSErrTypeCommonEx 
     * we have to specfy both in the method signature because IDL
     *  does not support hierarchy of exceptions.
     * @param depth depth of the error trace
     * @throw ACSErrTypeCommon::GenericErrorEx
     * @throw ACSErrTypeCommon::ACSErrTypeCommonEx
     * @htmlonly
     <br><hr>
     @endhtmlonly
    */
    virtual void  typeException(CORBA::Short depth); 


    /** 
     *  Method that throws a CORBA::BAD_PARAM system exception
     *  to show how to handle CORBA System Exceptions
     */
    virtual void corbaSystemException();

    /** 
     *  Simple method that returns an ACSErr::Completion,
     *  where an error trace is added from an exception.
     *  @param depth depth of the error trace, if <=0, returns OK completion
     *  @return ACSErr::Completion
     *  @htmlonly
     *  <br><hr>
     *  @endhtmlonly
     */
    virtual ACSErr::Completion *completionFromException(CORBA::Short depth); 

    /** 
     *  Simple method that returns a REMOTE  (CORBA) completion (ACSErr::Completion),
     *  where an error trace is added from a completion.
     *  @param depth depth of the error trace, if <=0, returns OK completion
     *  @return ACSErr::Completion
     *  @htmlonly
     *  <br><hr>
     *  @endhtmlonly
     */
    virtual ACSErr::Completion *completionFromCompletion(CORBA::Short depth); 

   /** 
     * This method is similar than  #completionFromCompletion, 
     * but it allocated C++ completion on the stack instead on the heap.
     *  @param depth depth of the error trace, if <=0, returns OK completion
     *  @return ACSErr::Completion
     *  @htmlonly
     *  <br><hr>
     *  @endhtmlonly
     */
    ACSErr::Completion *completionOnStack(CORBA::Short depth); 

    /**
     * Simple method that returns an ACSErr::Completion as an out parameter
     * @param comp Completion as out parameter
     *  @htmlonly
     *  <br><hr>
     *  @endhtmlonly
     */
    void outCompletion(ACSErr::Completion_out comp);
    
    /**
     * A method wich will generate the SIGFPE signal
     * This can be used to test what happens when there is a floating point 
     * exception in a component
     *  @param way parameter used to specify the way to generate the SIGFPE
     *  way == 0: Send SIGFPE signal with the kill() function363
     *  way == 1: Generate a floating point exception by dividing by 0
     *  @htmlonly
     *  <br><hr>
     *  @endhtmlonly
     */
     void generateSIGFPE (CORBA::Short way);
    
    /**
     * A method wich will generate the SIGSEGV signal
     * This can be used to test what happens when there is a segmentation 
     * fault in a component
     *  @param way parameter used to specify the way to generate the SIGSEGV
     *  way == 0: Send SIGSEGV signal with the kill() function
     *  way == 1: Generate a segmentation fault by accessing to a null pointer
     *  @htmlonly
     *  <br><hr>
     *  @endhtmlonly
     */
    void generateSIGSEGV (CORBA::Short way);

    /**
	 * A method that will sleep during nb_seconds seconds before to return
	 * This method could be used to test the behavior of components
	 * taking a long time before to reply.
	 * @param nb_seconds the number of seconds to sleep before to return
	 */
    void sleepingCmd(CORBA::Short nb_seconds);

  private:
    
    /**
     * This method return a LOCAL (C++) completion
     * which contains an error trace if depth > 0,
     * otherwise it returns non error completion (ACSErrOKCompletion),
     * i.e. completion w/o ane error trace
     * @param depth depth of the error trace
     */
    virtual ACSErr::CompletionImpl *createCompletion(unsigned short depth);

    
    /**
     * The aim of this method is to build an error trace.
     * It builds the error trace using exception, 
     * but the same could be done using Completion.
     * It simply throw an exception containing
     * an error trace with the requested depth, if > 0
     * Otherwise just returns.
     *
     * Notice that this method throws a LOCAL exception xxxExImpl 
     * and not a remote exception xxx
     *
     * @param depth depth of the error trace
     * @throw ACSErrTypeCommon::GenericErrorExImpl
     */     
    virtual void
    buildErrorTrace(unsigned short depth) ;

};
/*\@}*/
/*\@}*/

#endif /*!_ERROR_COMPONENT_H*/



