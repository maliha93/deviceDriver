# deviceDriver  

##### sudo insmod chardrivertest1.ko  

// if devs are not created yet  
sudo mknod /dev/chardev c $MAJOR 0  
sudo chmod 666 chardev  
sudo mknod /dev/chardev1 c $MAJOR 1  
sudo chmod 666 chardev
