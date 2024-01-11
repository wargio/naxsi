# REMOVE ME
sed -i 's/dl-cdn.alpinelinux.org/mirrors.ustc.edu.cn/g' /etc/apk/repositories

cd /sources/naxsi
sh ./distros/alpine/build-ci.sh
