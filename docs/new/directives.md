**NAXSI Directives**
=====================

### Mandatory Directives
------------------------

* **SecRulesEnabled**: Enables NAXSI in a location.
* **LearningMode**: If enabled, does not honor the `BLOCK` directive and allows learning mode to take effect instead.

 Rule-Based Directives
----------------------

* **CheckRule**: Takes an action (LOG, BLOCK, DROP, ALLOW) based on a specific score associated with the request. Example: `CheckRule "$SQL >= 8" BLOCK;`

 LibInjection Directives
-------------------------

* **LibInjectionXss**: Enables libinjection's XSS detection feature.
* **LibInjectionSql**: Enables libinjection's SQLI detection feature.

 Other Directives
-----------------

* **DeniedUrl**: Specifies where blocked requests will be redirected.
* `BasicRule` and `MainRule`: Used to declare rules or whitelists that work at the location level ( BasicRule ) or globally ( MainRule ).
