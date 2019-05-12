bramex_proto = Proto("bramex", "Bramex Protocol dissector")

--source_port = ProtoField.uint16("bramex.source", "udp.srcport", base.DEC)
local data = ProtoField.new("Payload", "bramex.data", ftypes.STRING)

bramex_proto.fields = { data }

function bramex_proto.dissector(buffer, pinfo, tree)
	pinfo.cols.protocol = "BRAMEX"
    	length = buffer:len()
    		if length == 0 then return end

	local pktlen = buffer:reported_length_remaining()

        local subtree = tree:add(bramex_proto, buffer:range(0, pktlen), "Bramex protocol data")
	subtree:add(data, buffer:range(0, pktlen))
--add other payloads details
--subtree:append_text(", Packet details below")
end

function bramex_proto.init()
end

local udpLite_dissector_table = DissectorTable.get("udp.port")
udpLite_dissector_table:add(6969, bramex_proto)
