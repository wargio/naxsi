### NAXSI_FMT

NAXSI_FMT are the lines you can see in your error.log :

```
2013/11/10 07:36:19 [error] 8278#0: *5932 NAXSI_FMT: ip=X.X.X.X&server=Y.Y.Y.Y&uri=/phpMyAdmin-2.8.2/scripts/setup.php&learning=0&vers=0.52&total_processed=472&total_blocked=204&block=0&cscore0=$UWA&score0=8&zone0=HEADERS&id0=42000227&var_name0=user-agent, client: X.X.X.X, server: blog.memze.ro, request: "GET /phpMyAdmin-2.8.2/scripts/setup.php HTTP/1.1", host: "X.X.X.X"
```

Here, client `X.X.X.X` request to server `Y.Y.Y.Y` did trigger the rule `42000227` in the var named `user-agent` in the`HEADERS` zone. `id X` might seem obscure, but you can see the meaning in [naxsi_core.rules]( https://github.com/nbs-system/naxsi/blob/master/naxsi_config/naxsi_core.rules ):
 
```
MainRule "str:<" "msg:html open tag" "mz:ARGS|URL|BODY|$HEADERS_VAR:Cookie" "s:$XSS:8" id:1302;
```

NAXSI_FMT is composed of different items :

- `ip` : Client's ip 
- `server` :  Requested Hostname (as seen in http header `Host`) 
- `uri`: Requested URI (without arguments, stops at `?`) 
- `learning`: tells if naxsi was in learning mode (0/1) 
- `vers` : Naxsi version, only since [0.51]( https://github.com/nbs-system/naxsi/tree/0.51 )
- `total_processed`: Total number of requests processed by nginx's worker 
- `total_blocked`: Total number of requests blocked by (naxsi) nginx's worker
- `zoneN`: Zone in which match happened (see "Zones" in the table below) 
- `idN`: The rule id that matched 
- `var_name`: Variable name in which match happened (optional)

Several groups of zone, id, var_name can be present in a single line.

Existing zones can be are the following:

- `ARGS`: GET args
- `$ARGS_VAR `: named GET argument 
- `$ARGS_VAR_X`: regex matching the name of a GET argument
- `HEADERS `: HTTP Headers 
- `$HEADERS_VAR ` : named HTTP header 
- `$HEADERS_VAR_X`: regex matching a named HTTP header 
- `BODY `: POST args (and RAW_BODY)
- `$BODY_VAR `: named POST argument 
- `$BODY_VAR_X`: regex matching the name of a POST argument
- `URL`: The URL (before '?') 
- `$URL`: The specified URL 
- `$URL_X`: regex matching the URL (before '?')
- `FILE_EXT`: Filename (in a multipart POST containing a file) 

A zone can be suffixed with "|NAME", meaning the rule matched in the name of the variable, not its content.

As well, several scores can be present since [0.53]( https://github.com/nbs-system/naxsi/tree/0.53 ):

```ini
cscoreX=[score_tag]
scoreX=[score_value]
```

### NAXSI_EXLOG

NAXSI_EXLOG is a complement to [[naxsilogs]]. Along with exceptions, it contains actual content of the matched request. While NAXSI_FMT only contains IDs and location of exception, NAXSI_EXLOG provides actual content, allowing you to easily decide if it's a false positive or not.

Learning tools uses this at his advantage. Extensive log is enabled by adding the following line in your server {} section but **out** of your location.
 
```javascript
set $naxsi_extensive_log 1;
```

This feature is provided by [[DynamicModifiers]].

```
2013/05/30 20:47:05 [debug] 10804#0:*1 NAXSI_EXLOG: ip=127.0.0.1&server=127.0.0.1&uri=/&id=1302&zone=ARGS&var_name=a&content=a<>bcd
2013/05/30 20:47:05 [error] 10804#0:*1 NAXSI_FMT: ip=127.0.0.1&server=127.0.0.1&uri=/&learning=0&vers=0.50&total_processed=1&total_blocked=1&zone0=ARGS&id0=1302&var_name0=a, client: 127.0.0.1, server: , request: "GET /?a=a<>bcd HTTP/1.0", host: "127.0.0.1"
```

### Naxsi Internal IDs

"User defined" rules are supposed to have IDs > `1000`.

IDs inferior `1000` are reserved for [[naxsi internal rules|embedded_rules]], which are usually related to protocol sanity and things that cannot be expressed through regular expressions or string matches.

Think twice before whitelisting one of those IDs, as it might partially/totally disable naxsi.