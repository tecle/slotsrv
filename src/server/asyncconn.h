#ifndef ASYNCCONN_H
#define ASYNCCONN_H

#include <array>
#include <memory>
#include <iostream>
#include <http/httpresponsepacket.h>
#include <handlers/ihandler.h>
#include <util/databuffer.h>

BEGIN_NAMESPACE(cgserver)
// no matter what happend, server will colse socket after Xs
#define CONN_EXPIRE_TIME 5

class AsyncConn: public std::enable_shared_from_this< AsyncConn >
{
public:
	explicit AsyncConn(asio_service &service);
        ~AsyncConn();
	void setHandler(IHandler *handler);

	void startConn();
	asio_socket &getSocket();
	HttpResponsePacket &getResponse();

private:
	bool process();
	void afterWrite(const asio_error &err, size_t write_len);
	void afterRead(const asio_error &err, size_t read_len);
	bool validatePacket(HTTPPacket &packet) const;
        void disconnect(const asio_error &err);
        void closeSocket();

	HttpResponsePacket _resp;
	asio_service &_service;
	asio_socket _socket;
	DataBuffer _input;
	DataBuffer _output;
	bool _finish;
	bool _limit;
        bool _connected;
        asio_deadline_timer_ptr _timer;

	IHandler *_handler;
};
typedef std::shared_ptr<AsyncConn> AsyncConnPtr;
END_NAMESPACE
#endif
