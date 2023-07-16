# naxsi dynamic configuration (aka nginx vars)

Since `0.49`, naxsi supports a limited set of variables that can override or modify its behavior.

 * `naxsi_flag_learning` : If present, this variable will override naxsi learning flag ("0" to disable learning, "1" to enable it).
 * `naxsi_flag_post_action` : If present and set to "0" this variable may be used to disable post_action in learning mode. 
 * `naxsi_flag_enable` : If present, this variable will override naxsi's "SecRulesEnabled" ("0" to disable naxsi, "1" to enable).
 * `naxsi_extensive_log` : If present (and set to "1"), this variable will force naxsi to log the CONTENT of variable matching rules (see notes at bottom).

Since `0.54`, naxsi as well support libinjection enable/disable flags at runtime
 * `naxsi_flag_libinjection_sql`
 * `naxsi_flag_libinjection_xss`

Since `0.56`, naxsi as well support JSON output from blocks/events with enable/disable flags at runtime
 * `naxsi_json_log`

### Gentle reminder
It is important to know that naxsi operates at the REWRITE phase of nginx. Thus, setting those variables directly in the location where naxsi is present is ineffective (as naxsi will be called before variable set is effective). 

This is correct:
 
```
 set $naxsi_flag_enable 0;
 location / {
 ...
 }
```

But this is *wrong*:
```
 location / {
          set $naxsi_flag_learning 1;
  ...
 }
```

With that said, you can use the power of nginx, lua, etc. to change naxsi's behavior. The presence of these variables will enable/disable learning mode, naxsi itself or force extensive logging. 
You can thus do things naxsi is usually not able to, like modifying its behavior according to (nginx) variables set at run-time :

```
 # Disable naxsi if client ip is 127.0.0.1
 if ($remote_addr = "127.0.0.1") {
  set $naxsi_flag_enable 0;
 }
```


Those variables can as well be set from lua scripts (see nginx's mod_lua).

### naxsi_flag_learning

If `naxsi_flag_learning` variable is present, this value will override naxsi's current static configuration regarding learning mode.
 
```
 if ($remote_addr = "1.2.3.4") {
 set $naxsi_flag_learning 1;
 }
 location / {
 ...
 } 
```


### naxsi_flag_post_action

[post_action](http://wiki.nginx.org/HttpCoreModule#post_action) can be used by naxsi to literally forward a request to the [DeniedUrl](directives#deniedurl) location. It is on by default until naxsi 0.50 (a souvenir from ̀nx_intercept`) and is off by default since 0.51, because of the switch to [nxtool](https://github.com/nbs-system/naxsi/tree/master/nxapi).
Using this might lead to unpredictable behavior
Can be set to 0 or 1

### naxsi_flag_enable

If `naxsi_flag_enable` variable is present and set to 0, naxsi will be disabled in this request. This allows you to partially disable naxsi in specific conditions.
To completely disable naxsi for "trusted" users :

```
 set $naxsi_flag_enable 0;
 location / {
 ...
 }
```

### naxsi_extensive_log

If present (and set to “1”), this variable will force naxsi to log the CONTENT of variable matching rules.
Because of a potential impact on performance, use this with caution. Naxsi will log those to nginx’s `error_log`, ie:

```
 NAXSI_EXLOG: ip=%V&server=%V&uri=%V&id=%d&zone=%s&var_name=%V&content=%V
```

See [naxsi logs](naxsilogs) for more details.


### naxsi_flag_libinjection_sql

If set to "1", naxsi will pass every parsed content to [libinjection](libinjection-integration) and ask for SQL injection detection.
If the libinjection matches, internal rule [libinjection_sql is fired ](internal-rules#libinjection_sql).

### naxsi_flag_libinjection_xss

If set to "1", naxsi will pass every parsed content to [libinjection](libinjection-integration) and ask for XSS detection.
If the libinjection matches, internal rule [libinjection_xss is fired ](internal-rules#libinjection_xss).


### naxsi_json_log

If present (and set to “1”), this variable will force naxsi to log in JSON format of events/blocks.
JSON output will be limited to 1948, if it will be more to log, second entry (multiline log) will be empty JSON. Makes no sense to connect big JSONs on multiline.

So if a multiline event is made, I send an empty JSON. You cannot connect JSON across logs lines, this would be very difficult for JSON parsing.

I think this will help in most of cases. 

Multiline log example:

```
2020/07/22 09:24:57 [error] 14714#0: *1 NAXSI_FMT: ip=127.0.0.1&server=localhost&uri=/&vers=0.56&total_processed=1&total_blocked=1&config=learning&cscore0=$SQL&score0=630&zone0=ARGS&id0=1998&var_name0=aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa1&zone1=ARGS&id1=1998&var_name1=aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa2&zone2=ARGS&id2=1998&var_name2=aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa3&zone3=ARGS&id3=1998&var_name3=aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa4&zone4=ARGS&id4=1998&var_name4=aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa5&zone5=ARGS&id5=1998&var_name5=aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa6&zone6=ARGS&id6=1998&var_name6=aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa7&zone7=ARGS&id7=1998&var_name7=aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa8&zone8=ARGS&id8=1998&var_name8=aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa9&zone9=ARGS&id9=1998&var_name9=aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa10&zone10=ARGS&id10=1998&var_name10=aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa11&zone11=ARGS&id11=1998&var_name11=aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa12&zone12=ARGS&id12=1998&var_name12=aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa13&seed_start=185, client: 127.0.0.1, server: localhost, request: "GET /?AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
2020/07/22 09:24:57 [error] 14714#0: *1 NAXSI_FMT: seed_end=185&zone13=ARGS&id13=1998&var_name13=aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa14&zone14=ARGS&id14=1998&var_name14=aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa15, client: 127.0.0.1, server: localhost, request: "GET /?AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA1=1998&AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA2=1998&AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA3=1998&AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA4=1998&AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA5=1998&AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA6=1998&AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA7=1998&AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA8=1998&AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA9=1998&AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA10=1998&AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA11=1998&AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA12=1998&AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA13=1998&AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA14=1998&AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA15=1998 HTTP/1.1", host: "localhost"
As you can see they're connected by "seed_start" and "seed_end".
```

Naxsi will just return empty JSON:

```
2020/07/22 09:24:57 [error] 14714#0: *1 { }, client: 127.0.0.1, server: localhost, request: "GET /?AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA1=1998&AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA2=1998&AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA3=1998&AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA4=1998&AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA5=1998&AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA6=1998&AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA7=1998&AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA8=1998&AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA9=1998&AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA10=1998&AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA11=1998&AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA12=1998&AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA13=1998&AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA14=1998&AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA15=1998 HTTP/1.1", host: "localhost"
2020/07/22 09:24:57 [error] 14714#0: *1 { }, client: 127.0.0.1, server: localhost, request: "GET /?AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA1=1998&AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA2=1998&AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA3=1998&AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA4=1998&AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA5=1998&AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA6=1998&AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA7=1998&AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA8=1998&AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA9=1998&AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA10=1998&AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA11=1998&AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA12=1998&AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA13=1998&AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA14=1998&AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA15=1998 HTTP/1.1", host: "localhost"
```

My use case is DataDog (which parser cannot connect multiple JSON with "seed_start" and "seed_end" values, this would be also very complex to connect and parse in DataDog)

The typical case should look like this (no Multiline log)

```
2020/07/22 09:25:01 [error] 14714#0: *2 { "ip":"127.0.0.1","server":"localhost","uri":"/","vers":"0.56","total_processed":"2","total_blocked":"2","config":"learning","cscore0":"$XSS","score0":"16","zone0":"ARGS","id0":"1302","var_name0":"","zone1":"ARGS","id1":"1303","var_name1":"" }, client: 127.0.0.1, server: localhost, request: "GET /?<script> HTTP/1.1", host: "localhost"
```
