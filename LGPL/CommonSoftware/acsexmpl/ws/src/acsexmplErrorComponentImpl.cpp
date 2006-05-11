/*******************************************************************************
*    ALMA - Atacama Large Millimiter Array
*    (c) Associated Universities Inc., 2002 *
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
*
*
* "@(#) $Id: acsexmplErrorComponentImpl.cpp,v 1.2 2006/05/11 13:13:19 bjeram Exp $"
*
* who       when      what
* --------  --------  ----------------------------------------------
* david 2002-09-26 added many more comments
* david  25/09/02  created 
*/
 
#include <acsexmplErrorComponentImpl.h>
#include <ACSErrTypeCommon.h>
#include <ACSErrTypeOK.h>
#include <iostream>

ACE_RCSID(acsexmpl, acsexmplErrorComponentImpl, "$Id: acsexmplErrorComponentImpl.cpp,v 1.2 2006/05/11 13:13:19 bjeram Exp $")

/* ----------------------------------------------------------------*/
ErrorComponent::ErrorComponent( 
		       const ACE_CString &name,
		       maci::ContainerServices * containerServices) :
    ACSComponentImpl(name, containerServices)
{
    // ACS_TRACE is used for debugging purposes
    ACS_TRACE("::ErrorComponent::ErrorComponent");
}
/* ----------------------------------------------------------------*/
ErrorComponent::~ErrorComponent()
{
    // ACS_TRACE is used for debugging purposes
    ACS_TRACE("::ErrorComponent::~ErrorComponent");
    ACS_DEBUG_PARAM("::ErrorComponent::~ErrorComponent", "Destroying %s...", name());
}
/* --------------------- [ CORBA interface ] ----------------------*/
void
ErrorComponent::displayMessage ()
    throw (CORBA::SystemException)
{
    std::cout << "Hello World" << std::endl; 
}
/* ----------------------------------------------------------------*/
void 
ErrorComponent::badMethod(CORBA::Short depth) 
    throw(CORBA::SystemException, ACSErrTypeCommon::GenericErrorEx)
{
    ACS_TRACE("::ErrorComponent::badMethod");
    try
	{
	// We decrement the depth, because we are going to add one
        // error here in any case.
	buildErrorTrace(depth-1);
	}
    catch(ACSErrTypeCommon::GenericErrorExImpl &ex)
	{
	ACSErrTypeCommon::GenericErrorExImpl ex2(ex, 
						 __FILE__, __LINE__, 
						 "ErrorComponent::badMethod");
	ex2.setErrorDesc("Generated multi level exception");
	throw ex2.getGenericErrorEx();
	}
    catch(...)
	{
	ACSErrTypeCommon::GenericErrorExImpl ex2(__FILE__, __LINE__, 
						 "ErrorComponent::badMethod");
	ex2.setErrorDesc("Got unexpected exception");
	throw ex2.getGenericErrorEx();
	}
    
    /*
     * We should get here only if a depth<=1 was requested.
     */
    ACSErrTypeCommon::GenericErrorExImpl ex(__FILE__, __LINE__, 
					    "ErrorComponent::badMethod");
    ex.setErrorDesc("An error trace with depth lower or equal to 1 was requested.");
    throw ex.getGenericErrorEx();
}


void ErrorComponent::typeException(CORBA::Short depth) 
    throw (CORBA::SystemException, 
	   ACSErrTypeCommon::GenericErrorEx,  
	   ACSErrTypeCommon::ACSErrTypeCommonEx)
{
    ACS_TRACE("::ErrorComponent::typeException");
    try
	{
	// We decrement the depth, because we are going to add one
        // error here in any case.
	buildErrorTrace(depth-1);
	}
    catch(ACSErrTypeCommon::GenericErrorExImpl &ex)
	{
	ACSErrTypeCommon::GenericErrorExImpl ex2(ex, 
						 __FILE__, __LINE__, 
						 "ErrorComponent::badMethod");
	ex2.setErrorDesc("Generated multi level exception");
	throw ex2.getACSErrTypeCommonEx();
	}
    catch(...)
	{
	ACSErrTypeCommon::GenericErrorExImpl ex2(__FILE__, __LINE__, 
						 "ErrorComponent::badMethod");
	ex2.setErrorDesc("Got unexpected exception");
	throw ex2.getACSErrTypeCommonEx();
	}
    
    /*
     * We should get here only if a depth<=1 was requested.
     */
    ACSErrTypeCommon::GenericErrorExImpl ex(__FILE__, __LINE__, 
					    "ErrorComponent::badMethod");
    ex.setErrorDesc("An error trace with depth lower or equal to 1 was requested.");
    throw ex.getACSErrTypeCommonEx();
}//typeException


/* ----------------------------------------------------------------*/
ACSErr::Completion *ErrorComponent::completionFromException(CORBA::Short depth) 
    throw (CORBA::SystemException)
{
    ACS_TRACE("::ErrorComponent::completionFromException");
    // here we get LOCAL (C++) completion 
    CompletionImpl *er = returnCompletion(depth);

    // returnCompletion() automatically deletes er
    //   NOTE: you can use returnCompletion() 
    //   just if completion is allocated on the heap. 
    //   If completion is allocated on the stack
    //   returnCompletion(false) has to be used. 
    return er->returnCompletion();
}//completionFromException

ACSErr::Completion *ErrorComponent::completionOnStack(CORBA::Short depth) 
    throw (CORBA::SystemException)
{
    ACS_TRACE("::ErrorComponent::completionOnStack");

    // here we get LOCAL (C++) completion 
    CompletionImpl *comp = returnCompletion(depth);
    

    // if comp does not conatin error (=is error free) we return it 
    // otherwise we create a new completion which takes the error trace from a completion comp.
    if (comp->isErrorFree())
	{
	// memory for comp is released in the call 
	return comp->returnCompletion();
	}
    else
	{
	// The constructor takes care for the memory manamgent 
	// for the passed completion or exception.
	// If a completion or an exception is passed as pointer the constructor assumes that
        // completion was created on the heap and thus it deletes it afterwards,
	// so it MUST NOT be deleted by the user !
	// If a completion or an exception is passed as an object (or reference to it) 
	// the constructor assumes that the it was created on the stack 
	// and thus it does not delete it.
        //
        // NOTE: do not pass a pointer of a completion or an exception 
	// which was created on the stack !! In this case just send the completion object.
	ACSErrTypeCommon::GenericErrorCompletion erg(comp, 
						     __FILE__, __LINE__, 
						     "ErrorComponent::completionOnStack");
	erg.setErrorDesc("Put an error trace in completionOnStack");
	return erg.returnCompletion(false); 
        // With false flag we tell returnCompletion() 
	// not to delete its object
	// since it is automatically deleted when we go out of scope
	}//if
}//completionOnStack

void  ErrorComponent::exceptionFromCompletion(CORBA::Short depth) 
    throw (CORBA::SystemException, ACSErrTypeCommon::GenericErrorEx)
{
    ACS_TRACE("ErrorComponent::exceptionFromCompletion");
     CompletionImpl *comp = returnCompletion(depth);

     // if comp does not conatin error (=is error free) we just return 
     // otherwise we create a new exception which takes the error trace from a completion comp.     
    if (!comp->isErrorFree())
	{
	// The constructor takes care for the memory manamgent 
	// for the passed completion or exception.
	// If a completion or an exception is passed as pointer the constructor assumes that
        // completion was created on the heap and thus it deletes it afterwards,
	// so it MUST NOT be deleted by the user !
	// If a completion or an exception is passed as an object (or reference to it) 
	// the constructor assumes that the it was created on the stack 
	// and thus it does not delete it.
        //
        // NOTE: do not pass a pointer of a completion or an exception 
	// which was created on the stack !! In this case just send the completion object.
	ACSErrTypeCommon::GenericErrorExImpl ex2(comp, 
						 __FILE__, __LINE__, 
						 "ErrorComponent::exceptionFromCompletion");
	ex2.setErrorDesc("Exception generated by adding an error trace from a completion");
	throw ex2.getGenericErrorEx();
	}//if
}

ACSErr::Completion *ErrorComponent::completionFromCompletion(CORBA::Short depth) 
	throw (CORBA::SystemException)
{
    ACS_TRACE("ErrorComponent::completionFromCompletion");
    CompletionImpl *comp = returnCompletion(depth);

    // if comp does not conatin error (=is error free) we return it 
    // otherwise we create a new completion which takes the error trace from a completion comp.
    if (!comp->isErrorFree())
	{
	// comp is deleted inside the constructor
	ACSErrTypeCommon::GenericErrorCompletion *erg = 
	    new ACSErrTypeCommon::GenericErrorCompletion(comp, 
							 __FILE__, __LINE__, 
							 "ErrorComponent::completionFromCompletion");
	erg->setErrorDesc("Put an error trace in completionFromCompletion");
	comp = erg;
	}//if
    
    return comp->returnCompletion();
}//completionFromCompletion

CompletionImpl *ErrorComponent::returnCompletion(unsigned short depth)
{
   ACS_TRACE("::ErrorComponent::returnCompletion");
   CompletionImpl *er;

    if( depth <= 0 )
	{
	er = new ACSErrTypeOK::ACSErrOKCompletion();
	}
    else
	{
	try
	    {
	    buildErrorTrace(depth);
	    }
	catch(ACSErrTypeCommon::GenericErrorExImpl &ex)
	    {
          // Here we build a completion from an exception: 
	  //  we create a new completion where it is added the error trace from an exception.
	    ACSErrTypeCommon::GenericErrorCompletion *erg = 
		new ACSErrTypeCommon::GenericErrorCompletion(ex, 
						     __FILE__, __LINE__, 
						     "ErrorComponent::completionFromException");
	    erg->setErrorDesc("Put an error trace in completionFromException");
	    er = erg;
	    }
	catch(...)
	    {
	    ACSErrTypeCommon::GenericErrorCompletion *erg = 
		new ACSErrTypeCommon::GenericErrorCompletion( 
						     __FILE__, __LINE__, 
						     "ErrorComponent::completionFromException");
	    erg->setErrorDesc("Got unexpected exception");
	    er=erg;
	    }
	}  
    
    // we are in local case so caller has to do the memory managment 
    // i.e. release the memory of CompletionImpl !
    return er;
}//returnCompletion

/************
 * Utility method to build a deep ErrorTrace
 ************/
void
ErrorComponent::buildErrorTrace(unsigned short depth) 
    throw (ACSErrTypeCommon::GenericErrorExImpl)
{
    ACS_TRACE("::ErrorComponent::buildErrorTrace");

    /*
     * If depth is 1, we are at the bottom and 
     * we just have to throw an exception.
     * Going up the recursive chain this will be
     * atteched to all other exceptions
     */
    if(depth == 1)
	{
	ACSErrTypeCommon::GenericErrorExImpl ex(__FILE__, __LINE__, 
						"ErrorComponent::buildErrorTrace");
	ex.setErrorDesc("Bottom of error trace");
	throw ex;
	}
	    
    /*
     * If depth > 1, make a recursive call.
     * We will have to get back an exception with a trace with
     * a depth shorter by 1 element.
     */
    if(depth > 1)
	{
	try
	    {
	    buildErrorTrace(depth-1);
	    }
	catch(ACSErrTypeCommon::GenericErrorExImpl &ex)
	    {
	    ACSErrTypeCommon::GenericErrorExImpl ex2(ex, 
						     __FILE__, __LINE__, 
						     "ErrorComponent::errorTrace");
	    ex2.setErrorDesc("Generated multi level exception");
	    throw ex2;
	    }
	catch(...) // This should never happen!!!!
	    {
	    ACSErrTypeCommon::GenericErrorExImpl ex2(__FILE__, __LINE__, 
						     "ErrorComponent::errorTrace");
	    ex2.setErrorDesc("Got unexpected exception");
	    throw ex2;
	    }
	}
    /*
     * We should get here only if depth <= 0,
     * I.e. if there is not exception to throw.
     */
    return;
}


/* --------------- [ MACI DLL support functions ] -----------------*/
#include <maciACSComponentDefines.h>
MACI_DLL_SUPPORT_FUNCTIONS(ErrorComponent)
/* ----------------------------------------------------------------*/


/*___oOo___*/



