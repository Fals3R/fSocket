# Socket-Connection
Multiclient socket library written on c++

*Created in VisualStudio 2019*

# Usage

## • Creating server
```cpp
bool packet_handler(SOCKET connection, fsocket::Packet packet) {
  if(packet == fsocket::Packet::ExamplePacket) {
    // do smth
  }
  return true;
}

void main() {
  fsocket::Server* server = new fsocket::Server("127.0.0.1", 1337, packet_handler);
  bool status = server->start();
}
```
## • Creating client
```cpp
bool packet_handler(SOCKET connection, fsocket::Packet packet) {
  if(packet == fsocket::Packet::ExamplePacket) {
    // do smth
  }
  return true;
}

void main() {
  fsocket::Client* client = new fsocket::Client("127.0.0.1", 1337, packet_handler);
  bool status = client->start();
}
```
## • Creating new packet
Editing Packet enum
```cpp
enum Packet {
  TestString,
  TestInt,
  ExamplePacket
}
```
## • Sending information
```cpp
fsocket::Net::sendPacket(connection, fsocket::Packet::ExamplePacket); // sending packet
fsocket::Net::sendInt(connection, 1337); // sending int
fsocket::Net::sendString(connection, "example text"); // sending string
fsocket::Net::sendBytes(connection, (char*)0x90, 1); // sending bytes
```
## • Receiving information
```cpp
fsocket::Packet data;
fsocket::Net::recvPacket(connection, data); // receiving packet

int data;
fsocket::Net::recvInt(connection, data); // receiving int

std::string data;
fsocket::Net::recvString(connection, data); // receiving string

char* data = new char[7 + 1];
data[7] = '\0';
fsocket::Net::recvBytes(connection, data, 7); // receiving bytes
...
delete[] data;
```
## • Catching error
All functions in Net returns boolean where **true** is good and **false** is bad
```cpp
bool status = fsocket::Net::sendString(connection, "example text");
if(!status)
  return false; // error
return true; // successfully
```
