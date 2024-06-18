Libinjection is being integrated into naxsi, starting at 0.54rc0

  * https://github.com/nbs-system/naxsi/releases/tag/0.54rc0

We are looking for people to test it and provide feedback on it.

Usage :

  * By default, Libinjection (sql/xss) is disabled, it needs to be enabled at location level using one of these directives : LibInjectionSql / LibInjectionXss (or libinjection_sql / libinjection_xss in nginx style).
  * It can as well be dynamically enabled / disabled using :
```
set $naxsi_libinjection_xss 0|1;
set $naxsi_libinjection_sql 0|1;
```
  * When triggered, libinjection sql/xss will trigger internal rules :
    * libinjection_sql, with internal ID of 17, will increase $LIBINJECTION_SQL of 8
    * libinjection_xss, with internal ID of 18, will increase $LIBINJECTION_XSS of 8

  * If you want to add action uppon trigger, write your CheckRules on those scores ;) (as for other rules, LOG/BLOCK/DROP is allowed!)



