You can list which IP or CIDR (i.e 192.168.0.1/24), should be always allowed and never blocked.

IP is taken from `X-Forwarder-for` header (when available or from the client IP).

Your load balancer, some proxy could add it already, but if not you can add it with `proxy_set_header X-Forwarded-For $remote_addr;`

Usage:

```
location / {
     SecRulesEnabled;
     IgnoreIP   "1.2.3.4";
     IgnoreCIDR "192.168.0.0/24";
     DeniedUrl "/RequestDenied";
     CheckRule "$SQL >= 8" BLOCK;
     CheckRule "$RFI >= 8" BLOCK;
     CheckRule "$TRAVERSAL >= 4" BLOCK;
     CheckRule "$XSS >= 8" BLOCK;
     root /var/www/html/;
     index index.html index.htm;
}
```