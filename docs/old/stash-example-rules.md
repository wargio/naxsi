FIXMEFIXME


# Examples

This will look for the string `Submit=Run` on the url `/script`, with the *POST* variable `Submit` present:
```
MainRule "msg:detection Submit=Run in POST" "str:Submit=Run" "mz:$URL:/script|$BODY_VAR:Submit" "s:$ATTACK" id: 1230001;
```

This will look for accesses on the `/hidden.html` url:
```
MainRule "msg:detection URL-Access" "str:/hidden.html" "mz:URL" "s:$ATTACK" id:1230002;
```

This will detect the string `jjoplmh` in the `cms` *GET* variable:
```
MainRule "str:jjoplmh" "msg:Possible Wordpress-Plugin-Backdoor detected" "mz:$ARGS_VAR:cms" "s:$UWA:8" id:42000347;
```

naxsi_core.rules contains examples of rules. See as well mex's [[Doxi rules | https://bitbucket.org/lazy_dogtown/doxi-rules]] for more rules examples (doxi is third party rules that will focus on emerging threats).

