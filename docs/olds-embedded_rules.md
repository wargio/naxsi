
# Internal rules

Since its [0.53 release]( https://github.com/wargio/naxsi/tree/0.53 ), naxsi comes with a predefined set of rules with the following id:

- `1`  - "weird request" : This a generic exception used for improperly formatted requests.
- `2`  - "big request" : Request is too big and has been buffered to disk by nginx.
- `10` - "uncommon hex encoding" : Encoding suggests this might be an escape attempt.
- `11` - "uncommon content-type" : Content-type of BODY is unknown / cannot be parsed.
- `12` - "uncommon URL" : URL is malformed
- `13` - "uncommon post format" : malformed boundary or content-disposition
- `14` - "uncommon post boundary" : BODY boundary line is malformed, or boundary breaks RFC
- `15` - invalid JSON - gets parsed when application/json is detected (experimental as of summer 2014)
- `16` - "empty body" : POST with empty BODY, available since [naxsi 0.53-1]( https://github.com/wargio/naxsi/tree/0.53 ), was merged with `id:11` before.
- `17` - "Libinjection SQL" : Libinjection SQL detection was triggered.
- `18` - "Libinjection XSS" : Libinjection XSS detection was triggered.


## naxsi-core.rules

Naxsi ships with a [basic core-rule-set]( https://github.com/wargio/naxsi/blob/main/naxsi_rules/naxsi_core.rules )
that protects against common attacks. Those Core-Rules should always be loaded.

- [SQL-Injections]( https://www.owasp.org/index.php/SQL_Injection ) (1000-1099)
- Obvious [Remote File Inclusions]( https://www.owasp.org/index.php/OWASP_Periodic_Table_of_Vulnerabilities_-_Remote_File_Inclusion ) (1100-1199)
- [Directory Traversal]( https://www.owasp.org/index.php/Path_Traversal ) (1200-1299)
- [Cross Site Scripting]( https://www.owasp.org/index.php/XSS ) (1300-1399)
- [Basic Evading tricks]( https://www.owasp.org/index.php/XSS_Filter_Evasion_Cheat_Sheet ) (1400-1500)
- [File uploads]( https://www.owasp.org/index.php/Unrestricted_File_Upload ) (1500-1600)
