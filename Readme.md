# SmartBramex
## Opis projektu
## Linki
**Dokumentacja**: [LINK](https://cloudcooking.sharepoint.com/:w:/t/Studia/TIN/Eb4ZTIl2awJAgY-VnGqGm64B1f6HMb0b6MQn_OA9PyJiaw)
## Zespół
<ul>
<li>Mateusz Dudek</li>
<li>Sebastian Kamoda</li>
<li>Krzysztof Kasprzak</li>
<li>Piotr Ogrodnik</li>
</ul>

## Uruchomienie
1. Uruchomić plik `code/build.sh`
2. Plik powinien utworzyć folder `bin` oraz uruchomić `makefile`, które kompiluje pliki `.cpp` w folderu `src` i wrzuca skompilowane pliki binarne do folderu `bin`
3. Serwer można uruchomić za pomocą komendy `server $port_number`.
4. Klienta UDP-Lite można uruchomić za pomocą komendy `client_udp $server_ip $port_number $device_number $interval $jitter $start_message_id`

## Scenariusze testowe
Scenariusze testowe można uruchomić za pomocą polecenia `sh tests.sh`.

Uruchamiane są następujące scenariusze testowe:
* Uruchomienie serwera
* Uruchomienie klienta (bez serwera)
* Uruchomienie serwera i klienta, wraz z weryfikacją wysłanych wiadomości

# Dodanie skryptu Lua do Wiresharka
Plik należy wkleić do folderu wskazanego przez Wireshark. Dla Ubuntu jest to: /home/{nazwaKonta}/.local/lib/wireshark/plugins, albo krócej $HOME/.wireshark/plugins

Żeby dowiedzieć się do jakiego folderu wkleić dany plik należy:
1. Odpalić Wireshark
2. Wybrać z ekranu głównego zakładkę Help -> About Wireshark
3. Wybrać zakładkę Folders
4. Folder w którym powinny znajdować się dodatkowe pliki znajduje się pod nazwą Personal Lua Plugins

