red=`tput setaf 1`
green=`tput setaf 2`
yellow=`tput setaf 3`
cyan=`tput setaf 6`
reset=`tput sgr0`

port_number=6969
server_ip="127.0.0.1"
device_number=12345
timeout_time=5s

client_temp_file=$(mktemp)

echo "${yellow}Execute starting client which connects to not available server...${reset}"

echo "${cyan}Use ${client_temp_file} for temporary client output${reset}"

echo "${cyan}Start client with device number ${device_number} and connect to server ${server_ip}:${port_number}${reset}"
bin/client_udp $server_ip $port_number $device_number > $client_temp_file &
client_pid=$(ps -C "client_udp" -o pid=)
echo "${cyan}Client started with PID ${client_pid}${reset}"

sleep $timeout_time

echo "${cyan}Close client[${client_pid}]"
kill $client_pid

grep -q "Heartbeat sent" $client_temp_file \
&& grep -q "Message sent" $client_temp_file \
&& ! grep -q "Heartbeat response received" $client_temp_file \
&& ! grep -q "Message from server" $client_temp_file \
&& {
    echo "${cyan}Expected lines detected in client logs. The server is not alive${reset}"
} || {
    echo "${red}Expected lines not detected in client logs or server is alive${reset}"
    rm ${client_temp_file}
    exit 1
}

rm ${client_temp_file}

echo "${yellow}Starting client which connects to not available server passed...${reset}"