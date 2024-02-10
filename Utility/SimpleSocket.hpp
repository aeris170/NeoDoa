#pragma once

#include <chrono>
#include <string_view>

#include <zmq.hpp>

struct SimpleSocket;

// Refer to ZeroMQ for what these are.
enum class SocketType {
	Request,
	Reply,
	Dealer,
	Router,
	Publisher,
	Subscriber,
	XPublisher,
	XSubscriber,
	Push,
	Pull
};

// Refer to ZeroMQ for what these are.
enum class SendFlag {
	None,
	DontWait,
	SendMore
};

// Refer to ZeroMQ for what these are.
enum class ReceiveFlag {
	None,
	DontWait
};

struct Poller {
	void Register(SimpleSocket* socket) noexcept;
	int Poll(std::chrono::milliseconds timeout = std::chrono::milliseconds{ -1 }) noexcept;

private:
	std::vector<zmq::pollitem_t> items;
};


struct SimpleSocket {

	explicit SimpleSocket(SocketType type) noexcept;
	virtual ~SimpleSocket() noexcept;

	void Send(std::string_view message, SendFlag flag = SendFlag::None) noexcept;
	std::string_view Receive(ReceiveFlag flag = ReceiveFlag::None) noexcept;

protected:
	zmq::context_t context{ 1 };
	zmq::socket_t socket;
	zmq::message_t message;

	friend struct Poller;
};

struct ServerSocket : SimpleSocket {

	explicit ServerSocket(std::string_view address, SocketType type) noexcept;
};

struct ClientSocket : SimpleSocket {

	explicit ClientSocket(std::string_view address, SocketType type) noexcept;
};