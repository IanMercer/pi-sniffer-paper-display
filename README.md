# pi-sniffer-paper-display
Sample code for a 2.13" Waveshare paper-display on a Raspberry Pi displaying the data from [pi-sniffer](https://github.com/IanMercer/pi-sniffer). 

````
sudo apt-get install wiringpi
mkdir bin
make
cp paper.service /etc/systemd/system
sudo systemctl enable paper.service
sudo systemctl start paper.service
````
