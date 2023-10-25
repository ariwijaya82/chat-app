#include "controller.hpp"

#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>
#include <nlohmann/json.hpp>

#include <iostream>
#include <vector>
#include <thread>
#include <chrono>

using namespace std;

using json = nlohmann::json;
typedef websocketpp::server<websocketpp::config::asio> server;
using connection_hdl = websocketpp::connection_hdl;
using websocketpp::lib::placeholders::_1;
using websocketpp::lib::placeholders::_2;

// global variable
server *ws_server = new server();
int connection_number = 0;
connection_hdl ws_conn;

void on_open(server*, connection_hdl);
void on_close(server*, connection_hdl);
void on_message(server*, connection_hdl, server::message_ptr);

int main(int argc, char** argv) {
  GlobalData *global = new GlobalData("../../../");
  Controller *controller = new Controller(global);

  // generate websocket
  ws_server->set_open_handler(bind(on_open, ws_server, ::_1));
  ws_server->set_close_handler(bind(on_close, ws_server, ::_1));
  ws_server->init_asio();
  ws_server->set_error_channels(websocketpp::log::elevel::none);
  ws_server->set_access_channels(websocketpp::log::alevel::none);
  ws_server->set_message_handler(bind(on_message, ws_server, ::_1, ::_2));

  thread server_thread([&]() {
    while (true) {
      try {
        ws_server->listen(9000);
        ws_server->start_accept();
        cout << controller->getName() << " server running" << endl;
        ws_server->run();
        break;
      } catch (websocketpp::exception const &e) {
        cout << "failed init websocket and try again" << endl;
        cout << "websocket error: " << e.what() << endl;
        this_thread::sleep_for(chrono::seconds(30));
      }
    }
  });
  thread main_thread([&]() {
    controller->setManual(true);
    controller->setTarget(controller->getPosition() + Vec{0, 300});
    while (true) {
      if (connection_number == 1) {
        try {
          json data;
          data["id"] = 0;
          data["type"] = "position";
          data["value"]["x"] = controller->getPosition().x;
          data["value"]["y"] = controller->getPosition().y;
          ws_server->send(ws_conn, to_string(data), websocketpp::frame::opcode::text);
        } catch(...) {
          cout << "failed send data" << endl;
        }
      }
      controller->process();
    }

  });
  server_thread.join();
  main_thread.join();

  return 0;
}

void on_open(server* ws_server, connection_hdl hdl) {
  cout << "connection open" << endl;
  ws_conn = hdl;
  connection_number = 1;
}

void on_close(server* ws_server, connection_hdl hdl) {
  cout << "connection close" << endl;
  connection_number = 0;
}

void on_message(server* ws_server, connection_hdl hdl, server::message_ptr msg) {
  json data = json::parse(msg->get_payload());
}