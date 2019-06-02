# Projekt: SmartBramex
# Funkcja: Scenariusze testowe rozwiazania
# Ostatnia zmiana: 02/06/2019
# Autorzy: Mateusz Dudek */

red=`tput setaf 1`
green=`tput setaf 2`
yellow=`tput setaf 3`
reset=`tput sgr0`

# Build solution
echo "${green}Build solution...${reset}"
sh build.sh || { 
    echo "${red}Build failed${reset}"
    exit 1
}
echo "${green}Solution built properly...${reset}"

# Run unit tets
# echo "${yellow}Run Unit tests...${reset}"
# sh build.sh
# echo "${green}Unit tests passed...${reset}"

# Run test scenarios
echo "${green}Run test scenarios...${reset}"
{
    sh tests/start_server.sh
    sh tests/send_message_to_server.sh
    sh tests/start_client_without_server.sh
} || {
    echo "${red}Test scenarios failed...${reset}"
    exit 1;   
}

echo "${green}Test scenarios passed...${reset}"
