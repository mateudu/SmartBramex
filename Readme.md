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
