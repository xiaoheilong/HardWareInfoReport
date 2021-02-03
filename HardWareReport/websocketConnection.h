#pragma once
#include "websocketpp/config/asio_no_tls_client.hpp"
#include "websocketpp/client.hpp"
#include <iostream>
#include <functional>
namespace WebSocketNamsSpace{
typedef websocketpp::client<websocketpp::config::asio_client> client;

using websocketpp::lib::placeholders::_1;
using websocketpp::lib::placeholders::_2;
typedef websocketpp::config::asio_client::message_type::ptr message_ptr;

//typedef  void (*SocketCallback)(std::string string ,int error);

class OutterInterfaceConnection{
public:
    OutterInterfaceConnection(){

    }
    virtual ~OutterInterfaceConnection(){

    }
public:
    virtual void ConnectedCallback(std::string msg , int error) = 0;
    virtual void DisconnectedCallback(std::string msg , int error) = 0;
    virtual void MessageCallback(std::string msg , int error) = 0;
    virtual void FailureCallback(std::string msg , int error) = 0;
    virtual void InterruptCallback(std::string msg , int error) = 0;
};


class WsAppConnection
{
public:
    WsAppConnection();
    ~WsAppConnection();
public:
    int  init(std::string wsUrl);
    void connect();
    void close();
    void terminate();
    bool isConnected();
    int  Send(std::string msg);
    void SetCallback(OutterInterfaceConnection *outter);
protected:
    void on_open(websocketpp::connection_hdl hdl);

    void on_message(websocketpp::connection_hdl hdl, message_ptr msg);

    void on_close(websocketpp::connection_hdl hdl);

    void on_failure(websocketpp::connection_hdl hdl);

    void on_pong(websocketpp::connection_hdl hdl , std::string msg);

    void on_ping(websocketpp::connection_hdl hdl, std::string msg);

    void onTimer(const boost::system::error_code& ec);

    void On_Interrupt(websocketpp::connection_hdl hdl);
private:
	client c;
	websocketpp::lib::shared_ptr<websocketpp::lib::thread> thread_;
	websocketpp::connection_hdl hdl_;
    std::string m_uri;
    //std::shared_ptr<boost::asio::deadline_timer> m_timer;
    bool m_isConnected = false;
    OutterInterfaceConnection *m_outter;
};


}
