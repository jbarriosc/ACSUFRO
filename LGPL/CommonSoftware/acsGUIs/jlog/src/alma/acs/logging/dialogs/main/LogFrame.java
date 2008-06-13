/*
 *    ALMA - Atacama Large Millimiter Array
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
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston, 
 *    MA 02111-1307  USA
 */
package alma.acs.logging.dialogs.main;

import java.awt.Dimension;
import java.awt.Toolkit;
import java.awt.event.WindowListener;
import java.io.File;
import java.util.logging.Logger;

import javax.swing.JFrame;
import javax.swing.JOptionPane;
import javax.swing.SwingUtilities;

import alma.acs.logging.AcsLogLevel;
import alma.acs.logging.ClientLogManager;

import com.cosylab.logging.LoggingClient;
import com.cosylab.logging.engine.log.LogTypeHelper;

/**
 * The window with all the  controls of the main GUI:
 * 
 * @author acaproni
 *
 */
public class LogFrame extends JFrame implements WindowListener {
	
	// A boolean to signal that the application is closing
	private volatile boolean closing =false;
	
	// The GUI showed into the frame
	private LoggingClient loggingClient;
	
	// The logger
	private Logger logger;
	
	// The Shutdown hook
	private ShutdownHook shutdownHook;
	
	/**
	 * Constructor
	 * Creates the main window and setup the panel with the controls.
	 * 
	 * @param filterFile A file of filters to load
	 *                   It can be null if there are no filters to load
	 * @param logFile A file of logs to load
	 *                It can be null if there are no logs to load
	 * @param discardLevel The discard level to set in the engine; 
	 *                     If <code>null</code> the level in the engine is not set and the default is used
	 * @param doNotConnect If <code>true</code> do not try to connect to ACS (i.e. start offline)
	 * @param unlimited If <code>true</code> the number of logs in memory is unlimited, 
	 *                  otherwise the default is used
	 */
	public LogFrame(
			File filterFile, 
			String logFileName, 
			LogTypeHelper discardLevel, 
			boolean doNotConnect, 
			boolean unlimited) {
		super();
		setName("LogFrame");
		
		logger = ClientLogManager.getAcsLogManager().getLoggerForApplication("Logging client GUI",true);
		initShutdownHook(); 
		
		initialize(discardLevel,unlimited);
		// Move the window to the center of the screen 
        Dimension screenSize = Toolkit.getDefaultToolkit().getScreenSize();
        Dimension windowSize = getSize();
        setLocation(
        		Math.max(0,(screenSize.width -windowSize.width)/2), 
        		Math.max(0,(screenSize.height-windowSize.height)/2));
        pack();
		setVisible(true);
		
		//	Load the filters (if any)
		if (filterFile!=null) {
			try {
				loggingClient.getLogEntryTable().getFilters().loadFilters(filterFile,true,null);
			} catch (Throwable t) {
				JOptionPane.showMessageDialog(null, "Error: "+t.getMessage(), "Error loading filters", JOptionPane.ERROR_MESSAGE);
			}
		}
		
		// If there is no file name in the command line then connect
		// the client to the CORBA notification channel
		if (logFileName==null) {
			if (!doNotConnect) {
				loggingClient.connect();
			}
		} else {
			loggingClient.getLCModel1().loadFromFile(logFileName);
		}
		
		// Get events from the main window
		this.addWindowListener(this);
		
	}
	
	/**
	 * Initialize the content of the frame
	 *
	 * @param discardLevel The discard level
	 * @param unlimited If <code>true</code> the number of logs in memory is unlimited, 
	 *                  otherwise the default is used
	 */
	private void initialize(LogTypeHelper discardLevel, boolean unlimited) {
		setTitle("LoggingClient");
		addWindowListener(this);
        
		// build the LoggingClient
        loggingClient = new LoggingClient(this, LoggingClient.DEFAULT_LOGLEVEL, discardLevel, unlimited); 
        if (loggingClient==null) {
        	throw new NullPointerException("The logging client is null");
        }
        this.setRootPane(loggingClient);
        // Enable the exit menu
        loggingClient.hideExitMenu(false);
	}
	
	/**
	 * Starts the application.
	 * @param args an array of command-line arguments
	 */
	public static void main(java.lang.String[] args)
	{
		// First check if there are parameter in the command line
		
		/** 
		 * If it is null then the user specified a file name in the
		 * command line
		 */
		String initLogFileName = null;
		
		/** 
		 * If it is null then the user specified a filter file name in the
		 * command line
		 */
		String initFilterFileName = null;
		
		/**
		 *  <code>true</code> if the user do not want the logging client tries to connect to ACS 
		 *  at startup
		 */
		boolean doNotConnect=false;
		
		/**
		 * <code>true</code> if the user does not want to limit the number of logs to  keep in memory
		 */
		boolean unlimited=false;
		
		/**
		 * The initial discard level.
		 * If it not set in the command line, the logging client starts
		 * with the default discard level
		 */
		LogTypeHelper initialDiscardLevel=LoggingClient.DEFAULT_DISCARDLEVEL;
		
		// First check if there are parameter in the command line
		if (args.length>7) {
			// Wrong number of params
			printUsage("cmd line too long");
			System.exit(-1);
		} else if (args.length>0) {
			// Retrieve the params
			for (int t=0; t<args.length; t++) {
				if (args[t].compareTo("-f")==0 || args[t].compareTo("--filter")==0) {
					t++;
					if (t<args.length) {
						initFilterFileName=args[t];
						System.out.println("Using filter file "+initFilterFileName);
					} else if (initFilterFileName!=null) {
						// A filter file was already defined
						printUsage("Two filter file names in cmd line");
						System.exit(-1);
					} else {
						// -f was the last param in the cmd
						printUsage("No filter file name after "+args[t-1]);
						System.exit(-1);
					}
				} else if (args[t].compareTo("-d")==0 || args[t].compareTo("--discard")==0) {
					t++;
					if (t<args.length) {
						initialDiscardLevel= LogTypeHelper.fromLogTypeDescription(args[t]);
						if (initialDiscardLevel==null && !(args[t].compareToIgnoreCase("None")==0)) {
							System.out.println("Invalid discard level "+args[t]);
							System.out.println("Valid discard levels are:");
							System.out.println("\tNone");
							for (LogTypeHelper logType: LogTypeHelper.values()) {
								System.out.println("\t"+logType);
							}
						} else {
							System.out.println("Using initial discard level: "+initialDiscardLevel);
						}
					} else {
						System.out.println("No discard level found in command line");
						System.exit(-1);
					}
				} else if (args[t].compareTo("-dnc")==0 || args[t].compareTo("--DoNotConnect")==0) {
					doNotConnect=true;
					System.out.println("Connection to ACS inhibited");
				} else if (args[t].compareTo("-u")==0 || args[t].compareTo("--unlimited")==0) {
					System.out.println("Number of logs in memory is unlimited");
					unlimited=true;
				} else {
					if (initLogFileName==null) {
						initLogFileName=args[t];
					} else {
						// A log file was already found!
						printUsage("Two log file names in cmd line");
						System.exit(-1);
					}
				}
			}
		}
		
		File logFile = null;
		if (initLogFileName!=null) {
			// Check if the file in the cmd line is readable
			logFile = new File(initLogFileName);
			if (!logFile.canRead()) {
				System.err.println("log file "+initLogFileName+" is unreadable!");
				initLogFileName=null;
				System.exit(-1);
			}
		}
		
		File filterFile = null;
		if (initFilterFileName!=null) {
			filterFile = new File(initFilterFileName);
			if (!filterFile.canRead()) {
				System.err.println("Filter file "+initFilterFileName+" is unreadable!");
				System.exit(-1);
			}
		}
		
		try
		{
			// Create the frame
			class FrameLauncher extends Thread {
				File f;
				String name;
				boolean offline;
				LogTypeHelper discard;
				boolean noLimit;
				public FrameLauncher(File fltFile, String initFileName, LogTypeHelper initDiscard, boolean noACS, boolean unlimit) {
					f=fltFile;
					name=initFileName;
					discard=initDiscard;
					offline=noACS;
					noLimit=unlimit;
				}
				public void run() {
					new LogFrame(f,name,discard,offline,noLimit);
				}
			}
			SwingUtilities.invokeLater(new FrameLauncher(filterFile,initLogFileName,initialDiscardLevel,doNotConnect,unlimited));
		}
		catch (Throwable exception)
		{
			System.err.println("Exception occurred in main() of LoggingFrame");
			exception.printStackTrace(System.out);
		}
	}
	
	/**
	 * Print the standard usage message if the parameters in the command
	 * line are wrong.
	 *
	 * @param errorMsg An optional error message to print
	 */
	private static void printUsage(String errorMsg) {
		if (errorMsg!=null) {
			System.out.println("Wrong parameters: "+errorMsg);
		}
		System.out.println("USAGE:");
		System.out.println("jlog [logFileName] [(-f|--filter) filterFileName] [(-d|--discard) (NONE|discard level)] [-dnc|--DoNotConnect] [-u||--unlimited]\n");
	}
	
	/**
	 * @see WindowListener
	 */
	public void windowActivated(java.awt.event.WindowEvent e) {}
	
	/**
	 * @see WindowListener
	 */
	public void windowOpened(java.awt.event.WindowEvent e) {}
	
	/**
	 * @see WindowListener
	 */
	public void windowClosed(java.awt.event.WindowEvent e) {
	    	 loggingClient=null;
	}
	
	/**
	 * @see WindowListener
	 */
	public void windowDeactivated(java.awt.event.WindowEvent e) {}
	
	/**
	 * @see WindowListener
	 */
	public void windowDeiconified(java.awt.event.WindowEvent e) {}
	
	/**
	 * @see WindowListener
	 */
	public void windowIconified(java.awt.event.WindowEvent e) {}

	/**
	 * @see WindowListener
	 */
	public void windowClosing(java.awt.event.WindowEvent e)	{
		if (closing) {
			return;
		}
		closeApplication();
	}
	
	/**
	 * Close the application
	 */
	public void closeApplication() {
		setVisible(false);
		closing=true;
		loggingClient.close(true);
		
		dispose();
	}
	
	/**
	 * Init the shutdown hook that intercept CTRL+C events
	 * and cleanly terminates the application
	 */
	private void initShutdownHook() {
		shutdownHook = new ShutdownHook(logger,"Logging client GUI", this);
		Runtime.getRuntime().addShutdownHook(shutdownHook);
	}
}
