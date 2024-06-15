### naxsi dynamic configuration (aka nginx vars)

Since [0.49]( https://github.com/wargio/naxsi/tree/0.49 ), naxsi supports a limited set of boolean variables that can be set to `0` or `1` to override or modify its behaviour.

* `naxsi_flag_learning` can override Naxsi learning flag.
* `naxsi_flag_post_action` may be used to disable `post_action` in learning mode.
* `naxsi_flag_enable` could override naxsi's `SecRulesEnabled`.
* `naxsi_extensive_log` can force naxsi to log the `CONTENT` of variable matching rules (see notes at bottom).

### Gentle reminder
It is important to know that naxsi operates at the `REWRITE` phase of nginx. Thus, setting those variables directly in the location where Naxsi is present is ineffective (as naxsi will be called before variable set is effective). 

This is correct:

```nginx 
 set $naxsi_flag_enable 0;
 location / {
 ...
 }
```

But this is wrong:
 
```nginx
 location / {
          set $naxsi_flag_learning 1;
  ...
 }
```

With that said, you can use the power of nginx, lua, etc. to change naxsi's behaviour. The presence of these variables will enable/disable learning mode, naxsi itself or force extensive logging. You can thus do things naxsi is usually not able to, like modifying its behaviour according to (nginx) variables set at run-time :
 
```nginx
 # Disable naxsi if client ip is 127.0.0.1
 if ($remote_addr = "127.0.0.1") {
  set $naxsi_flag_enable 0;
 }
```

Those variables can as well be set from lua scripts (see nginx's [mod_lua]( https://github.com/openresty/lua-nginx-module#readme )).

### naxsi_flag_learning

If the `naxsi_flag_learning` variable is present, this value will override naxsi's current static configuration regarding learning mode.
 
```nginx
 if ($remote_addr = "1.2.3.4") {
 set $naxsi_flag_learning 1;
 }
 location / {
 ...
 }
```

### naxsi_flag_post_action

The [post_action]( http://wiki.nginx.org/HttpCoreModule#post_action ) option can be used by naxsi to literally forward a request to the `DeniedUrl` location. It is on by default until [naxsi 0.50]( https://github.com/wargio/naxsi/tree/0.50 ) (a souvenir from `nx_intercept`) and is off by default since [naxsi 0.51]( https://github.com/wargio/naxsi/tree/0.51 ).

### naxsi_flag_enable
If the `naxsi_flag_enable` variable is present and set to `0`, naxsi will be disabled in this request. This allows you to partially disable naxsi in specific conditions. To completely disable naxsi for "trusted" users :
 
```nginx
 set $naxsi_flag_enable 0;
 location / {
 ...
 }
```

### naxsi_extensive_log
If present and set to `1`, this variable will force naxsi to log the CONTENT of variable matching rules.
Because of a potential impact on performance, use this with caution. Naxsi will log those to nginx’s error_log, ie:
 
```
 NAXSI_EXLOG: ip=%V&server=%V&uri=%V&id=%d&zone=%s&var_name=%V&content=%V
```
