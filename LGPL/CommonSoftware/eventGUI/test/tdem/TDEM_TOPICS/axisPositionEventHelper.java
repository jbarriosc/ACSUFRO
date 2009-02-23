package tdem.TDEM_TOPICS;


/**
 *	Generated from IDL definition of struct "axisPositionEvent"
 *	@author JacORB IDL compiler 
 */

public final class axisPositionEventHelper
{
	private static org.omg.CORBA.TypeCode _type = null;
	public static org.omg.CORBA.TypeCode type ()
	{
		if (_type == null)
		{
			_type = org.omg.CORBA.ORB.init().create_struct_tc(tdem.TDEM_TOPICS.axisPositionEventHelper.id(),"axisPositionEvent",new org.omg.CORBA.StructMember[]{new org.omg.CORBA.StructMember("ref", org.omg.CORBA.ORB.init().get_primitive_tc(org.omg.CORBA.TCKind.from_int(7)), null),new org.omg.CORBA.StructMember("act", org.omg.CORBA.ORB.init().get_primitive_tc(org.omg.CORBA.TCKind.from_int(7)), null),new org.omg.CORBA.StructMember("key", org.omg.CORBA.ORB.init().get_primitive_tc(org.omg.CORBA.TCKind.from_int(3)), null),new org.omg.CORBA.StructMember("timestamp", alma.ACS.TimeIntervalHelper.type(), null),new org.omg.CORBA.StructMember("actAz", org.omg.CORBA.ORB.init().get_primitive_tc(org.omg.CORBA.TCKind.from_int(7)), null),new org.omg.CORBA.StructMember("actEl", org.omg.CORBA.ORB.init().get_primitive_tc(org.omg.CORBA.TCKind.from_int(7)), null),new org.omg.CORBA.StructMember("actRA", org.omg.CORBA.ORB.init().get_primitive_tc(org.omg.CORBA.TCKind.from_int(7)), null),new org.omg.CORBA.StructMember("actDec", org.omg.CORBA.ORB.init().get_primitive_tc(org.omg.CORBA.TCKind.from_int(7)), null),new org.omg.CORBA.StructMember("trackingError", org.omg.CORBA.ORB.init().get_primitive_tc(org.omg.CORBA.TCKind.from_int(7)), null)});
		}
		return _type;
	}

	public static void insert (final org.omg.CORBA.Any any, final tdem.TDEM_TOPICS.axisPositionEvent s)
	{
		any.type(type());
		write( any.create_output_stream(),s);
	}

	public static tdem.TDEM_TOPICS.axisPositionEvent extract (final org.omg.CORBA.Any any)
	{
		return read(any.create_input_stream());
	}

	public static String id()
	{
		return "IDL:tdem/TDEM_TOPICS/axisPositionEvent:1.0";
	}
	public static tdem.TDEM_TOPICS.axisPositionEvent read (final org.omg.CORBA.portable.InputStream in)
	{
		tdem.TDEM_TOPICS.axisPositionEvent result = new tdem.TDEM_TOPICS.axisPositionEvent();
		result.ref=in.read_double();
		result.act=in.read_double();
		result.key=in.read_long();
		result.timestamp=in.read_longlong();
		result.actAz=in.read_double();
		result.actEl=in.read_double();
		result.actRA=in.read_double();
		result.actDec=in.read_double();
		result.trackingError=in.read_double();
		return result;
	}
	public static void write (final org.omg.CORBA.portable.OutputStream out, final tdem.TDEM_TOPICS.axisPositionEvent s)
	{
		out.write_double(s.ref);
		out.write_double(s.act);
		out.write_long(s.key);
		out.write_longlong(s.timestamp);
		out.write_double(s.actAz);
		out.write_double(s.actEl);
		out.write_double(s.actRA);
		out.write_double(s.actDec);
		out.write_double(s.trackingError);
	}
}
