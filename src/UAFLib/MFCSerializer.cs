using System;
using System.IO;
using System.Text;

namespace UAFLib
{
	public class MFCSerializer
	{
		public bool debug = false;
		Stream mStream;
		protected BinaryWriter writer = null;
		protected BinaryReader reader = null;
		private const int m_Index = 0; // actually never changes
		private bool mIsStoring;

		public MFCSerializer()
        {
			mStream = new MemoryStream();
			writer = new BinaryWriter(mStream);
			mIsStoring = true;

		}
		public MFCSerializer(Stream stream)
		{
			mStream = stream;
			reader = new BinaryReader(mStream);
			mIsStoring = false;
		}

		public Stream GetStream()
		{
			return mStream;
		}

		public bool IsStoring()
		{
			return mIsStoring;
		}

		/*public void Serialize(IArchiveSerialization obj)
		{
			obj.Serialize(this);
		}*/

		//////////////////////////////////////////////////////
		// write functions

		public void writeChar(Char ch)
		{
			//writer.Write(ch);
			writer.Write(Convert.ToInt16(ch));
		}

		public void writeUInt16(UInt16 n)
		{
			writer.Write(n);
		}

		public void writeInt16(Int16 n)
		{
			writer.Write(n);
		}

		public void writeUInt32(UInt32 n)
		{
			writer.Write(n);
		}

		public void writeInt32(Int32 n)
		{
			writer.Write(n);
		}

		public void writeUInt64(UInt64 n)
		{
			writer.Write(n);
		}

		public void writeInt64(Int64 n)
		{
			writer.Write(n);
		}

		public void writeSingle(Single d)
		{
			writer.Write(d);
		}

		public void writeDouble(Double d)
		{
			writer.Write(d);
		}

		public void writeDecimal(Decimal d)
		{
			// store decimals as Int64
			Int64 n = Decimal.ToOACurrency(d);
			writer.Write(n);
		}

		public void writeDateTime(DateTime dt)
		{
			writer.Write(dt.ToBinary());
		}

		public void writeBool(Boolean b)
		{
			writer.Write(b);
		}

		public void writeString(string s)
		{
			this.writeByte(Convert.ToInt32(s.Length));
			writer.Write(s.ToCharArray());
		}

		public void writeGuid(Guid guid)
		{
			byte[] bytes = guid.ToByteArray();
			writeBytes(bytes);
		}

		public void writeBytes(Byte[] buffer)
		{
			writer.Write(buffer);
		}

		public void writeByte(int i)
		{
			writer.Write((byte)i);
		}

		///////////////////////////////////////////////////
		// Read functions

		public string readString()
		{
			int length = 0;
			length = readByte();

			char[] ch = new char[length];

			reader.Read(ch, m_Index, length);

			StringBuilder sb = new StringBuilder();
			sb.Append(ch);
			if (this.debug) {
				System.Console.WriteLine("ReadString: " + sb.ToString());
			}
			return sb.ToString();
		}

		public UInt16 readUInt16()
		{
			byte[] bytes = new byte[2];
			reader.Read(bytes, m_Index, 2);
			return BitConverter.ToUInt16(bytes, 0);
		}

		public Int16 readInt16()
		{
			byte[] bytes = new byte[2];
			reader.Read(bytes, m_Index, 2);
			return BitConverter.ToInt16(bytes, 0);
		}

		public UInt32 readUInt32()
		{
			byte[] bytes = new byte[4];
			reader.Read(bytes, m_Index, 4);
			if (this.debug)
			{
				System.Console.WriteLine("ReadUInt32 " + BitConverter.ToUInt32(bytes, 0));
			}
			return BitConverter.ToUInt32(bytes, 0);
		}

		public Int32 readInt32()
		{
			byte[] bytes = new byte[4];
			reader.Read(bytes, m_Index, 4);
			if (this.debug)
			{
				System.Console.WriteLine("ReadUInt32 " + BitConverter.ToInt32(bytes, 0));
			}
			return BitConverter.ToInt32(bytes, 0);
		}

		public UInt64 readUInt64()
		{
			byte[] bytes = new byte[8];
			reader.Read(bytes, m_Index, 8);
			return BitConverter.ToUInt64(bytes, 0);
		}

		public Int64 readInt64()
		{
			byte[] bytes = new byte[8];
			reader.Read(bytes, m_Index, 8);
			return BitConverter.ToInt64(bytes, 0);
		}

		public Char readChar()
		{
			Int16 n;
			n = readInt16();
			return Convert.ToChar(n);

			/* direct reading as char doesn't work for some reason
				Sometimes it works, but sometimes the character
				takes up only one byte in the buffer and it seems
				to depend on what comes before and after the item in the
								buffer

			*/

			//			byte[] bytes = new byte[2];
			//			reader.Read(bytes, m_Index, 2);
			//			ch = BitConverter.ToChar(bytes, 0);
		}

		public float readFloat()
		{
			byte[] bytes = new byte[4];
			reader.Read(bytes, m_Index, 4);
			if (this.debug)
			{
				System.Console.WriteLine("ReadFloat " + BitConverter.ToSingle(bytes, 0));
			}
			return BitConverter.ToSingle(bytes, 0);
		}

		public double readDouble()
		{
			byte[] bytes = new byte[8];
			reader.Read(bytes, m_Index, 8);
			return BitConverter.ToDouble(bytes, 0);
		}

		public double read()
		{
			byte[] bytes = new byte[8];
			reader.Read(bytes, m_Index, 8);
			return BitConverter.ToDouble(bytes, 0);
		}

		public Decimal readDecimal()
		{
			byte[] bytes = new byte[8];
			reader.ReadBytes(8);

			// BitConverter does not support direct conversion to
			// Decimal so use Int64
			Int64 n = BitConverter.ToInt64(bytes, 0);
			return Decimal.FromOACurrency(n);
		}

		public DateTime ReadDateTime()
		{
			Int64 l = readInt64();
			return DateTime.FromBinary(l);
		}

		public bool readBool()
		{
			byte[] bytes = new byte[1];
			reader.Read(bytes, m_Index, 1);
			return BitConverter.ToBoolean(bytes, 0);
		}

		public Guid readGuid()
		{
			byte[] bytes = new byte[16];
			bytes = readBytes(16);
			return new Guid(bytes);
		}

		public byte[] readBytes(int bufferSize)
		{
			byte[] buffer = new byte[bufferSize];
			reader.Read(buffer, m_Index, bufferSize);
			return buffer;
		}
		public int readByte()
		{
			byte b = readBytes(1)[0];
			if (this.debug)
			{
				System.Console.WriteLine("ReadByte " + b);
			}
			return b;
		}
	}
}