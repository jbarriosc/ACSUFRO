/*
 * ALMA - Atacama Large Millimiter Array (c) European Southern Observatory, 2010
 * 
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by the Free
 * Software Foundation; either version 2.1 of the License, or (at your option)
 * any later version.
 * 
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
 * details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation, Inc.,
 * 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 * 
 */
package com.cosylab.logging.engine.cache;

import java.io.File;
import java.io.IOException;
import java.util.Random;

/**
 * A default implementation of {@link ILogQueueFileHandler} to create 
 * and delete cache files locally.
 * 
 * @author acaproni
 * @since ACS 9.0
 */
public class LogQueueFileHandlerImpl implements ILogQueueFileHandler {
	
	/**
	 * The prefix of each file of the cache.
	 */
	private final String fileNamePrefix;
	
	/**
	 * The max size of the files of the cache.
	 */
	private final long maxFileSize;
	
	/**
	 * The random number generator
	 */
	private final Random randomNumGenerator = new Random(System.currentTimeMillis());
	
	/**
	 * Build the handler with the default size  and prefix.
	 * 
	 * The max size of each file of the cache is calculated in the following way:
	 * 1. if the java property is present, the size is taken from such a property
	 * 2. the default size is used
	 */
	public LogQueueFileHandlerImpl() {
		this(Long.getLong(MAXSIZE_PROPERTY_NAME,DEFAULT_SIZE));
	}
	
	/**
	 * Build the handler with the passed size for the files.
	 * 
	 * @param maxFileSize The max size of the files of the cache.
	 */
	public LogQueueFileHandlerImpl(long maxFileSize) {
		this(maxFileSize,"jlogEngineCache");
	}
	
	/**
	 * Build the handler with the default size for the files
	 * and the passed prefix
	 * 
	 * @param maxFileSize The max size of the files of the cache.
	 * @param prefix The prefix of the name of the cache files
	 */
	public LogQueueFileHandlerImpl(String prefix) {
		this(Long.getLong(MAXSIZE_PROPERTY_NAME,DEFAULT_SIZE),prefix);
	}
	
	/**
	 * Build the handler with the passed size and prefix.
	 * 
	 * @param maxFileSize The max size of the files of the cache.
	 * @param prefix The prefix of the name of the cache files
	 */
	public LogQueueFileHandlerImpl(long maxFileSize, String prefix) {
		this.maxFileSize=maxFileSize;
		fileNamePrefix=prefix;
	}

	/**
	 * @return the maximum size of each file of the cache
	 */
	@Override
	public long getMaxFileSize() {
		return maxFileSize;
	}

	/** 
	 * @see com.cosylab.logging.engine.cache.ILogQueueFileHandler#fileProcessed(java.io.File, java.lang.String, java.lang.String)
	 */
	@Override
	public void fileProcessed(File filePointer, String minTime, String maxTime) {
		if (filePointer==null) {
			throw new IllegalArgumentException("The file can't be null");
		}
		if (!filePointer.delete()) {
			System.err.println("Error deleting "+filePointer.getAbsolutePath());
		}
	}

	/**
	 * Attempts to create the file for the strings in several places
	 * before giving up.
	 * 
	 * @see com.cosylab.logging.engine.cache.ILogQueueFileHandler#getNewFile()
	 */
	@Override
	public File getNewFile() throws IOException {
		String name=null;
		File f=null;
		try {
			// Try to create the file in $ACS_TMP
			String acstmp = System.getProperty("ACS.tmp");
			if (!acstmp.endsWith(File.separator)) {
				acstmp=acstmp+File.separator;
			}
			File dir = new File(acstmp);
			f = File.createTempFile(fileNamePrefix,".tmp",dir);
			name=f.getAbsolutePath();
		} catch (IOException ioe) {
			// An error :-O
			String homeDir = System.getProperty("user.dir");
			// Check if the home dir is writable
			File homeDirFile = new File(homeDir);
			if (homeDirFile.isDirectory() && homeDirFile.canWrite()) {
				do {
					// Try to create the file in the home directory
					int random = randomNumGenerator.nextInt();
					name = homeDir + File.separator+fileNamePrefix+random+".jlog";
					f = new File(name);
				} while (f.exists());
			} else {
				// The home folder is not writable: try to get a system temp file
				f=File.createTempFile(fileNamePrefix,".tmp");
				name=f.getAbsolutePath();
			}
		}
		if (f!=null) {
			f.deleteOnExit();
		}
		return f;
	}
}
