local socket=require "xsocket"
local s=socket.connect("www.baidu.com",80)
socket.write(s,"GET / HTTP/1.0\n\n")
local len,buff=socket.read(s,1024)  --len
print(buff)