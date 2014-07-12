local socket=require "xsocket"
local serverfd=socket.new()
local ret=socket.bind(serverfd,"127.0.0.1",8080)
if ret<0 then
	print("listen error")
	return
end
ret=socket.listen(serverfd)

if ret<0 then
	print("listen error")
	return
end

function handleClient(client)
 while true do
 	local len,buff=socket.read(client,1024)
 	socket.write(client,buff)
 end
end

while true do
	client=socket.accept(serverfd)
	handleClient(client)
end



