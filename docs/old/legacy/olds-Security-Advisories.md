# Security Advisories
_Because nobody's perfect and coding while drunk is dangerous !_

## Reporting Vulnerabilities and Security Issues

[As stated on the project's homepage](https://github.com/nbs-system/naxsi#security-issues), you can send me an email directly. Or, you can use the [issues](https://github.com/nbs-system/naxsi/issues)

## NO-CVE: [Medium-Low] Potential bypass on SQL keywords for IIS/ASP
* Rated : Medium-Low
* Date : 26 March 2013
* Affected : All
* Fixed in : 0.50-1 [r565 (on Google code)](https://code.google.com/p/naxsi/source/detail?r=565) 
* Discovered by : Safe3
* References : [http://seclists.org/bugtraq/2013/Mar/133](http://seclists.org/bugtraq/2013/Mar/133)

Naxsi does not suppress/decode incorrectly url-encoded characters. On the other hand, IIS/ASP will treat "s%e%l%e%c%t" as "select", while naxsi will still see it as "s%e%l%e%c%t", thus rendering SQL keywords rule ineffective. Only the SQL-keywords rule is affected, thus the exploitation window is limited to quote-less, two-fields (max) SQL injections.

## CVE-2012-3380: [Medium] Potential file disclosure in naxsi's nx_extract
* Rated: Medium
* Date 18 May 2012
* Affected: 0.46
* Fixed in: 0.46-1
* Discovered by : Naxsi dev team
* References: [oss-security](http://seclists.org/oss-sec/2012/q3/12) [Secunia](https://secunia.com/advisories/49811) [Securelist](https://www.securelist.com/en/advisories/49811)

Local includes in nx_extract are not properly filtered, allowing a remote attacker to disclose files local to nx_extract.
More details [https://code.google.com/p/naxsi/source/detail?r=307](https://code.google.com/p/naxsi/source/detail?r=307)

## NO-CVE: [Low] SQL Injection in naxsi's nx_intercept
* Rated: Low
* Date: 2 Apr 2012
* Affected: 0.44
* Fixed in: 0.44-1
* Discovered by : Naxsi dev team

Ironically, an SQL Injection is present in naxsi's new python learning daemon (nx_intercept.py). The vulnerability is rated as low, as:
* Learning daemon is usually restricted to trusted IPs
* No sensitive data are present into database, as it is only used to store exceptions.
The vulnerabilty only affects the nx_intercept python daemon, not the naxsi's core. Vulnerable code is:
```python
 if md5 is not None and ip is not None:
  cursor.execute("INSERT INTO http_monitor (peer_ip, md5) VALUES ('%s', '%s')" % (ip, md5))
  return
```
