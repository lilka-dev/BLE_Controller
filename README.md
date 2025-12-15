# Lilka BLE Controller

![Made in Ukraine](https://img.shields.io/badge/Made%20in-Ukraine-blue?labelColor=yellow)
[![firmware](https://github.com/lilka-dev/BLE_Controller/actions/workflows/firmware.yml/badge.svg)](https://github.com/lilka-dev/BLE_Controller/actions/workflows/firmware.yml)

Multi-mode Bluetooth HID Controller for Lilka v2.

**Modes:** Gamepad • Mouse • Keyboard

---

## 🇬🇧 English

### Features
- **Gamepad Mode**: D-pad + 6 buttons, works with games and emulators
- **Mouse Mode**: D-pad moves cursor, A/B for clicks, C/D for scroll, Start for middle click
- **Keyboard Mode**: On-screen keyboard with 5 rows including special keys
- **Language Switch**: Press SELECT in keyboard mode to switch language (WIN+SPACE)
- **Battery Level**: Displayed on screen and reported via BLE

### Controls

| Button | Gamepad | Mouse | Keyboard |
|--------|---------|-------|----------|
| D-pad | Axes | Move cursor | Navigate keys |
| A | Button 1 | Left click | Type character |
| B | Button 2 | Right click | Backspace |
| C | Button 3 | Scroll up | Toggle layer (abc/ABC/!@#) |
| D | Button 4 | Scroll down | Space |
| Start | Button 5 | Middle click | Enter |
| Select | Button 6 | - | Switch language (WIN+Space) |

**Hold START + SELECT for 3 seconds to switch mode**

### How to Flash

#### Option 1: Build from source
```sh
# Clone repository
git clone https://github.com/lilka-dev/BLE_Controller.git
cd BLE_Controller

# Build and upload (English)
pio run -e en -t upload

# Or Ukrainian version
pio run -e uk -t upload
```

#### Option 2: Flash from release (no build required)

1. **Download firmware**
   - Go to [Releases](https://github.com/lilka-dev/BLE_Controller/releases)
   - Download `BLE_Controller_merged_en.bin` (or `_uk.bin` for Ukrainian)

2. **Open ESPTool**
   - Go to [ESPTool Web Flasher](https://espressif.github.io/esptool-js/)
   - ⚠️ Works only in **Google Chrome** or Chromium browsers

3. **Connect Lilka**
   - Use a USB cable that supports **data transfer** (not just charging)
   - ✅ Recommended: USB-A to USB-C cable
   - ❌ USB-C to USB-C may not work
   
   **Enter Boot Mode if needed:**
   1. Disconnect USB
   2. Hold **BOOT (SELECT)** button
   3. Connect USB
   4. Release button

4. **Connect in ESPTool**
   - Click **"Connect"**
   - Select COM port or `/dev/ttyUSB*` or `/dev/ttyACM*`
   - Allow browser access

5. **Erase Flash** (recommended)
   - Click **"Erase Flash"**

6. **Flash firmware**
   - Click **"Program"**
   - Select `BLE_Controller_merged_en.bin` (or `_uk.bin` for Ukrainian)
   - Set address: `0x0`
   - Click **"Program"**
   - Wait for **"Success"**

7. **Done!**
   - Press **RESET** button on Lilka
   - The controller will start and show "Disconnected"
   - Connect via Bluetooth from your device

---

## 🇺🇦 Українська

### Можливості
- **Режим геймпаду**: D-pad + 6 кнопок, працює з іграми та емуляторами
- **Режим миші**: D-pad рухає курсор, A/B для кліків, C/D для прокрутки, Start для середнього кліку
- **Режим клавіатури**: Екранна клавіатура з 5 рядками та спеціальними клавішами
- **Перемикання мови**: Натисніть SELECT в режимі клавіатури (WIN+SPACE)
- **Рівень батареї**: Відображається на екрані та передається через BLE

### Керування

| Кнопка | Геймпад | Миша | Клавіатура |
|--------|---------|------|------------|
| D-pad | Осі | Рух курсора | Навігація |
| A | Кнопка 1 | Лівий клік | Ввести символ |
| B | Кнопка 2 | Правий клік | Backspace |
| C | Кнопка 3 | Прокрутка вгору | Змінити шар (abc/ABC/!@#) |
| D | Кнопка 4 | Прокрутка вниз | Пробіл |
| Start | Кнопка 5 | Середній клік | Enter |
| Select | Кнопка 6 | - | Змінити мову (WIN+Space) |

**Утримуйте START + SELECT 3 секунди для зміни режиму**

### Як прошити

#### Варіант 1: Зібрати з вихідного коду
```sh
# Клонувати репозиторій
git clone https://github.com/lilka-dev/BLE_Controller.git
cd BLE_Controller

# Зібрати та завантажити (Англійська)
pio run -e en -t upload

# Або українську версію
pio run -e uk -t upload
```

#### Варіант 2: Прошити з релізу (без збірки)

1. **Завантажити прошивку**
   - Перейдіть до [Releases](https://github.com/lilka-dev/BLE_Controller/releases)
   - Завантажте `BLE_Controller_merged_uk.bin` (або `_en.bin` для англійської)

2. **Відкрити ESPTool**
   - Перейдіть на [ESPTool Web Flasher](https://espressif.github.io/esptool-js/)
   - ⚠️ Працює лише в **Google Chrome** або Chromium браузерах

3. **Підключити Лілку**
   - Використовуйте USB кабель з **передачею даних** (не лише зарядка)
   - ✅ Рекомендовано: USB-A до USB-C кабель
   - ❌ USB-C до USB-C може не працювати
   
   **Вхід у Boot Mode (якщо потрібно):**
   1. Від'єднайте USB
   2. Утримуйте кнопку **BOOT (SELECT)**
   3. Під'єднайте USB
   4. Відпустіть кнопку

4. **Підключення в ESPTool**
   - Натисніть **"Connect"**
   - Виберіть COM порт або `/dev/ttyUSB*` або `/dev/ttyACM*`
   - Дозвольте браузеру доступ

5. **Очистити Flash** (рекомендовано)
   - Натисніть **"Erase Flash"**

6. **Прошити**
   - Натисніть **"Program"**
   - Виберіть `BLE_Controller_merged_uk.bin` (або `_en.bin` для англійської)
   - Введіть адресу: `0x0`
   - Натисніть **"Program"**
   - Дочекайтесь **"Success"**

7. **Готово!**
   - Натисніть кнопку **RESET** на Лілці
   - Контролер запуститься і покаже "Disconnected"
   - Підключіться через Bluetooth з вашого пристрою

---

## 💡 Tips / Поради

- If flashing fails, try entering Boot Mode again
- Use "Erase Flash" before flashing a new version
- You can flash as many times as you want - it's safe

---

- Якщо прошивка не вдається, спробуйте знову увійти в Boot Mode
- Використовуйте "Erase Flash" перед прошивкою нової версії
- Можна прошивати скільки завгодно разів - це безпечно

---

## License

MIT License

