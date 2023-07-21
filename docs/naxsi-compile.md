*Note: This process is intended to run on a fully up-to-date Debian Stretch, that follows [installation best practices](https://www.ssi.gouv.fr/guide/recommandations-de-securite-relatives-a-un-systeme-gnulinux/)*

### Build dependencies

- GCC
- make
- nginx :)
- libpcre3-dev

### Get naxsi

You can download naxsi from the releases page [here](https://github.com/wargio/naxsi/releases)
Get the latest release source code and the corresponding signature file : 

```shell
usr@008885ac189c:~$ export NAXSI_VER=1.3
usr@008885ac189c:~$ wget https://github.com/wargio/naxsi/releases/download/$NAXSI_VER/naxsi-$NAXSI_VER-src-with-deps.tar.gz -O naxsi-$NAXSI_VER-src-with-deps.tar.gz
```

### Get Nginx

Next, you need to get the source code for the nginx version you are running (in this example, we are using the nginx from the debian stretch repository) : 

```shell
usr@008885ac189c:~$ export NGINX_VER=X.YY.Z
usr@008885ac189c:~$ wget https://nginx.org/download/nginx-$NGINX_VER.tar.gz
usr@008885ac189c:~$ wget https://nginx.org/download/nginx-$NGINX_VER.tar.gz.asc
usr@008885ac189c:~$ gpg --recv-key 520A9993A1C052F8
usr@008885ac189c:~$ gpg --verify nginx-$NGINX_VER.tar.gz.asc
usr@008885ac189c:~$ rm nginx-$NGINX_VER.tar.gz.asc 
```

If the signature fails, check that both files have been correctly downloaded and are not corrupted.

### Build naxsi as a dynamic extension

First, extract the source code : 
```shell
usr@008885ac189c:~$ mkdir -p naxsi-$NAXSI_VER
usr@008885ac189c:~$ tar -C naxsi-$NAXSI_VER -xzf naxsi-$NAXSI_VER-src-with-deps.tar.gz
usr@008885ac189c:~$ tar vxf nginx-$NGINX_VER.tar.gz 
usr@008885ac189c:~$ cd nginx-$NGINX_VER  
usr@008885ac189c:~/nginx-X.YY.Z$ ./configure --add-dynamic-module=../naxsi-$NAXSI_VER/naxsi_src/
usr@008885ac189c:~/nginx-X.YY.Z$ make modules
```
The resulting `ngx_http_naxsi_module.so` will be located in the `objs` directory.
Copy it to your nginx server, in `/etc/nginx/modules/`.
Also copy `naxsi_core.rules` to `/etc/nginx/`.


### Build naxsi as a dynamic extension for nginx from your distribution package (i.e Ubuntu)

You need to build the module with the same flags as your NGINX server package was compiled for example via `apt-get source nginx`

Execute this to list them:
```shell    
usr@008885ac189c:~$ nginx -V 
```
Then pass them to `./configure`.

```shell    
usr@008885ac189c:~/nginx-X.YY.Z$ ./configure <FLAGS FROM ABOVE COMMAND> --add-dynamic-module=../naxsi-$NAXSI_VER/naxsi_src/
usr@008885ac189c:~/nginx-X.YY.Z$ make modules
```
This will ensure module will be compatible with your nginx and will load properly.


### Basic setup

#### Main configuration

You need to tell nginx to load the naxsi module and naxsi core rules (if you don't load them, naxsi will block every requests) : 

```shell
usr@008885ac189c:~$ cat /etc/nginx/nginx.conf
...
load_module /etc/nginx/modules/ngx_http_naxsi_module.so; # load naxsi

http {
    include /etc/nginx/naxsi_core.rules; # load naxsi core rules
    ...
}
...

```

Naxsi works on a per-location basis, meaning you can only enable it inside a location : 
```python 
server {
...

    location / { # naxsi is enabled, and in learning mode

        SecRulesEnabled; #enable naxsi
        LearningMode; #enable learning mode
        LibInjectionSql; #enable libinjection support for SQLI
        LibInjectionXss; #enable libinjection support for XSS

        DeniedUrl "/RequestDenied"; #the location where naxsi will redirect the request when it is blocked
        CheckRule "$SQL >= 8" BLOCK; #the action to take when the $SQL score is superior or equal to 8
        CheckRule "$RFI >= 8" BLOCK;
        CheckRule "$TRAVERSAL >= 5" BLOCK;
        CheckRule "$UPLOAD >= 5" BLOCK;
        CheckRule "$XSS >= 8" BLOCK;


        proxy_pass http://127.0.0.1;
        ....
    }

    location /admin { # naxsi is disabled 

        SecRulesDisabled; #optional, naxsi is disabled by default
        
        allow 1.2.3.4;
        deny all;
        proxy_pass http://127.0.0.1;
        ....
    }

    location /vuln_page.php { # naxsi is enabled, and is *not* in learning mode

        SecRulesEnabled;
        proxy_pass http://127.0.0.1;
    }
    
    location /RequestDenied {
        internal;
        return 403;
    }
...

}
```

#### Whitelist

As naxsi uses a whitelist approach, a lot of false positives may be generated, potentially dropping legitimate requests.
To prevent this, whitelists must be written (either manually or with [nx-tool](https://github.com/nbs-system/nxtool-ng)).
For example, if you have an e-commerce website that sells furniture, people will be likely to search for something like `table`. Unfortunately, `table` is also a SQL keyword, which will trigger naxsi.
To prevent this, you can write a whitelist telling naxsi to allow the `table` keyword in the search form (assuming the search form in on `/search`) : 

```python
server {
    
    location / {
        SecRulesEnabled; #enable naxsi
        LearningMode; #enable learning mode
        LibInjectionSql; #enable libinjection support for SQLI
        LibInjectionXss; #enable libinjection support for XSS

        DeniedUrl "/RequestDenied"; #the location where naxsi will redirect the request when it is blocked
        CheckRule "$SQL >= 8" BLOCK; #the action to take when the $SQL score is superior or equal to 8
        CheckRule "$RFI >= 8" BLOCK;
        CheckRule "$TRAVERSAL >= 5" BLOCK;
        CheckRule "$UPLOAD >= 5" BLOCK;
        CheckRule "$XSS >= 8" BLOCK;

        BasicRule wl:1000 "mz:$URL:/search|$ARGS_VAR:q";
        proxy_pass http://127.0.0.1;
        ....
        
    }    
    
}
```

See [here](whitelists-bnf) and [here](whitelists-examples) for more informations about whitelists.

#### Blacklist

You can also create blacklist with naxsi, allowing you to drop requests even when  in learning mode.
Let's imagine a PHP script (`/vuln_page.php`) vulnerable to an unquoted SQLI in the `id` parameter, a typical exploitation would look like this : 
```
GET /vuln_page.php?id=1'+or+1=1/*
```
You can use naxsi to virtually patch this vulnerabilty by adding a blacklist denying every requests on `/vuln_page.php` where the `id` parameter contains anything other than a number (in this example, the blacklist will only apply to the `/` location, you can use `MainRule` and put it outside in the `http` block to make it global):

```python
server {
    
    location / {
        SecRulesEnabled; #enable naxsi
        LearningMode; #enable learning mode
        LibInjectionSql; #enable libinjection support for SQLI
        LibInjectionXss; #enable libinjection support for XSS

        DeniedUrl "/RequestDenied"; #the location where naxsi will redirect the request when it is blocked
        CheckRule "$SQL >= 8" BLOCK; #the action to take when the $SQL score is superior or equal to 8
        CheckRule "$RFI >= 8" BLOCK;
        CheckRule "$TRAVERSAL >= 5" BLOCK;
        CheckRule "$UPLOAD >= 5" BLOCK;
        CheckRule "$XSS >= 8" BLOCK;

        BasicRule id:4242 "mz:$URL:/vuln_page.php|$ARGS_VAR:id" "rx:[^\d]+" "s:DROP" "msg:blacklist for SQLI in /vuln_page.php";
        proxy_pass http://127.0.0.1;
        ....
        
    }    
    
}
```
