# Przestrzelski_Wasiluk_UMIR

Kod finalnej wersji urządzenia
ESP32-CAM-code: kod znajdujący się na mikrokontrolerze ESP32-CAM wysyłający obraz z kamery na stronę. Kod ten też zawiera odbiór sygnału sterującego i przełożenie go na obrót kamery
getPictureOur.py: kod uruchamiany na PC, dokonujący detekcji i wysyłający sygnał sterujący.

gotowy_przykład: poglądwy skrypt pythonowy do analizy obrazu wykorzystjący gotową sieć YOLO v3. Kod ten został wzięty z innego repozytorium w ramach przeglądu istniejących rozwiązań

Aby uruchomić program należy:
1. wgrać program "ESP32-CAM-code" na mikrokontroler
2. złożyć układ ESP32-CAM - servo1 - servo2 - zasilanie 5v
3. udostępnić sieć mikrokontolerowi
4. zamienić adres urządzenia w kodzie "getPictureOur"
5. uruchomić program "getPictureOur"
