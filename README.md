# HP Primax MacOS Keyboard Driver

HP Pavilion 600 series keyboards and some other keyboards that have been manufactured by Primax are not supported in MacOS. The function keys are working, but others are not. This issue has been asked on the internet a few times ([1](https://discussions.apple.com/thread/8613419), [2](https://discussions.apple.com/thread/250836764), [3](https://www.reddit.com/r/mac/comments/b9c35y/hp_pavilion_600_wireless_keyboard_on_macbook_pro/)).

According to a customer review on November 22, 2019 from [Amazon](https://www.amazon.com/HP-Pavilion-Wireless-Keyboard-Combo/dp/B07FJHZDPK), with a name Lirio, this issue only happens in the HP Pavilion 600 keyboards that have a usb dongle with version 1.05. I think Primax fixed the issue later and updated the USB dongle to version 1.07.

## What is the issue?

These keyboards manufactured by Primax send one extra byte with every keystroke. After my research, I realized that the implementation is not wrong according to HID (Human Interface Device) specification, but not a common one. Windows supports parsing the HID data with the external byte; however MacOS does not. Further information can be found [here](https://www.linux.org.ru/forum/linux-hardware/14777529)

Support for the extra byte has been implemented to Linux in February and published with the Linux Kernel 5.2 in April. Further information can be found [here](https://lkml.org/lkml/2019/2/28/510) and [here](https://unix.stackexchange.com/questions/377830/linux-hid-driver-for-primax-wireless-keyboards).

HP Pavilion 600 keyboard is not the only keyboard with this issue; some from Lenovo and Asus have that issue as well.

Before the Linux patch, a Linux driver has been written for [Lenovo Ultraslim keyboard](https://github.com/y-trudeau/linux_lenovo_ultraslim_plus), and a fork exists for [HP Pavilion 600](https://github.com/JohnJohanssonChalmers/hp_wireless_keyboard). However these drivers are Linux only.

## The Project

This is an experimental HP Pavilion 600 driver for MacOS. It basically reads the input using [hidapi](https://github.com/libusb/hidapi), and interprets the input. It uses CGEvent API from Apple to simulate key inputs.

In addition to basic keystrokes, it supports key combinations (at some level), and basic press & hold key repeat. However, this driver is still an experiment.

## How to run?

First, install [hidapi](https://github.com/libusb/hidapi) using brew.

```
brew install hidapi
```

Some build tools and libraries are also required. It is advised to install Xcode. Edit the `Makefile` using your [hidapi](https://github.com/libusb/hidapi) library path (brew prints it after the installation).

Then run `make`. This will create a executable called `driver`. Run `driver` with `sudo`.

For an unknown reason, sometimes the library cannot open the keyboard device. Try rerunning the command a few times.

## Contribution

Please feel free to contribute.

## Authors

* [Ozan Akın](https://github.com/oznakn)

## License

See `LICENSE.md`.
