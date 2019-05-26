red=`tput setaf 1`
green=`tput setaf 2`
yellow=`tput setaf 3`
cyan=`tput setaf 6`
reset=`tput sgr0`

port_number=6969
server_ip="127.0.0.1"
device_number=19194
timeout_time=5s

server_temp_file=$(mktemp)
client_temp_file=$(mktemp)

echo "${yellow}Execute sending message from client to server...${reset}"

echo "${cyan}Use ${server_temp_file} for temporary server output${reset}"
echo "${cyan}Use ${client_temp_file} for temporary client output${reset}"

echo "${cyan}Start server at ${port_number} port${reset}"
bin/server $port_number > $server_temp_file &
server_pid=$(ps -C "server" -o pid=)
echo "${cyan}Server started with PID ${server_pid}${reset}"

echo "${cyan}Start client with device number ${device_number} and connect to server ${server_ip}:${port_number}${reset}"
bin/client_udp $server_ip $port_number $device_number > $client_temp_file &
client_pid=$(ps -C "client_udp" -o pid=)
echo "${cyan}Client started with PID ${client_pid}${reset}"

echo "${cyan}Wait ${timeout_time}"
sleep $timeout_time

echo "${cyan}Close client[${client_pid}] and server[${server_pid}]"
kill $client_pid
kill $server_pid

grep -q "Content: abcdef" $server_temp_file && grep -q "Response sent" $server_temp_file && {
    echo "${cyan}Expected message content and response detected in server logs${reset}"
} || {
    echo "${red}Expected message content or response not detected in server logs${reset}"
    rm ${server_temp_file}
    rm ${client_temp_file}
    exit 1
}

grep -q "Message from server: abcdef" $client_temp_file && grep -q "Message sent" $client_temp_file && {
    echo "${cyan}Expected message receive confirmation detected in client logs${reset}"
} || {
    echo "${red}Expected message receive confirmation not detected in client logs${reset}"
    rm ${server_temp_file}
    rm ${client_temp_file}
    exit 1
}

rm ${server_temp_file}
rm ${client_temp_file}

echo "${yellow}Sending message from client to server passed...${reset}"