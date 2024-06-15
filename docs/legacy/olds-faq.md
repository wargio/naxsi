## Naxsi blocks all requests but no IDs are written

If you see lines like :
```
2013/09/03 00:11:45 [error] 14913#0: *1 NAXSI_FMT: ip=127.0.0.1&server=localhost&uri=/&learning=1&vers=0.50&total_processed=1&total_blocked=1, client: 127.0.0.1, server: , request: "GET / HTTP/1.0", host: "localhost"
```

It's usually because you forgot to include naxsi rules in your `http {}` block. Try adding `include /etc/nginx/naxsi_core.rules;` to it.

## I compiled naxsi from source, I enabled NAXSI, but it doesn't seem to filter requests?

Did you put naxsi directives first in your location configuration and `./configure`?
You **MUST** put naxsi's directives first in your location configuration. Please check as well your configuration at both `http {}` and `location {}` levels.

## How to limit learning mode to some specific IPs/Users/locations?

Naxsi supports [[DynamicModifiers]] to change behaviour at runtime.
You can as well rely on nginx's [HttpAllowModule]( http://wiki.nginx.org/HttpAccessModule ).
You could also set different vhosts (with associated locations) up, and define some to have `learningmode`, and others without.

## How to test that naxsi is working?

Setup your RequestDenied as follow :

```nginx
location /RequestDenied {
  return 500;
}
```

Check that `learningMode` is disabled, that `naxsi_core.rules` is included, and issue a request like `http:/.../?a=<>`. You should get a 500 from nginx, and get an entry in your nginx error log, starting with `NAXSI_FMT:`.

## I downloaded / installed packages from X, but I don't have this feature / new learning tools?
Naxsi is a young and evolving project and distributions cannot always keep up. Use the source luke, as documented  [[here|installation]].

## Why do you keep radically changing learning tools?
Because it's a not-that-easy problem, and we haven't found a satisfying solution yet. If you have ideas about how we could do it better, please tell us!

## Can I run naxsi on Windows
No one tried that yet, but feel free to go down the [rabbit hole]( http://nginx-win.ecsds.eu/ )