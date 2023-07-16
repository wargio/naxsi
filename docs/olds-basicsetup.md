### Initial setup

Let's take the first step to use : setting up learning mode for your website!
This page assumes you already know how to properly configure *nginx* without Naxsi and get it working.

```nginx
#/etc/nginx/nginx.conf :
 user                www-data;
 worker_processes    1;
 worker_rlimit_core  500M;
 working_directory   /tmp/; 
 error_log           /var/log/nginx/error.log;
 pid                 /var/run/nginx.pid; 
 events {
     worker_connections 1024;
     use epoll;
     # multi_accept on;
 }
 http {
     include                        /etc/nginx/naxsi_core.rules;
     include                        /etc/nginx/mime.types;
     server_names_hash_bucket_size  128;
     access_log                     /var/log/nginx/access.log;
     sendfile                       on;
     keepalive_timeout              65;
     tcp_nodelay                    on;
     gzip                           on;
     gzip_disable                   "MSIE [1-6]\.(?!.*SV1)";
     include                        /etc/nginx/sites-enabled/*;
 }
```

Notice the inclusion of the `/etc/nginx/naxsi_core.rules` file. This is the only thing you need to add to your existing `http {}` section if you already have a configuration. The [naxsi_core.rules]( https://github.com/wargio/naxsi/blob/main/naxsi_rules/naxsi_core.rules ) file is provided, and contains Naxsi core rules. As you might notice, these are not signatures, in the classic WAF sense, but simple "score rules", for example:

```
MainRule "str:\"" "msg:double quote" "mz:BODY|URL|ARGS|$HEADERS_VAR:Cookie" "s:$SQL:8,$XSS:8" id:1001;
```

You can see more about rules syntax byt taking a look at the [[rules syntac documentation|rulessyntax]]
Now, let's have a look at my /etc/nginx/site-enabled/default :

```nginx
 server {
     proxy_set_header  Proxy-Connection "";
     listen            *:80;
     access_log        /tmp/nginx_access.log;
     error_log         /tmp/nginx_error.log debug;   
     location / {
          include           /etc/nginx/naxsi.rules;
          proxy_pass        http://x.x.x.x/;
          proxy_set_header  Host www.mysite.com;
     }
     location /RequestDenied {
         return 418;
     }
 }
```

The naxsi's configuration itself is in the file `/etc/nginx/naxsi.rules`:

```ini
 LearningMode;  # Enables learning mode
 SecRulesEnabled;
 #SecRulesDisabled;
 DeniedUrl "/RequestDenied";

 ## check rules
 CheckRule "$SQL >= 8" BLOCK;
 CheckRule "$RFI >= 8" BLOCK;
 CheckRule "$TRAVERSAL >= 4" BLOCK;
 CheckRule "$EVADE >= 4" BLOCK;
 CheckRule "$XSS >= 8" BLOCK;
```

With the following setup :
* Naxsi will be enabled
* Naxsi will not block any requests (while LearningMode is active)
* To-be-blocked requests will generate event logs in your location's error.log file

If you issue a request to  `http://127.0.0.1/?a=<`, you'll get something like this in your logs:

```
2013/05/30 20:09:43 [error] 8404#0:*3 NAXSI_FMT: ip=127.0.0.1&server=127.0.0.1&uri=/&learning=0&vers=0.50&total_processed=3&total_blocked=1&zone0=ARGS&id0=1302&var_name0=a, client: 127.0.0.1, server: , request: "GET /?a=< HTTP/1.0", host: "127.0.0.1"
```

Once you get this kind of lines in your error log, you have naxsi running in LearningMode, congrats! You can now move on to [[generating whitelists|whitelists]]!