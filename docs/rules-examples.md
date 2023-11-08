Go to [Rules Explanation](rules-bnf.md)


 * [generic rules](#generic-rules)
 * [Blocking "bad" user agents](#blocking-bad-user-agents)
 * [Blocking "bad" referers](#blocking-bad-referers)
 * [Blocking dangerous directories](#blocking-dangerous-directories)
 * [Virtual patching : Simple/Generic XSS](#simplegeneric-xss)
 * [Virtual patching : Simple/Generic (wider) XSS](#simplegeneric-wider-xss)
 * [Virtual patching : Simple/Generic File Upload](#simplegeneric-file-upload)
 * [Raw Body rules](#raw-body)
 * [LibInjection (XSS) Virtual Patching (>= 0.55rc1)](#libinjection-xss-virtual-patching)
 * [LibInjection (SQL) Virtual Patching (>= 0.55rc1)](#libinjection-sql-virtual-patching)
 * [Negative URL rule](#negative-rule)


## 'generic' rules

Search for string `0x` in any POST/PUT arg, any part of the URL, any GET arg, or the HTTP header named `cookie` (extracted from naxsi_core.rules). If rule matches, `$SQL` score is increased by 2. Rule can be whitelisted via id `1002`.

```
MainRule "str:0x" "msg:0x, possible hex encoding" "mz:BODY|URL|ARGS|$HEADERS_VAR:Cookie" "s:$SQL:2" id:1002;
```

## Practices

Sometime, you can write rules to enforce best practices or simply to deter automated attacks.


### Blocking "bad" user agents
```
MainRule "str:w3af.sourceforge.net" "msg:DN SCAN w3af User Agent" "mz:$HEADERS_VAR:User-Agent" "s:$UWA:8" id:42000041 ;  
```

Block w3af user-agent  (http://w3af.org).


### Blocking "bad" referers

```
BasicRule "str:http://www.shadowysite.com/" "msg:Bad referer" "mz:$HEADERS_VAR:referer" "s:DROP" id:20001;
```


### Blocking dangerous directories

ie. following [CVE-2015-2067](http://cve.circl.lu/cve/CVE-2015-2067) on magento's plugin "magmi", you want to block access to the plugin :

```
MainRule "str:/magmi/" "msg:Access to magmi folder" "mz:URL" "s:$UWA:8" id:42000400;
MainRule "str:/magmi.php" "msg:Access to magmi.php" "mz:URL" "s:$UWA:8" id:42000401;
```

## Vpatching Examples

Virtual patching usually aims at protecting a vulnerable software from exploitation. 

### Simple/Generic XSS

There is a reflected XSS in GET variable "foo" on URL "/target" :

```
MainRule id:4242 "str:<" "msg:xss (angle bracket)" "mz:$ARGS_VAR:foo|$URL:/target" s:DROP;
```

This rule will stop any request containing the character '<' at the targeted location.


### Simple/Generic (wider) XSS

There is a reflected XSS in GET variable "foo" on all product URLs :

```
MainRule id:4242 "str:<" "msg:xss (angle bracket)" "mz:$ARGS_VAR_X:^foo$|$URL_X:^/product/[0-9]+/product$" s:DROP;
```


### Simple/Generic File Upload

Blocking asp/php file upload (part of core rules). Increases `$UPLOAD` by 8 if the string uploaded file names contains `ph` (.php / .pht ...) `.asp` or `.ht` (.htaccess ...).

```
MainRule "rx:\.ph|\.asp|\.ht" "msg:asp/php file upload!" "mz:FILE_EXT" "s:$UPLOAD:8" id:1500;
```


### Raw Body

Raw Body zone is meant for the content-types that naxsi can't parse (XML, java serialized objects, unorthodox developments).
See [RAW_BODY](rawbody.md) for details on RAW_BODY behaviour.

```
MainRule "id:4241" "s:DROP" "str:RANDOMTHINGS" "mz:RAW_BODY";
```

### LibInjection (XSS) Virtual Patching

(>= 0.55rc1)

Will drop any request for which libinjection detects content of GET var `foo` as an XSS.

```
MainRule "id:4241" "s:DROP" "d:libinj_xss" "mz:$ARGS_VAR:foo";
```

### LibInjection (SQL) Virtual Patching

(>= 0.55rc1)

Will drop any request for which libinjection detects content of GET var `foo` as an SQLi.

```
MainRule "id:4241" "s:DROP" "d:libinj_sql" "mz:$ARGS_VAR:foo";
```

### Negative rule

Will drop any request for which the URL doesn't start with "/rest/"

```
MainRule "id:4241" negative "s:DROP" "rx:^/rest/" "mz:URL";
```


