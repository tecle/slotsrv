#include "httpserver.h"
#include "mysql/mysqlconnpool.h"
#include "util/config.h"
#include "util/luatoolfactory.h"
#include "util/processorfactory.h"
#include "util/databuffer.h"

BEGIN_NAMESPACE(cgserver)

void HttpHandler::operator() (const HttpRequest &req, HttpConnPtr conn) {
    // parse request.destination/ request.method
    CLOG(INFO) << "Request in:" << req.destination;
    HttpResponsePacket resp;
    HTTPPacket packet;
    packet.setMethod(HTTPPacket::HM_POST);
    if (parseURI(req.destination, packet)) {
        _handler->process(packet, resp);
    }
    DataBuffer buff;
    resp.encode(&buff);
    std::string out(buff.getData(), buff.getDataLen());
    conn->write(out);
}

bool HttpHandler::parseURI(const std::string &uri, HTTPPacket &packet) {
    int uriLen = uri.size();
    int qFlag = uri.find('?');
    if (qFlag == std::string::npos) {
	packet.setPath(uri.data(), uriLen);
	return true;
    }
    if (qFlag == 0) {
        return false;
    }
    packet.setPath(uri.data(), qFlag);
    if(!parseParam(uri.begin() + qFlag + 1, uri.end(), packet)){
	CLOG(WARNING) << "parse param from uri [" << uri << "] failed.";
	return false;
    }
    return true;
}

#define str_itr std::string::const_iterator
bool HttpHandler::parseParam(str_itr begin, str_itr end, HTTPPacket &packet){
    if (begin == end) return true;
    auto cur = begin;
    while((cur = locateChar(begin, end, '&'))!= end) {
        parseKV(begin, cur, packet);
        ++cur;
        begin = cur;
    }
    if (begin != end) {
        parseKV(begin, end, packet);
    }
    return true;
}

bool HttpHandler::parseKV(str_itr begin, str_itr end, HTTPPacket &packet) {
    if (begin == end) return true;
    auto pos = locateChar(begin, end, '=');
    if (pos != begin && pos != end) {
        packet.addParam(std::string(begin, pos), std::string(pos + 1, end));
    }
    return true;
}
#undef str_itr

bool HttpHandler::parseParam(const char *paramStr, int strLen, HTTPPacket &packet){
    if (strLen <= 0) return true;
    int remain = strLen;
    int pos;
    while((pos = locateChar(paramStr, '&')) >= 0){
	parseKV(paramStr, pos, packet);
	paramStr += pos + 1;
        remain -= (pos + 1);
    }
    if (*paramStr != '\0') {
	parseKV(paramStr, remain, packet);
    }
    return true;
}

void HttpHandler::parseKV(const char *begin, int kvLen, HTTPPacket &packet) {
    if (kvLen <= 0)
	return;
    int pos = locateChar(begin, '=');
    if (pos == kvLen -1)
	packet.addParam(std::string(begin, pos), "");
    else if (pos < 0)
	packet.addParam(std::string(begin, kvLen), "");
    else
	packet.addParam(std::string(begin, pos), std::string(begin + pos + 1, kvLen - pos -1));
}

std::string::const_iterator HttpHandler::locateChar(
    std::string::const_iterator begin, std::string::const_iterator end, char ch)
{
    while(begin != end) {
        if (*begin == ch) return begin;
        ++begin;
    }
    return end;
}

int HttpHandler::locateChar(const char *dest, char tar) {
    int ret = 0;
    while(*dest != '\0') {
	if (*dest == tar) return ret;
	++dest;
	++ret;
    }
    return -1;
}


void HttpHandler::log(const HttpStr &info) {
    CLOG(INFO) << info;
}

void HttpHandler::setBusinessHandler(IHandler *handler) {
    _handler = handler;
}

HttpServer::HttpServer(const std::string &addr, const std::string &port):
    _options(_httpHandler),
    _server(_options.address(addr).port(port))
{
}

HttpServer::~HttpServer(){
}

bool HttpServer::startServer() {
    if (!initResource()) {
        return false;
    }
    _server.run();
}

void HttpServer::stopServer() {
    _server.stop();
    if (_handler) {
        delete _handler;
    }
}

bool HttpServer::initResource() {
    CLOG(INFO) << "Init mysql pool.";
    if (!MysqlConnPool::getInstance().init()) {
	CLOG(ERROR) << "Init mysql pool failed.";
	return false;
    }
    CLOG(INFO) << "Init mysql pool done.";

    _handler = HandlerFactory::getHandler();
    if (!_handler->init()) {
	return false;
    }
    auto &cfg = Config::getInstance();
    int poolSize = cfg.getServerThreadPoolSize();

    if (cfg.enableLua()) {
        if (!LuaToolFactory::getInstance().init(poolSize + 1, cfg.getLuaPath())) {
            CLOG(ERROR) << "Init lua factory failed.";
            return false;
        }
    }

    _httpHandler.setBusinessHandler(_handler);

    CLOG(INFO) << "Init server success.";
    return true;
}

END_NAMESPACE
