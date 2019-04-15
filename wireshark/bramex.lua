-- we create Proto object and call it bramex proto
-- object takes two arguments -  name and description
bramex_proto = Proto("bramex", "Bramex Protocol (lua)")

source_port         = ProtoField.int32("bramex.source_port", "sourcePort", base.DEC)
destination_port    = ProtoField.int32("bramex.destination_port", "destinationPort", base.DEC)
checksum_coverage   = ProtoField.int32("bramex.checksum_coverage", "checksumCoverage", base.DEC)
checksum            = ProtoField.int32("bramex.checksum", "checksum", base.DEC)
payload             = ProtoField.int32("bramex.payload ", "payload", base.DEC)

-- protocol requires fields table and dissector fucntion
-- we havent added any fields so this table is empty
bramex_proto.fields = {source_port, destination_port, checksum_coverage, checksum, payload}

-- buffer - actual data to dissect
-- pinfo - packet info structure has whole collection of information about previous dissection
--         pinfo also contains the columns of the packet list
-- tree - tree structure to append items for own protocol
function bramex_proto.dissector(buffer, pinfo, tree)
    length = buffer:len()
    if length == 0 then return end

    -- for changing name of column to our protocol name
    --pinfo.cols.protocol = bramex_proto.name
    -- array of columns that are being displayed
    --pinfo.cols

    -- create a subtree in the tree structure found in the Packet Details
    -- it done by adding an additional tree item to the tree object that was passed
    -- as an argument to the dissector function
    local subtree = tree:add(bramex_proto, buffer(), "Bramex Protocol Data")

    subtree:add(source_port, buffer(0,2))
    subtree:add(destination_port, buffer(2,2))
    subtree:add(checksum_coverage, buffer(4,2))
    subtree:add(checksum, buffer(6,2))
    subtree:add(payload, buffer(8,4))


    local checksum = buffer(0, 4):uint()
    local command = buffer(4, 1)
    local data = buffer(5):tvb()
    
end

  

-- TODO: check if udp-lite port is really udp.port
local udpLite_dissector_table = DissectorTable.get("udp.port")
udpLite_dissector_table:add(59274, bramex_proto)
-- number is number of port that we use to connect to server

-- name Dudek_Udp_Lite_dokumetacja.docx