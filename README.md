# ft_irc - *rdalal & gmechaly*

---

## 📝 Overview
**ft_irc** is a custom Internet Relay Chat (IRC) server developed in **C++ 98**. The goal of this group project was to recreate a functional IRC server from scratch, capable of handling multiple concurrent clients, managing channels, and facilitating real-time communication without any memory leaks or crashes.

This project follows the **RFC 1459** and **RFC 2812** standards.

---

## 🚀 Features
* **Multi-client support:** Handle multiple simultaneous connections using non-blocking I/O.
* **Authentication:** Secure connection with a server password.
* **Channel Management:** Create, join, leave, and moderate channels.
* **Operator Privileges:** Special commands for channel operators (KICK, INVITE, TOPIC, MODE).
* **Private Messaging:** Send direct messages (PRIVMSG) to specific users or entire channels.

---

## 🛠️ Technical Implementation
The server is built using **I/O multiplexing** to ensure high performance and responsiveness:
* **Language:** C++ 98
* **Socket Programming:** TCP/IP sockets.
* **Event Handling:** Implementation using `poll()` for non-blocking communication.
* **Manual Parsing:** Custom command parser for IRC protocols.

---

## 💻 Usage

### Compilation
```bash
make

```

### Running the server

```bash
./ircserv <port> <password>

```

* **port:** The port number on which your server will listen (e.g., 6667).
* **password:** The password required for clients to connect.

---

### Connecting

You can connect using any IRC client (like **Irssi** or **Netcat**):

```bash
/connect 127.0.0.1 6667 password

```

---

## 🛠️ Commands Supported

| Command | Description |
| --- | --- |
| `NICK` | Change your nickname |
| `USER` | Specify username, hostname, etc. |
| `JOIN` | Join a specific channel |
| `PRIVMSG` | Send a private message to a user or channel |
| `KICK` | Eject a user from a channel (Operator only) |
| `INVITE` | Invite a user to a channel |
| `TOPIC` | Change or view the channel topic |
| `MODE` | Change channel modes (i, t, k, o, l) |

---

*Developed as part of the 42 Network curriculum.*
