#include "MessageSystem.h"

#ifdef __DEBUG
TEST( MessageSystem, sendThenRecevieMessage )
{
	MessageSystem ms;
	ms.addMessage( MessageSystem::Message( "test", "just_testing", std::string("Hello World") ) );
	ASSERT_EQ( ms.messageCount( "test" ), 1 );

	MessageSystem::Message m = ms.getMessage( "test" );
	ASSERT_EQ( m.info, "just_testing" );

	std::string data = m.receiveMessage<std::string>();
	ASSERT_EQ( data, "Hello World" );
}
#endif
int MessageSystem::messageCount(std::string _to)
{
	if(recipients.count(_to) == 0)
		return 0;
	else
		return messages[recipients[_to]].size();
}

MessageSystem::Message MessageSystem::getMessage(std::string _to)
{
	mutex.lock();
	if(recipients.count(_to) == 0 || messages[recipients[_to]].size() == 0)
	{
		mutex.unlock();
		throw std::string("No messages to: " + _to);
	}
	Message result(messages[recipients[_to]].back());
	messages[recipients[_to]].pop_back();
	mutex.unlock();
	return result;
}

void MessageSystem::addMessage(MessageSystem::Message _message)
{
	mutex.lock();
	if(recipients.count(_message.to) == 0)
	{
		messages.push_back(std::vector< Message >());
		recipients.insert(std::pair<std::string, int>(_message.to, messages.size() - 1));
	}
	messages[recipients[_message.to]].push_back(_message);
	mutex.unlock();
}