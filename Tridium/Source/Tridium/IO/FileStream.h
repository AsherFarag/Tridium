#pragma once
#include <Tridium/IO/IOStream.h>
#include <Tridium/IO/FilePath.h>
#include <fstream>

namespace Tridium {

	//=================================================================================================
	// File Input Stream: Implementation of IInputStream for reading from files.
	//=================================================================================================
	class FileInputStream : public IInputStream
	{
	public:

		//=============================================================================================
		NON_COPYABLE( FileInputStream );

		//=============================================================================================
		FileInputStream( FilePath a_Path )
			: m_Path( std::move( a_Path ) ), m_Stream( m_Path.Native(), std::ifstream::in | std::ifstream::binary )
		{}

		//=============================================================================================
		~FileInputStream()
		{
			Close();
		}

		//=================================================================================================
		// Returns true if the stream is valid and can be read from.
		bool Valid() const override 
		{ 
			return m_Stream.is_open() && m_Stream.good();
		}

		//=================================================================================================
		// Returns the current position in the stream that will be read from (in bytes from the start).
		size_t StreamPosition() override
		{
			return m_Stream.tellg(); 
		}

		//=================================================================================================
		// Seeks to the specified position in the stream. Returns true on success.
		bool Seek( size_t a_Position ) override 
		{ 
			m_Stream.seekg( a_Position );
			return Valid();
		}

		//=================================================================================================
		// Reads raw data from the stream and writes it into 'a_Destination'. Returns true on success.
		bool ReadRaw( byte_t* a_Destination, size_t a_Size ) override
		{
			m_Stream.read( ReinterpretCast<char*>( a_Destination ), a_Size );
			return Valid();
		}

		//=================================================================================================
		// Returns the file path associated with this stream.
		const FilePath& Path() const 
		{ 
			return m_Path;
		}

		//=================================================================================================
		// Closes the stream if it is open.
		void Close()
		{
			if ( m_Stream.is_open() )
			{
				m_Stream.close();
			}
		}

	private:

		//=================================================================================================
		FilePath m_Path;
		std::ifstream m_Stream;

	};


	class FileOutputStream : public IOutputStream
	{
	public:

		//=============================================================================================
		NON_COPYABLE( FileOutputStream );

		//=============================================================================================
		FileOutputStream( FilePath a_Path )
			: m_Path( std::move( a_Path ) )
		{
			m_Stream = std::ofstream( m_Path.Native(), std::ofstream::out | std::ofstream::binary );
		}

		//=============================================================================================
		~FileOutputStream()
		{
			Close();
		}

		//=================================================================================================
		// Returns true if the stream is valid and can be written to.
		bool Valid() const override 
		{ 
			return m_Stream.is_open() && m_Stream.good();
		}

		//=================================================================================================
		// Returns the current position in the stream that will be written to (in bytes from the start).
		size_t StreamPosition() override
		{
			return m_Stream.tellp();
		}

		//=================================================================================================
		// Seeks to the specified position in the stream. Returns true on success.
		bool Seek( size_t a_Position ) override
		{ 
			m_Stream.seekp( a_Position ); 
			return Valid();
		}

		//=================================================================================================
		// Writes raw data to the stream. Returns true on success.
		bool WriteRaw( const byte_t* a_Data, size_t a_Size ) override
		{
			m_Stream.write( ReinterpretCast<const char*>( a_Data ), a_Size );
			return Valid();
		}

		//=================================================================================================
		// Returns the file path associated with this stream.
		const FilePath& Path() const 
		{ 
			return m_Path;
		}

		//=================================================================================================
		// Closes the stream if it is open. This serializes any buffered data to the file.
		void Close()
		{
			if ( m_Stream.is_open() )
			{
				m_Stream.close();
			}
		}

	private:

		//=================================================================================================
		FilePath m_Path;
		std::ofstream m_Stream;

	};

} // namespace Tridium