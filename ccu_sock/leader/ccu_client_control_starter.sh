sudo systemctl stop nvgetty
sudo systemctl disable nvgetty
sudo udevadm trigger
sudo chmod 777 /dev/ttyS0
./ccu_client_control 192.168.0.4 5000
