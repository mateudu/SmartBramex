# Projekt: SmartBramex
# Funkcja: Scenariusz testowy uruchomienia serwera
# Ostatnia zmiana: 02/06/2019
# Autorzy: Mateusz Dudek

red=`tput setaf 1`
green=`tput setaf 2`
yellow=`tput setaf 3`
cyan=`tput setaf 6`
reset=`tput sgr0`

PORT_NUMBER=6969
server_temp_file=$(mktemp)
timeout_time=5s

echo "${yellow}Execute server start...${reset}"

echo "${cyan}Use ${server_temp_file} for temporary server output${reset}"

echo "${cyan}Start server at ${PORT_NUMBER} port${reset}"
timeout -k $timeout_time $timeout_time bin/server $PORT_NUMBER > $server_temp_file
echo "${cyan}Server started and closed${reset}"

grep -q "IPROTO_UDPLITE" $server_temp_file && {
    echo "${cyan}Expected log detected in server logs${reset}"
} || {
    echo "${red}Expected log not detected in server logs${reset}"
    rm ${server_temp_file}
    exit 1
}

rm ${server_temp_file}
echo "${yellow}Server start passed...${reset}"