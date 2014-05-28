#pragma once

#include <vector>
#include <string>
#include <typeinfo>
#include <map>
#include <mutex>

// TOOD: DEBUG_MODE is used as macro to check if in debug node or not, must be changed to correct macro name
/* A completely passive thread safe message system, depends on other systems to periodically checking for messages */
class MessageSystem
{
public:
	/* The class used to send messages with, provides extra checks and debug information in debug mode */
	struct Message
	{
		/* Constructor for message
		 * 
		 * #_to, the selfprovided name of the subsystem the message is sent to
		 * #_info, general info about the info. Preferable used to distinguse between different types of messages
		 * #_message, whatever data that is needed to be sent. A copy of the original data will be used and sent */
		template< typename T >
		Message( std::string _to, std::string _info, T& _message )
			: to(_to), info(_info), have_message(true)
		{
			message = new T(_message);
#ifdef DEBUG_MODE
			type = typeid(T).name();
#endif
		}

		/* Constructor for message
		 * 
		 * #_to, the selfprovided name of the subsystem the message is sent to
		 * #_info, a string containing data to be sent */
		Message( std::string _to, std::string _info )
			: to(_to), info(_info), have_message(false)
		{
			message = nullptr;
#ifdef DEBUG_MODE
			type = "no_type";
#endif
		}

		/* Function called by the receiver of the message to gain access to the data
		 * If not called for a message it will result in a memory leak
		 *
		 * #returns a copy of the data contained in the message
		 *
		 * #throws string if:
		 *		the provided data type is incorrect (only in debug mode)
		 *		it has already been received
		 *		the message is a null pointer */
		template< typename T >
		T receiveMessage() 
		{
#ifdef DEBUG_MODE
			if( type != typeid(T).name() )
				throw std::string( "Incorrect type" );
#endif
			if( !have_message )
				throw std::string( "Have no message to deliver" );
			else if( message == nullptr )
				throw std::string( "Error null pointer" );
			T* ptr = reinterpret_cast<T*>(message);
			T t( *ptr );
			delete message;
			message = nullptr;
			return t;
		}

		/* Function called by the receiver of the message to gain a peek at the data
		 * Warning, if recieveMessage is never called on message it will result in a memory leek
		 *
		 * #returns a copy of the data contained in the message
		 *
		 * #throws string if:
		 *		the provided data type is incorrect (only in debug mode)
		 *		it has already been received
		 *		the message is a null pointer */
		template< typename T >
		T peekMessage() 
		{
#ifdef DEBUG_MODE
			if( type != typeid(T).name() )
				throw std::string( "Incorrect type" );
#endif
			if( !have_message )
				throw std::string( "Have no message to deliver" );
			else if( message == nullptr )
				throw std::string( "Error null pointer" );
			T* ptr = reinterpret_cast<T*>(message);
			T t( *ptr );
			return t;
		}

		std::string to;
		std::string info;
	private:
#ifdef DEBUG_MODE
		std::string type;
#endif
		bool have_message;
		void* message;
	};

	/* To be used in a for loop to access all messages
	 *
	 * #_to, the name the sender have used in the messages to the system
	 * 
	 * #returns an int specifying the number of messages specified to _to */
	int messageCount( std::string _to ); 

	/* To access messages that have been checked to be in existence by MessageCount
	 *
	 * #_to, the name the sender have used in the messages to the system
	 *
	 * #returns the message sent to _to
	 *
	 * #throws string if there is no messages to _to */
	Message getMessage( std::string _to );

	/* Adds a message to be delivered, also uses mutex locks so can take longer than expected
	 * (no race condition is possible)
	 *
	 * #_message, an instance of the class Message that will be sent to the receiver */
	void addMessage( Message _message );

private:
	std::mutex mutex;
	std::map< std::string, int > recipients;
	std::vector< std::vector< Message > > messages;
};