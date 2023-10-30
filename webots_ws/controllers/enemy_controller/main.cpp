#include "controller.hpp"

#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>
#include <nlohmann/json.hpp>

#include <iostream>
#include <vector>
#include <thread>
#include <chrono>

using namespace std;

using nlohmann::json;
typedef websocketpp::server<websocketpp::config::asio> server;
using websocketpp::connection_hdl;
using websocketpp::lib::placeholders::_1;
using websocketpp::lib::placeholders::_2;

// global variable
server *ws_server = new server();
connection_hdl ws_conn;

GlobalData *global = new GlobalData("../../../");
Controller *controller = new Controller(global);

bool isRunning = false;

void on_open(server*, connection_hdl);
void on_close(server*, connection_hdl);
void on_message(server*, connection_hdl, server::message_ptr);

int main(int argc, char** argv) {
  ws_server->set_open_handler(bind(on_open, ws_server, ::_1));
  ws_server->set_close_handler(bind(on_close, ws_server, ::_1));
  ws_server->init_asio();
  ws_server->set_error_channels(websocketpp::log::elevel::none);
  ws_server->set_access_channels(websocketpp::log::alevel::none);
  ws_server->set_message_handler(bind(on_message, ws_server, ::_1, ::_2));

  thread server_thread([&]() {
    while (true) {
      try {
        ws_server->listen(9000 + controller->getName().back() - '0');
        ws_server->start_accept();
        cout << "server running: " << controller->getName() << endl;
        ws_server->run();
        break;
      } catch (websocketpp::exception const &e) {
        cout << controller->getName() << " websocket error: " << e.what() << endl;
        this_thread::sleep_for(chrono::seconds(20));
      }
    }
  });
  thread main_thread([&]() {
    while (true) {
      if (isRunning) {
        try {
          json data;
          data["type"] = "position";
          data["value"]["x"] = controller->getPosition().x;
          data["value"]["y"] = controller->getPosition().y;
          data["value"]["dir"] = controller->getDirInRadian();
          ws_server->send(ws_conn, to_string(data), websocketpp::frame::opcode::text);
        } catch(...) {
          cout << "failed send data" << endl;
        }

        if (controller->getIsFinished()) {
          json data;
          data["type"] = "finished";
          data["name"] = controller->getName();
          data["target"]["x"] = controller->getTarget().x;
          data["target"]["y"] = controller->getTarget().y;
          ws_server->send(ws_conn, to_string(data), websocketpp::frame::opcode::text);
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
  cout << "connection open: " << controller->getName() << endl;
  ws_conn = hdl;
}

void on_close(server* ws_server, connection_hdl hdl) {
  cout << "connection close: " << controller->getName() << endl;
}

void on_message(server* ws_server, connection_hdl hdl, server::message_ptr msg) {
  json data = json::parse(msg->get_payload());
  string type = data["type"].template get<string>();
  if (type == "run") {
    cout << controller->getName() << " is running" << endl;
    string value = data["value"].template get<string>();
    if (value == "start") {
      isRunning = true;
      controller->run(true);
      Vec target = Vec(
        data["target"]["x"].template get<double>(),
        data["target"]["y"].template get<double>()
      );
      controller->setTarget(target);
    } else if (value == "stop") {
      isRunning = false;
      controller->run(false);
    }
  } else if (type == "target") {
    Vec target = Vec(
      data["value"]["x"].template get<double>(),
      data["value"]["y"].template get<double>()
    );
    controller->setTarget(target);
  }
}