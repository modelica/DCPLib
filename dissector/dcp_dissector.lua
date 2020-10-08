-- DCP Dissector for Wireshark
-- declare protocol
dcp_proto = Proto("dcp","Distributed Co-Simulation Protocol")
-- create a function to dissect it
function dcp_proto.dissector(buffer,pinfo,tree)
    pinfo.cols.protocol = "DCP"

    -- add msg subtree to wireshark display
    local subtree = tree:add(dcp_proto,buffer(),"DCP PDUs")
    local type_id = buffer(0,1):uint()

    if type_id == 1 then
	    subtree = subtree:add("STC_register")
   	    subtree:add(buffer(0,1),"type_id: " .. buffer(0,1):uint())
	    subtree:add(buffer(1,2),"pdu_seq_id: " .. buffer(1,2):le_uint())
    	    subtree:add(buffer(3,1),"receiver: " .. buffer(3,1):uint())
	    subtree:add(buffer(4,1),"state_id: " .. buffer(4,1):uint())
	    subtree:add(buffer(5,16),"slave_uuid: " .. buffer(5,1) .. buffer(6,1) .. buffer(7,1) .. buffer(8,1) .. "-" .. buffer(9,1) .. buffer(10,1) .. "-" .. buffer(11,1) .. buffer(12,1) .. "-" .. buffer(13,1) .. buffer(14,1) .. "-" .. buffer(15,1) .. buffer(16,1) .. buffer(17,1) .. buffer(18,1) .. buffer(19,1) .. buffer(20,1))
	    subtree:add(buffer(21,1),"op_mode: " .. buffer(21,1):uint())
	    subtree:add(buffer(22,1),"major_version: " .. buffer(22,1):uint())
	    subtree:add(buffer(23,1),"minor_version: " .. buffer(23,1):uint())
    end

    if type_id == 2 then
	    subtree = subtree:add("STC_deregister")
   	    subtree:add(buffer(0,1),"type_id: " .. buffer(0,1):uint())
	    subtree:add(buffer(1,2),"pdu_seq_id: " .. buffer(1,2):le_uint())
    	    subtree:add(buffer(3,1),"receiver: " .. buffer(3,1):uint())
	    subtree:add(buffer(4,1),"state_id: " .. buffer(4,1):uint())
    end

    if type_id == 3 then
	    subtree = subtree:add("STC_prepare")
   	    subtree:add(buffer(0,1),"type_id: " .. buffer(0,1):uint())
	    subtree:add(buffer(1,2),"pdu_seq_id: " .. buffer(1,2):le_uint())
    	    subtree:add(buffer(3,1),"receiver: " .. buffer(3,1):uint())
	    subtree:add(buffer(4,1),"state_id: " .. buffer(4,1):uint())
    end

    if type_id == 4 then
	    subtree = subtree:add("STC_configure")
   	    subtree:add(buffer(0,1),"type_id: " .. buffer(0,1):uint())
	    subtree:add(buffer(1,2),"pdu_seq_id: " .. buffer(1,2):le_uint())
    	    subtree:add(buffer(3,1),"receiver: " .. buffer(3,1):uint())
	    subtree:add(buffer(4,1),"state_id: " .. buffer(4,1):uint())
    end

    if type_id == 5 then
	    subtree = subtree:add("STC_initialize")
   	    subtree:add(buffer(0,1),"type_id: " .. buffer(0,1):uint())
	    subtree:add(buffer(1,2),"pdu_seq_id: " .. buffer(1,2):le_uint())
    	    subtree:add(buffer(3,1),"receiver: " .. buffer(3,1):uint())
	    subtree:add(buffer(4,1),"state_id: " .. buffer(4,1):uint())
    end

    if type_id == 6 then
	    subtree = subtree:add("STC_run")
   	    subtree:add(buffer(0,1),"type_id: " .. buffer(0,1):uint())
	    subtree:add(buffer(1,2),"pdu_seq_id: " .. buffer(1,2):le_uint())
    	    subtree:add(buffer(3,1),"receiver: " .. buffer(3,1):uint())
	    subtree:add(buffer(4,1),"state_id: " .. buffer(4,1):uint())
	    subtree:add(buffer(5,8),"time" .. buffer(5,8):le_uint64())
    end

    if type_id == 7 then
	    subtree = subtree:add("STC_do_step")
   	    subtree:add(buffer(0,1),"type_id: " .. buffer(0,1):uint())
	    subtree:add(buffer(1,2),"pdu_seq_id: " .. buffer(1,2):le_uint())
    	    subtree:add(buffer(3,1),"receiver: " .. buffer(3,1):uint())
	    subtree:add(buffer(4,1),"state_id: " .. buffer(4,1):uint())
	    subtree:add(buffer(5,4),"steps: " .. buffer(5,4):le_uint())
    end

    if type_id == 8 then
	    subtree = subtree:add("STC_send_outputs")
   	    subtree:add(buffer(0,1),"type_id: " .. buffer(0,1):uint())
	    subtree:add(buffer(1,2),"pdu_seq_id: " .. buffer(1,2):le_uint())
    	    subtree:add(buffer(3,1),"receiver: " .. buffer(3,1):uint())
	    subtree:add(buffer(4,1),"state_id: " .. buffer(4,1):uint())
    end

    if type_id == 9 then
	    subtree = subtree:add("STC_stop")
   	    subtree:add(buffer(0,1),"type_id: " .. buffer(0,1):uint())
	    subtree:add(buffer(1,2),"pdu_seq_id: " .. buffer(1,2):le_uint())
    	    subtree:add(buffer(3,1),"receiver: " .. buffer(3,1):uint())
	    subtree:add(buffer(4,1),"state_id: " .. buffer(4,1):uint())
    end

    if type_id == 10 then
	    subtree = subtree:add("STC_reset")
   	    subtree:add(buffer(0,1),"type_id: " .. buffer(0,1):uint())
	    subtree:add(buffer(1,2),"pdu_seq_id: " .. buffer(1,2):le_uint())
    	    subtree:add(buffer(3,1),"receiver: " .. buffer(3,1):uint())
	    subtree:add(buffer(4,1),"state_id: " .. buffer(4,1):uint())
    end

    if type_id == 32 then
	    subtree = subtree:add("CFG_time_res")
   	    subtree:add(buffer(0,1),"type_id: " .. buffer(0,1):uint())
	    subtree:add(buffer(1,2),"pdu_seq_id: " .. buffer(1,2):le_uint())
    	    subtree:add(buffer(3,1),"receiver: " .. buffer(3,1):uint())
	    subtree:add(buffer(4,4),"numerator: " .. buffer(4,4):le_uint())
   	    subtree:add(buffer(8,4),"denominator: " .. buffer(8,4):le_uint())
    end

    if type_id == 33 then
	    subtree = subtree:add("CFG_steps")
   	    subtree:add(buffer(0,1),"type_id: " .. buffer(0,1):uint())
	    subtree:add(buffer(1,2),"pdu_seq_id: " .. buffer(1,2):le_uint())
    	    subtree:add(buffer(3,1),"receiver: " .. buffer(3,1):uint())
   	    subtree:add(buffer(4,4),"steps: " .. buffer(4,4):le_uint())
	    subtree:add(buffer(8,2),"data_id: " .. buffer(8,2):le_uint())
    end

    if type_id == 34 then
	    subtree = subtree:add("CFG_input")
   	    subtree:add(buffer(0,1),"type_id: " .. buffer(0,1):uint())
	    subtree:add(buffer(1,2),"pdu_seq_id: " .. buffer(1,2):le_uint())
    	    subtree:add(buffer(3,1),"receiver: " .. buffer(3,1):uint())
	    subtree:add(buffer(4,2),"data_id: " .. buffer(4,2):le_uint())
    	    subtree:add(buffer(6,2),"pos: " .. buffer(6,2):le_uint())
    	    subtree:add(buffer(8,8),"target_vr: " .. buffer(8,8):le_uint64())
	    subtree:add(buffer(16,1),"source_data_type: " .. buffer(16,1):uint())
    end

    if type_id == 35 then
	    subtree = subtree:add("CFG_output")
   	    subtree:add(buffer(0,1),"type_id: " .. buffer(0,1):uint())
	    subtree:add(buffer(1,2),"pdu_seq_id: " .. buffer(1,2):le_uint())
    	    subtree:add(buffer(3,1),"receiver: " .. buffer(3,1):uint())
	    subtree:add(buffer(4,2),"data_id: " .. buffer(4,2):le_uint())
    	    subtree:add(buffer(6,2),"pos: " .. buffer(6,2):le_uint())
    	    subtree:add(buffer(8,8),"source_vr: " .. buffer(8,8):le_uint64())
    end

    if type_id == 36 then
            subtree = subtree:add("CFG_clear")
   	    subtree:add(buffer(0,1),"type_id: " .. buffer(0,1):uint())
	    subtree:add(buffer(1,2),"pdu_seq_id: " .. buffer(1,2):le_uint())
    	    subtree:add(buffer(3,1),"receiver: " .. buffer(3,1):uint())
    end

    if type_id == 37 then
	    subtree = subtree:add("CFG_target_network_information")
   	    subtree:add(buffer(0,1),"type_id: " .. buffer(0,1):uint())
	    subtree:add(buffer(1,2),"pdu_seq_id: " .. buffer(1,2):le_uint())
	    subtree:add(buffer(3,1),"receiver: " .. buffer(3,1):uint())
	    subtree:add(buffer(4,2),"data_id: " .. buffer(4,2):le_uint())
	    subtree:add(buffer(6,1),"transport_protocol: " .. buffer(6,1):uint())
	    subtree:add(buffer(7,2),"target_port: " .. buffer(7,2):le_uint())
    	    subtree:add(buffer(9,4),"target_ip_address: " .. buffer(12,1):uint() .. "." .. buffer(11,1):uint() .. "." .. buffer(10,1):uint() .. "." .. buffer(9,1):uint())
    end

    if type_id == 38 then
	    subtree = subtree:add("CFG_source_network_information")
   	    subtree:add(buffer(0,1),"type_id: " .. buffer(0,1):uint())
	    subtree:add(buffer(1,2),"pdu_seq_id: " .. buffer(1,2):le_uint())
	    subtree:add(buffer(3,1),"receiver: " .. buffer(3,1):uint())
	    subtree:add(buffer(4,2),"data_id: " .. buffer(4,2):le_uint())
	    subtree:add(buffer(6,1),"transport_protocol: " .. buffer(6,1):uint())
	    subtree:add(buffer(7,2),"source_port: " .. buffer(7,2):le_uint())
    end

    if type_id == 39 then
	    subtree = subtree:add("CFG_parameter")
   	    subtree:add(buffer(0,1),"type_id: " .. buffer(0,1):uint())
	    subtree:add(buffer(1,2),"pdu_seq_id: " .. buffer(1,2):le_uint())
    	    subtree:add(buffer(3,1),"receiver: " .. buffer(3,1):uint())
    	    subtree:add(buffer(4,8),"parameter_vr: " .. buffer(5,8):le_uint64())
	    subtree:add(buffer(12,1),"source_data_type: " .. buffer(4,1):uint())
    end

    if type_id == 40 then
	    subtree = subtree:add("CFG_tunable_parameter")
   	    subtree:add(buffer(0,1),"type_id: " .. buffer(0,1):uint())
	    subtree:add(buffer(1,2),"pdu_seq_id: " .. buffer(1,2):le_uint())
    	    subtree:add(buffer(3,1),"receiver: " .. buffer(3,1):uint())
	    subtree:add(buffer(4,2),"param_id: " .. buffer(4,2):le_uint())
     	    subtree:add(buffer(6,2),"pos: " .. buffer(6,2):le_uint())
     	    subtree:add(buffer(8,8),"parameter_vr: " .. buffer(9,8):le_uint64())
 	    subtree:add(buffer(16,1),"source_data_type: " .. buffer(8,1):uint())
    end

    if type_id == 41 then
	    subtree = subtree:add("CFG_param_network_information")
   	    subtree:add(buffer(0,1),"type_id: " .. buffer(0,1):uint())
	    subtree:add(buffer(1,2),"pdu_seq_id: " .. buffer(1,2):le_uint())
	    subtree:add(buffer(3,1),"receiver: " .. buffer(3,1):uint())
	    subtree:add(buffer(4,2),"param_id: " .. buffer(4,2):le_uint())
	    subtree:add(buffer(6,1),"transport_protocol: " .. buffer(6,1):uint())
	    subtree:add(buffer(7,2),"target_port: " .. buffer(7,2):le_uint())
    	    subtree:add(buffer(9,4),"target_ip_address: " .. buffer(12,1):uint() .. "." .. buffer(11,1):uint() .. "." .. buffer(10,1):uint() .. "." .. buffer(9,1):uint())
    end

    if type_id == 42 then
	    subtree = subtree:add("CFG_logging")
   	    subtree:add(buffer(0,1),"type_id: " .. buffer(0,1):uint())
	    subtree:add(buffer(1,2),"pdu_seq_id: " .. buffer(1,2):le_uint())
	    subtree:add(buffer(3,1),"receiver: " .. buffer(3,1):uint())
	    subtree:add(buffer(4,1),"log_category: " .. buffer(4,1):uint())
	    subtree:add(buffer(5,1),"log_level: " .. buffer(5,1):uint())
	    subtree:add(buffer(6,1),"log_mode: " .. buffer(6,1):uint())
    end

    if type_id == 43 then
	    subtree = subtree:add("CFG_scope")
   	    subtree:add(buffer(0,1),"type_id: " .. buffer(0,1):uint())
	    subtree:add(buffer(1,2),"pdu_seq_id: " .. buffer(1,2):le_uint())
    	    subtree:add(buffer(3,1),"receiver: " .. buffer(3,1):uint())
	    subtree:add(buffer(4,2),"data_id: " .. buffer(4,2):le_uint())
   	    subtree:add(buffer(6,1),"scope: " .. buffer(6,1):le_uint())
    end

    if type_id == 128 then
	    subtree = subtree:add("INF_state")
   	    subtree:add(buffer(0,1),"type_id: " .. buffer(0,1):uint())
	    subtree:add(buffer(1,2),"pdu_seq_id: " .. buffer(1,2):le_uint())
    	    subtree:add(buffer(3,1),"receiver: " .. buffer(3,1):uint())
    end

    if type_id == 129 then
	    subtree = subtree:add("INF_error")
   	    subtree:add(buffer(0,1),"type_id: " .. buffer(0,1):uint())
	    subtree:add(buffer(1,2),"pdu_seq_id: " .. buffer(1,2):le_uint())
    	    subtree:add(buffer(3,1),"receiver: " .. buffer(3,1):uint())
    end

    if type_id == 130 then
	    subtree = subtree:add("INF_log")
   	    subtree:add(buffer(0,1),"type_id: " .. buffer(0,1):uint())
	    subtree:add(buffer(1,2),"pdu_seq_id: " .. buffer(1,2):le_uint())
    	    subtree:add(buffer(3,1),"receiver: " .. buffer(3,1):uint())
	    subtree:add(buffer(4,1),"log_category: " .. buffer(4,1):uint())
	    subtree:add(buffer(5,1),"log_max_num: " .. buffer(5,1):uint())
    end

    if type_id == 176 then
	    subtree = subtree:add("RSP_ack")
   	    subtree:add(buffer(0,1),"type_id: " .. buffer(0,1):uint())
	    subtree:add(buffer(1,2),"resp_seq_id: " .. buffer(1,2):le_uint())
	    subtree:add(buffer(3,1),"sender: " .. buffer(3,1):uint())
    end

    if type_id == 177 then
	    subtree = subtree:add("RSP_nack")
   	    subtree:add(buffer(0,1),"type_id: " .. buffer(0,1):uint())
	    subtree:add(buffer(1,2),"resp_seq_id: " .. buffer(1,2):le_uint())
	    subtree:add(buffer(5,1),"sender: " .. buffer(5,1):uint())
	    subtree:add(buffer(3,2),"exp_seq_id: " .. buffer(3,2):le_uint())
	    subtree:add(buffer(6,2),"error_code: " .. buffer(6,2):le_uint())
    end

    if type_id == 178 then
	    subtree = subtree:add("RSP_state_ack")
   	    subtree:add(buffer(0,1),"type_id: " .. buffer(0,1):uint())
	    subtree:add(buffer(1,2),"resp_seq_id: " .. buffer(1,2):le_uint())
	    subtree:add(buffer(3,1),"sender: " .. buffer(3,1):uint())
	    subtree:add(buffer(4,1),"state_id: " .. buffer(4,1):uint())
    end

    if type_id == 179 then
	    subtree = subtree:add("RSP_error_ack")
   	    subtree:add(buffer(0,1),"type_id: " .. buffer(0,1):uint())
	    subtree:add(buffer(1,2),"resp_seq_id: " .. buffer(1,2):le_uint())
	    subtree:add(buffer(3,1),"sender: " .. buffer(3,1):uint())
	    subtree:add(buffer(4,2),"error_code: " .. buffer(4,2):le_uint())
    end

    if type_id == 180 then
	    subtree = subtree:add("RSP_log_ack")
   	    subtree:add(buffer(0,1),"type_id: " .. buffer(0,1):uint())
	    subtree:add(buffer(1,2),"resp_seq_id: " .. buffer(1,2):le_uint())
	    subtree:add(buffer(3,1),"sender: " .. buffer(3,1):uint())
	    subtree:add(buffer(4,1),"log_entries: " .. buffer(4,1):uint())
    end

    if type_id == 224 then
	    subtree = subtree:add("NTF_state_changed")
   	    subtree:add(buffer(0,1),"type_id: " .. buffer(0,1):uint())
	    subtree:add(buffer(1,1),"sender: " .. buffer(1,1):le_uint())
    	    subtree:add(buffer(2,1),"state_id: " .. buffer(2,1):uint())
    end

    if type_id == 225 then
	    subtree = subtree:add("NTF_log")
   	    subtree:add(buffer(0,1),"type_id: " .. buffer(0,1):uint())
	    subtree:add(buffer(1,1),"sender: " .. buffer(1,1):le_uint())
    	    subtree:add(buffer(4,8),"time: " .. buffer(4,8):le_uint64())
    	    subtree:add(buffer(2,1),"log_template_id: " .. buffer(2,1):uint())
    	    subtree:add(buffer(3,1),"log_arg_val: " .. buffer(3,1):uint())
    end

    if type_id == 240 then
	    subtree = subtree:add("DAT_input_output")
   	    subtree:add(buffer(0,1),"type_id: " .. buffer(0,1):uint())
	    subtree:add(buffer(1,2),"pdu_seq_id: " .. buffer(1,2):le_uint())
	    subtree:add(buffer(3,2),"data_id: " .. buffer(3,2):uint())
    end

    if type_id == 241 then
	    subtree = subtree:add("DAT_parameter")
   	    subtree:add(buffer(0,1),"type_id: " .. buffer(0,1):uint())
	    subtree:add(buffer(1,2),"pdu_seq_id: " .. buffer(1,2):le_uint())
	    subtree:add(buffer(3,2),"param_id: " .. buffer(3,2):uint())
    end

end
-- load the udp.port table
udp_table = DissectorTable.get("udp.port")

-- register our protocol to handle udp port
udp_table:add(10000,dcp_proto)

