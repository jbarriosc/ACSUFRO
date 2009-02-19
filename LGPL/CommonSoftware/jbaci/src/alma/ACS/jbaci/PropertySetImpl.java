/*******************************************************************************
 * ALMA - Atacama Large Millimiter Array
 * (c) European Southern Observatory, 2002
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
 */

package alma.ACS.jbaci;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.Map;

import org.omg.CORBA.Any;
import org.omg.CORBA.TypeCode;
import org.omg.CosPropertyService.ConflictingProperty;
import org.omg.CosPropertyService.ExceptionReason;
import org.omg.CosPropertyService.FixedProperty;
import org.omg.CosPropertyService.InvalidPropertyName;
import org.omg.CosPropertyService.MultipleExceptions;
import org.omg.CosPropertyService.PropertiesHolder;
import org.omg.CosPropertyService.PropertiesIteratorHolder;
import org.omg.CosPropertyService.Property;
import org.omg.CosPropertyService.PropertyException;
import org.omg.CosPropertyService.PropertyNamesHolder;
import org.omg.CosPropertyService.PropertyNamesIteratorHolder;
import org.omg.CosPropertyService.PropertyNotFound;
import org.omg.CosPropertyService.PropertySetPOA;
import org.omg.CosPropertyService.ReadOnlyProperty;
import org.omg.CosPropertyService.UnsupportedProperty;
import org.omg.CosPropertyService.UnsupportedTypeCode;


/**
 * Implementation of <code>org.omg.CosPropertyService.PropertySet</code>.
 * @author <a href="mailto:cmenayATcsrg.inf.utfsm.cl">Camilo Menay</a>
 * @author <a href="mailto:cmmaureirATinf.utfsm.cl">Cristian Maureira</a>
 * @version $id$
 */
public class PropertySetImpl extends PropertySetPOA
								 {

	private Map propMap; 
	private ArrayList allowed_property_types;
	private ArrayList allowed_properties;
	
	
	//others constructors might be created on demand
	
	public PropertySetImpl(){

		allowed_property_types = new ArrayList();
		allowed_properties = new ArrayList();
		propMap = new HashMap();
	}
	
	public PropertySetImpl(Property[] propSet) throws MultipleExceptions{

		allowed_property_types = new ArrayList();
		allowed_properties = new ArrayList();
		propMap = new HashMap();
		define_properties(propSet);
	}
	
	/***checkTypeAndProperty According to documentation, we jave to check if the new property will be valid
	 * according to the constraints that might exists. If both allowed lists are empty, then there aren't 
	 * any constraints.
	 * @param name
	 * @param value
	 * @return boolean True if the type and name of property are valid and can be added
	 * @throws InvalidPropertyName
	 * @throws UnsupportedTypeCode
	 * @throws UnsupportedProperty
	 */
	private boolean checkTypeAndProperty(String name, Any value) throws InvalidPropertyName, UnsupportedTypeCode, UnsupportedProperty{
		
		if(name==null || name.length()==0)
			throw new InvalidPropertyName();
		
		if(value==null)
			return false;
		if(allowed_property_types.size()==0 && allowed_properties.size()==0)
			return true;
		if(!isValidType(value.type()))
			throw new UnsupportedTypeCode();
		if(!isValidProperty(name))
			throw new UnsupportedProperty();
				
		return true;
	}
	
	private boolean isValidProperty(String name) {
		
		if (allowed_properties.size()==0)
			return true;
		for (int i=0;i<allowed_properties.size();i++)
			if(allowed_properties.get(i).toString()==name)
			  return true;
		
		return false;
	}

	private boolean isValidType(TypeCode code) {
		
		if (allowed_property_types.size()==0)
			return true;
		for (int i=0;i<allowed_property_types.size();i++)
			if(((TypeCode)allowed_property_types.get(i)).kind()==code.kind())
			  return true;
		
		return false;
	}

	public void define_properties(Property[] propSet) throws MultipleExceptions {
		
		ArrayList errorList = new ArrayList();
		for (int i=0;i<propSet.length;i++)
			try {
				define_property(propSet[i].property_name,propSet[i].property_value);
			} catch (ConflictingProperty e) {
				errorList.add(new PropertyException(ExceptionReason.conflicting_property,propSet[i].property_name) );
			} catch (UnsupportedProperty e) {
				errorList.add(new PropertyException(ExceptionReason.unsupported_property,propSet[i].property_name) );
			} catch (UnsupportedTypeCode e) {
				errorList.add(new PropertyException(ExceptionReason.unsupported_type_code,propSet[i].property_name) );
			} catch (ReadOnlyProperty e) {
				errorList.add(new PropertyException(ExceptionReason.read_only_property,propSet[i].property_name) );
			} catch (InvalidPropertyName e) {
				errorList.add(new PropertyException(ExceptionReason.invalid_property_name,propSet[i].property_name) );
			}	
			
			if(errorList.size()>0) //TODO: check if this cast is possibly
			throw new MultipleExceptions((PropertyException[])errorList.toArray());
	}



	public void define_property(String name, Any value)
			throws ConflictingProperty, UnsupportedProperty,
			UnsupportedTypeCode, ReadOnlyProperty, InvalidPropertyName {

		checkTypeAndProperty(name, value);
		if(  propMap.containsKey(name) )
		{
			Property p =  new Property(name,(Any)propMap.get(name)) ;
			if (value.type().kind() != p.property_value.type().kind())
				throw new ConflictingProperty();
			else {
				propMap.put(name, value);
			}
		} else {
			propMap.put(name, value);
		}

	}

	public boolean delete_all_properties() {
		// TODO Auto-generated method stub
		return false;
	}

	public void delete_properties(String[] arg0) throws MultipleExceptions {
		// TODO Auto-generated method stub
		
	}

	public void delete_property(String arg0) throws FixedProperty, PropertyNotFound, InvalidPropertyName {
		// TODO Auto-generated method stub
		
	}

	public void get_all_properties(int arg0, PropertiesHolder arg1, PropertiesIteratorHolder arg2) {
		// TODO Auto-generated method stub
		
	}

	public void get_all_property_names(int arg0, PropertyNamesHolder arg1, PropertyNamesIteratorHolder arg2) {
		// TODO Auto-generated method stub
		
	}

	public int get_number_of_properties() {
		// TODO Auto-generated method stub
		return 0;
	}

	public boolean get_properties(String[] arg0, PropertiesHolder arg1) {
		// TODO Auto-generated method stub
		return false;
	}

	public Any get_property_value(String arg0) throws PropertyNotFound, InvalidPropertyName {
		// TODO Auto-generated method stub
		return null;
	}

	public boolean is_property_defined(String arg0) throws InvalidPropertyName {
		// TODO Auto-generated method stub
		return false;
	}


}
