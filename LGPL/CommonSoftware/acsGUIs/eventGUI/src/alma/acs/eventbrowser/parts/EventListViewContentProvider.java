/*******************************************************************************
 * ALMA - Atacama Large Millimeter Array
 * Copyright (c) ESO - European Southern Observatory, 2011
 * (in the framework of the ALMA collaboration).
 * All rights reserved.
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307  USA
 *******************************************************************************/
package alma.acs.eventbrowser.parts;

import org.eclipse.jface.viewers.IStructuredContentProvider;
import org.eclipse.jface.viewers.Viewer;

import alma.acs.eventbrowser.model.EventData;
import alma.acs.eventbrowser.model.EventModel;


public class EventListViewContentProvider implements IStructuredContentProvider {

	private final EventModel eventModel;
		
	public EventListViewContentProvider(EventModel eventModel) {
		this.eventModel = eventModel;
	}

	@Override
	public void inputChanged(Viewer viewer, Object oldInput, Object newInput) {
		// TODO Auto-generated method stub

	}

	@Override
	public Object[] getElements(Object inputElement) {
		if (eventModel.getEventQueue().isEmpty()) {
			return new EventData[0];
		}
		EventData[] ed = eventModel.getEventQueue().toArray(new EventData[0]);
		eventModel.getEventQueue().clear();
		return ed;
	}

	@Override
	public void dispose() {
		// TODO Auto-generated method stub
		
	}
	
}
