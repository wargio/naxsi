# Libinjection integration

[libinjection](https://github.com/client9/libinjection) is a 3rd party library (developped by client9) aiming at detecting SQL injections (sqli) and cross-site scripting (xss) by tokenization. This library is integrated within naxsi for two purposes :
 - generic detection of xss/sqli
 - virtual patching

### Generic Detection

libinjection generic detection *must* be explicitely enabled using specific directives : [LibInjectionXss](directives#libinjectionxss) or [LibInjectionSql](directives#libinjectionsql). It can as well be enabled at runtime using [runtime modifiers](runtime-modifiers) : `naxsi_flag_libinjection_xss` and `naxsi_flag_libinjection_sql`.

 * Generic libinjection_xss rule has internal id 18 and increases named score `$LIBINJECTION_XSS` of 8 per match.

 * Generic libinjection_sql rule has internal id 17 and increases named score `$LIBINJECTION_SQL` of 8 per match.


A generic setup to block any request triggering libinjection_xss looks like :

```
location / {
 SecRulesEnabled;
 LibInjectionXss;
 CheckRule "$LIBINJECTION_XSS >= 8" BLOCK;
...
}
```

for libinjection_sql :
```
location / {
 SecRulesEnabled;
 LibInjectionSql;
 CheckRule "$LIBINJECTION_SQL >= 8" BLOCK;
...
}
```

When generic detection is enabled, false positives can be whitelisted using id 17 ([libinjection_xss](internal-rules#libinjection_xss)) or 18 ([libinjection_sql](internal-rules#libinjection_sql)).


Using runtime modifiers, it might look like :


```
#/foobar as LOTS of sql injections
if ($request_uri ~ ^/foobar(.*)$ ) {
    set $naxsi_flag_libinjection_sql 1;
}
...
location / {
 ...
 CheckRule "$LIBINJECTION_SQL >= 8" DROP;
 ...
}
```


### Virtual Patching

(>= 0.55rc2)

Widely enabling libinjection might not be possible depending on the application context.
However, libinjection can as well be used for virtual patching :

```
MainRule "d:libinj_xss" "s:DROP" "mz:$ARGS_VAR:ruuu" id:41231;
```


_Pass the content of GET variable 'ruuu' to libinjection, and drop request if it's detected as xss_

```
MainRule "d:libinj_sql" "s:DROP" "mz:$ARGS_VAR:ruuu" id:41231;
```

_DROP any request triggering libinjection_sql in GET variable 'ruuu'_


Using virtual patching approach, user-created rules can be managed/whitelisted without specificities.

