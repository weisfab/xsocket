xsocket
=======

super lightweight socket lib for lua 5.2

# Usage
## require socket library
```
local socket=require "xsocket"
```
## connect to server socket
``` lua
-- return client_fd represent the client socket fd from bsd socket.
-- parameter hostname and port
local client_fd = socket.connect("www.baidu.com",80) 
```
## create server socket
``` lua
-- return server socket fd
local server_fd = socket.new()
```
## bind server socket to addr
``` lua
local bind_ret = socket.bind(server_fd,"localhost",8080)
if bindret<0 then
	print("bind error")
end
``` 
## listen
``` lua
local listen_ret = socket.listen()
if listen_ret < 0 then
	print("listen error")
end
```
## accept
```
local clientfd=socket.accept(server_fd)

``` lua
## write socket
``` lua
socket.write(socket_fd,buff)
```
## read socket
``` lua
local readlen,buff = socket.read(socketfd,expected_len)
```

