# Glucose Data Collector - ESP32 Buzzer Client

*Read this in [Turkish (Türkçe)](#türkçe)*

This project is an ESP32-based hardware client for the [Glucose Data Collector](https://github.com/ermanalper/glucose-data-collector) system. 

It listens to the glucose data broadcasted by the main system and triggers a buzzer for audio notifications (alarms) at specified thresholds or states. To learn more about the data collection, processing, and the overall architecture, please visit the main repository.

## Setup & Configuration

Before compiling and flashing to your ESP32, you need to configure two things:

### 1. Configuration
Rename the `config.hpp.example` file in the project directory to `config.hpp` and fill in the required details (WiFi credentials, server IP/Port, etc.) according to your environment.

### 2. Hardware Connections
Pay attention to the buzzer pin numbers defined in the `main` file. 
* You can connect your buzzer(s) directly to these specified pins, 
* Or you can update the pin numbers in the code to match your custom physical wiring.

---

<h2 id="türkçe">Türkçe</h2>

Bu proje, [Glucose Data Collector](https://github.com/ermanalper/glucose-data-collector) sisteminin ESP32 tabanlı bir donanım istemcisidir (client). 

Ana sistemden gelen glikoz verilerini dinleyerek, belirlenen eşik değerlerinde veya durumlarda ESP32'ye bağlı bir buzzer üzerinden sesli bildirim (alarm) vermek üzere tasarlanmıştır. Sistemin veri toplama, işleme ve genel mimarisi hakkında daha fazla bilgi almak için ana repoyu inceleyebilirsiniz.

## Kurulum ve Ayarlar

Projeyi derleyip ESP32'ye yüklemeden önce yapmanız gereken iki ufak ayar bulunuyor:

### 1. Konfigürasyon
Proje dizininde bulunan `config.hpp.example` dosyasının adını `config.hpp` olarak değiştirin ve içerisindeki gerekli bilgileri (WiFi kimlik bilgileri, sunucu IP/Port detayları vb.) kendi sisteminize göre doldurun.

### 2. Donanım Bağlantıları
Buzzer donanımını ESP32'ye bağlarken pin numaralarına dikkat edin. `main` dosyasında tanımlanmış olan buzzer pin numaralarını kontrol edin. 
* Buzzer bağlantılarınızı bu pinlere yapabilir, 
* Ya da koddaki pin numaralarını kendi yaptığınız fiziksel bağlantıya göre güncelleyebilirsiniz.
