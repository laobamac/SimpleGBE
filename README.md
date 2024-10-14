# SimpleGBE
A macOS kext for Intel Gbit Ethernet.<br/>
macOS下英特尔千兆网卡驱动
移植自Ubuntu下的网卡驱动：EthernetIgb-5.17.4

## 支持列表
* I210 千兆卡
* I211 千兆卡
* 82575/82576/82580 千兆卡
* I350/I350T4 千兆卡

## 支持情况
* IPV4、IPV6自动获取（DHCP）
* TCP校验和
* TSO和TSO6
* 网线热插拔&插拔后重分配
* 支持Sequoia
* 强制速率受`igb_ethtool_set_link_ksettings()`影响无法生效

## 鸣谢
* Intel为Linux开发的[IGB驱动](https://www.intel.com/content/www/us/en/download/14098/intel-network-adapter-driver-for-82575-6-82580-i350-and-i210-211-based-gigabit-network-connections-for-linux.html)
