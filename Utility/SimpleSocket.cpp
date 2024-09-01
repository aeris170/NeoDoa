#include <Utility/SimpleSocket.hpp>

#include <utility>

static zmq::socket_type GetZMQSocketType(SocketType type) {
	switch (type) {
	case SocketType::Request:
		return zmq::socket_type::req;
	case SocketType::Reply:
		return zmq::socket_type::rep;
	case SocketType::Dealer:
		return zmq::socket_type::dealer;
	case SocketType::Router:
		return zmq::socket_type::router;
	case SocketType::Publisher:
		return zmq::socket_type::pub;
	case SocketType::Subscriber:
		return zmq::socket_type::sub;
	case SocketType::XPublisher:
		return zmq::socket_type::xpub;
	case SocketType::XSubscriber:
		return zmq::socket_type::xsub;
	case SocketType::Push:
		return zmq::socket_type::push;
	case SocketType::Pull:
		return zmq::socket_type::pull;
	}
	std::unreachable();
}

static zmq::send_flags GetZMQSendFlag(SendFlag flag) {
	switch (flag) {
	case SendFlag::None:
		return zmq::send_flags::none;
	case SendFlag::DontWait:
		return zmq::send_flags::dontwait;
	case SendFlag::SendMore:
		return zmq::send_flags::sndmore;
	}
	std::unreachable();
}

static zmq::recv_flags GetZMQRecvFlag(ReceiveFlag flag) {
	switch (flag) {
	case ReceiveFlag::None:
		return zmq::recv_flags::none;
	case ReceiveFlag::DontWait:
		return zmq::recv_flags::dontwait;
	}
	std::unreachable();
}


void Poller::Register(SimpleSocket* socket) noexcept {
	items.emplace_back(socket->socket, 0, ZMQ_POLLIN, 0);
}
int Poller::Poll(std::chrono::milliseconds timeout) noexcept {
	return zmq::poll(items.data(), items.size(), timeout);
}

SimpleSocket::SimpleSocket(SocketType type) noexcept :
    socket(context, GetZMQSocketType(type)) {}
SimpleSocket::~SimpleSocket() noexcept {}

void SimpleSocket::Send(std::string_view message, SendFlag flag) noexcept {
	socket.send(zmq::buffer(message), GetZMQSendFlag(flag));
}
std::string_view SimpleSocket::Receive(ReceiveFlag flag) noexcept {
	auto t = socket.recv(message, GetZMQRecvFlag(flag));
	if (t.has_value()) {
		return message.to_string_view();
	} else {
		return "";
	}
}

ServerSocket::ServerSocket(std::string_view address, SocketType type) noexcept :
	SimpleSocket(type) {
	socket.bind(address.data());
}
ClientSocket::ClientSocket(std::string_view address, SocketType type) noexcept :
	SimpleSocket(type) {
	socket.connect(address.data());
}