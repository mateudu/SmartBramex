-- Projekt: SmartBramex
-- Funkcja: Modul Wireshark
-- Ostatnia zmiana: 29/05/2019
-- Autor: Sebastian Kamoda

bramex_proto = Proto("bramex", "Bramex Protocol dissector")

local client_id = ProtoField.new("Client Id", "bramex.client_id", ftypes.UINT64)
local message_id = ProtoField.new("Message Id", "bramex.message_id", ftypes.UINT64)
local status = ProtoField.new("Status", "bramex.status", ftypes.INT32)
local message_type = ProtoField.new("Message Type", "bramex.message_type", ftypes.INT32)
local content = ProtoField.new("Content", "bramex.content", ftypes.STRING)

bramex_proto.fields = { client_id, message_id, status, message_type, content }

function bramex_proto.dissector(buffer, pinfo, tree)
	pinfo.cols.protocol = "BRAMEX"
		length = buffer:len()
			if length == 0 then return end

	local pktlen = buffer:reported_length_remaining()

		local subtree = tree:add(bramex_proto, buffer:range(0, pktlen), "Bramex protocol data")
	subtree:append_text(", packet details below")

	-- approximate value
	if length > 100
	then
		-- add_le for little endian for proper integer represantation
		subtree:add_le(client_id, buffer:range(0, 8))
		subtree:add_le(message_id, buffer:range(8, 8))
		subtree:add_le(status, buffer:range(16, 4))
		subtree:add_le(message_type, buffer:range(20, 4))
		local data = buffer(24):tvb()
		subtree:add(content, data())
	end

	if length < 100
	then
		-- tvb ("Testy Virtual Buffer") represents the packet's buffer
		local data = buffer(0):tvb()
		subtree:add(content, data())
	end
	--add other payloads details
end

function bramex_proto.init()
end

local udpLite_dissector_table = DissectorTable.get("udp.port")
udpLite_dissector_table:add(6969, bramex_proto)
udpLite_dissector_table:add(6970, bramex_proto)