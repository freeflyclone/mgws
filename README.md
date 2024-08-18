## mgws - Mongoose Web Server & SQLite C++ Wrapper
### A lightweight C++ wrapper around the Mongoose Web Server and SQLite database library

IoT applications often benefit from a Web-based UI for remote access, and [Mongoose](https://mongoose.ws) is an excellent choice for many use cases.

Mongoose supports:
  * [HTTP/HTTPS](https://mongoose.ws/documentation/#http-1) - for Web UI
  * [Websocket](https://mongoose.ws/documentation/#websocket-1) - for real-time updates
  * [MQTT](https://mongoose.ws/documentation/#mqtt-1) - for lightweight publish/subscribe messaging
  * [SNTP](https://mongoose.ws/documentation/#sntp) - for synchronizing time
  * [SMTP](https://mongoose.ws/documentation/tutorials/smtp/smtp-client/) - for sending emails

Mongoose is written in C, and can run on [bare-metal microcontrollers](https://mongoose.ws/documentation/#stm32) when properly configured.

Additionally, many IoT applications require some form of database, and [SQLite](https://www.sqlite.org/) offers a convenient lightweight solution.
SQLite features:
  * [Public domain](https://www.sqlite.org/copyright.html)
  * [Small](https://www.sqlite.org/footprint.html)
  * [Fast](https://www.sqlite.org/fasterthanfs.html)
  * [Self-contained](https://www.sqlite.org/selfcontained.html)
  * [Reliable](https://www.sqlite.org/hirely.html)
  * [Full-featured SQL](https://www.sqlite.org/fullsql.html)

## Notes:
  * [TLS/SSL instructions](http://www.steves-internet-guide.com/using-lets-encrypt-certificate-mosquitto/) for mosquitto
