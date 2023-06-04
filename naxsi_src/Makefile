MOD_PATH 	:=	shell pwd
NGINX_TMP_DIR     :=	/tmp/nginx
NAXSI_TMP_DIR	  :=	/tmp/wargio_naxsi
NGINX_VERS	:= "1.25.0"

NGINX_OPTIONS+="--modules-path=$(NAXSI_TMP_DIR)/modules/"
NGINX_OPTIONS+="--prefix=/etc/nginx"

all: nginx_download configure build install

format_code:
	clang-format --verbose -i $(MOD_PATH)/*.c

clean:
	rm -f "nginx-"$(NGINX_VERS)".tar.gz"
	rm -rf $(NAXSI_TMP_DIR)/
	rm -rf $(NGINX_TMP_DIR)/

nginx_download:
	wget --no-clobber "http://nginx.org/download/nginx-"$(NGINX_VERS)".tar.gz" || exit 1
	mkdir -p $(NGINX_TMP_DIR)/
	tar -C $(NGINX_TMP_DIR)/ -xzf nginx-$(NGINX_VERS).tar.gz  --strip-components=1

configure:
	cd $(NGINX_TMP_DIR)/
	./configure --with-cc-opt="-g3 -ggdb" --with-ld-opt="-lpcre" $(NGINX_OPTIONS) --add-dynamic-module=$(MOD_PATH) --with-compat

build:
	make -C $(NGINX_TMP_DIR)
	mkdir -p $(NAXSI_TMP_DIR)
	if [ -d "$(NAXSI_TMP_DIR)" ] && [ -f $(NGINX_TMP_DIR)/objs/ngx_http_naxsi_module.so ] ; then  cp $(NGINX_TMP_DIR)/objs/ngx_http_naxsi_module.so $(NAXSI_TMP_DIR)/modules/ngx_http_naxsi_module.so ; fi

install:
	make -C $(NGINX_TMP_DIR) install
