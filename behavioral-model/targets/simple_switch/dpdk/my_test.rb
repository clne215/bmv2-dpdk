require 'socket'

interface = 'enp0s8'         # interface name
interface_index = 0x8933     # SIOCGIFINDEX

frame = "\x08\x00\x27\x01\x2d\x63\x08\x00\x27\x9c\xcc\xde"

(12...100).each do |i|
  frame += i.chr
end

socket = Socket.new(Socket::AF_PACKET, Socket::SOCK_RAW, Socket::IPPROTO_RAW)
ifreq = [interface.dup].pack('a32')
socket.ioctl(interface_index, ifreq)
socket.bind([Socket::AF_PACKET].pack('s') + [Socket::IPPROTO_RAW].pack('n') + ifreq[16..20]+ ("\x00" * 12))

socket.send(frame, 0)
